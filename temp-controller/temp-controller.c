#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "lib/mem.h"
#include "lib/gpio.h"

#define INVALID_LOW 32
#define INVALID_HIGH 185

#define ASD_SECS 60
#define SLEEP_SECS 15

gpio_t *gpio;
double temperature = INVALID_LOW;
double fridge_temperature = INVALID_LOW;
double action_temperature = INVALID_LOW;
double current_target;
double asd_seconds = ASD_SECS;
int is_on = -1;
time_t last_off = 0;

typedef enum { TYPE_BOOLEAN, TYPE_DOUBLE, TYPE_STRING } type_t;

static const char *temperature_fname = NULL;
static const char *fridge_temperature_fname = NULL;
static double target_temperature = 60;
static double delta_above = 0.5, delta_below = 0.5;
static double pid_threshold = 0;
static double pid_factor = 2;
static double minimum_temperature = 33;
static bool heater_mode = false;
static bool single_temperature = false;

static const struct {
    const char *name;
    type_t      type;
    void       *value;
} params[] = {
    { "temperature_filename",	TYPE_STRING,	&temperature_fname },
    { "fridge_temperature_filename", TYPE_STRING, &fridge_temperature_fname },
    { "target_temperature",	TYPE_DOUBLE,	&target_temperature },
    { "delta_above",		TYPE_DOUBLE,	&delta_above },
    { "delta_below",		TYPE_DOUBLE,	&delta_below },
    { "pid_threshold",		TYPE_DOUBLE,	&pid_threshold },
    { "pid_factor",		TYPE_DOUBLE,	&pid_factor },
    { "minimum_temperature",	TYPE_DOUBLE,	&minimum_temperature },
    { "heater_mode",		TYPE_BOOLEAN,	&heater_mode },
    { "single_temperature",	TYPE_BOOLEAN,	&single_temperature },
    { "asd_seconds",		TYPE_DOUBLE,	&asd_seconds },
};

#define FRIDGE_ID 0
static gpio_table_t gpio_table[] = {
    { "fridge", 17, 0 }
};

#define N_PARAMS (sizeof(params) / sizeof(params[0]))

static void
read_parameters(const char *fname)
{
    FILE *f = fopen(fname, "r");
    char line[1024];

    if (! f) {
	perror(fname);
	exit(1);
    }

    while (fgets(line, sizeof(line)-1, f) != NULL) {
	const char *name, *value;
	char *p;
	int i;

	line[sizeof(line)-1] = '\0';
	for (p = line; *p && isspace(*p); p++) { }
	name = p;
	while (*p && ! isspace(*p)) p++;
	if (*p) *p++ = '\0';
	while (*p && isspace(*p)) p++;
	value = p;
	while (*p && ! isspace(*p)) p++;
	*p = '\0';

	for (i = 0; i < N_PARAMS; i++) {
	    if (strcmp(name, params[i].name) == 0) {
		switch(params[i].type) {
		case TYPE_STRING:
		     free(*(void **) params[i].value);
		     *(char **) params[i].value = fatal_strdup(value);
		     break;
		case TYPE_DOUBLE:
		    *(double *) params[i].value = atof(value);
		    break;
		case TYPE_BOOLEAN:
		    if (strcmp(value, "true") == 0) *(bool *) params[i].value = true;
		    else if (strcmp(value, "false") == 0) *(bool *) params[i].value = false;
		    else printf("Invalid boolean [%s] for [%s]\n", name, value);
		    break;
	        }
	    }
	}
    }

    fclose(f);
}

static bool
temperature_is_valid(double temp)
{
    return temp > INVALID_LOW && temp < INVALID_HIGH;
}

static void
read_temperature(const char *fname, double *temp)
{
    double new_temp;
    FILE *f;

    if (! fname) return;

    if ((f = fopen(fname, "r")) == NULL) {
	perror(fname);
	exit(1);
    }

    if (fscanf(f, "%lf", &new_temp) != 1) {
	fprintf(stderr, "warning: failed to read temperature from %s\n", fname);
    } else if (temperature_is_valid(new_temp)) {
	*temp = new_temp;
    } else {
	fprintf(stderr, "warning: invalid temp read: %f\n", new_temp);
    }

    fclose(f);
}

static void
log_temperature()
{
    printf("%lu %.3f %d %.3f %.3f %.3f\n", (unsigned long) time(NULL), current_target, is_on, temperature, fridge_temperature, action_temperature);
    fflush(stdout);
}

static int
action_for_fridge(double temp, double target)
{
    int new_on = -1;

    if (is_on < 0) {
	if (temp > target + delta_above) new_on = 1;
	else new_on = 0;
    } else {
	if (is_on && temp < target - delta_below) new_on = 0;
	if (! is_on && temp > target + delta_above) new_on = 1;
    }

    return new_on;
}

static int
action_for_heater(double temp, double target)
{
    int new_on = -1;

    if (is_on < 0) {
	if (temp < target - delta_below) new_on = 1;
	else new_on = 0;
    } else {
	if (is_on && temp > target + delta_above) new_on = 0;
	if (! is_on && temp < target - delta_below) new_on = 1;
    }

    return new_on;
}

static void
act_on_temperature_set_point(double temp, double target)
{
    int new_on;

    if (heater_mode) new_on = action_for_heater(temp, target);
    else new_on = action_for_fridge(temp, target);

    if (new_on > 0 && time(NULL) - last_off < asd_seconds) {
	fprintf(stderr, "ASD triggered\n");
    } else if (new_on >= 0 && new_on != is_on) {
	is_on = new_on;
	gpio_set_id(gpio, FRIDGE_ID, is_on == 1);
	if (! is_on) last_off = time(NULL);
	printf("%ld %s is now %s\n", time(NULL), heater_mode ? "heater" : "fridge", is_on ? "ON" : "OFF");
	fflush(stdout);
    }
}

static double
select_fridge_temperature_dual(double *temp)
{
    double target;
    double delta;
    double delta_sign = heater_mode ? -1 : +1;

    if (! temperature_is_valid(fridge_temperature)) {
	*temp = temperature;
	return target_temperature;
    } else {
	*temp = fridge_temperature;
    }
    delta = delta_sign * (*temp - target_temperature);
    if (delta < pid_threshold) return target_temperature;
    target = target_temperature - delta_sign * delta * pid_factor;
    if (target < minimum_temperature) return minimum_temperature;
    return target;
}

static double
select_fridge_temperature_single(double *temp)
{
    int n = 0;
    double T = 0;

    if (temperature_is_valid(temperature)) {
	T += temperature;
	n ++;
    }

    if (temperature_is_valid(fridge_temperature)) {
	T += fridge_temperature;
	n ++;
    }

    if (! n) {
	*temp = target_temperature;
    } else {
	*temp = T / n;
    }

    return target_temperature;
}

static double
select_fridge_temperature(double *temp)
{
    if (single_temperature) return select_fridge_temperature_single(temp);
    else return select_fridge_temperature_dual(temp);
}

static void
act_on_temperature()
{
    current_target = select_fridge_temperature(&action_temperature);
    act_on_temperature_set_point(action_temperature, current_target);
}

int
main(int argc, char **argv)
{
    if (argc != 2) {
	fprintf(stderr, "usage: config-file\n");
	exit(1);
    }

    read_parameters(argv[1]);

    if ((gpio = gpio_new(gpio_table, 1)) == NULL) {
	fprintf(stderr, "Failed to initialize gpios\n");
	exit(1);
    }

    while (1) {
	read_temperature(temperature_fname, &temperature);
	read_temperature(fridge_temperature_fname, &fridge_temperature);
	act_on_temperature();
	log_temperature();
	sleep(SLEEP_SECS);
   }

   return 0;
}
