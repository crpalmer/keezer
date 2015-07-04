#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "lib/mem.h"
#include "lib/piface.h"

#define INVALID_LOW 32
#define INVALID_HIGH 185

#define ASD_SECS 60
#define SLEEP_SECS 15

piface_t *piface;
double temperature = INVALID_LOW;
double fridge_temperature = INVALID_LOW;
double current_target;
int is_on = -1;
time_t last_off = 0;

typedef enum { TYPE_DOUBLE, TYPE_STRING } type_t;

static const char *temperature_fname = NULL;
static const char *fridge_temperature_fname = NULL;
static double target_temperature = 60;
static double delta_above = 0.5, delta_below = 0.5;
static double pid_threshold = 0;
static double pid_factor = 2;
static double minimum_temperature = 33;

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
    printf("%lu %.3f %d %.3f %.3f\n", (unsigned long) time(NULL), current_target, is_on, temperature, fridge_temperature);
    fflush(stdout);
}

static void
act_on_temperature_set_point(double temp, double target)
{
    int new_on = -1;

    if (is_on < 0) {
	if (temp > target + delta_above) new_on = 1;
	else new_on = 0;
    } else {
	if (is_on && temp < target - delta_below) new_on = 0;
	if (! is_on && temp > target + delta_above) new_on = 1;
    }

    if (new_on > 0 && time(NULL) - last_off < ASD_SECS) {
	fprintf(stderr, "ASD triggered\n");
    } else if (new_on >= 0) {
	is_on = new_on;
	piface_set(piface, 0, is_on == 1);
	if (! is_on) last_off = time(NULL);
    }
}

static double
select_fridge_temperature(double *temp)
{
    double target;
    double delta;

    if (! temperature_is_valid(fridge_temperature)) {
	*temp = temperature;
	return target_temperature;
    } else {
	*temp = fridge_temperature;
    }
    delta = temperature - target_temperature;
    if (delta < pid_threshold) return target_temperature;
    target = target_temperature - delta * pid_factor;
    if (target < minimum_temperature) return minimum_temperature;
    return target;
}

static void
act_on_temperature()
{
    double temp;

    current_target = select_fridge_temperature(&temp);
    act_on_temperature_set_point(temp, current_target);
}

int
main(int argc, char **argv)
{
    if (argc != 2) {
	fprintf(stderr, "usage: config-file\n");
	exit(1);
    }

    read_parameters(argv[1]);

    piface = piface_new();
    if (! piface) {
	fprintf(stderr, "failed to initialize piface\n");
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
