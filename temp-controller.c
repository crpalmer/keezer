#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "../lib/mem.h"
#include "../lib/piface.h"

#define INVALID_LOW 32
#define INVALID_HIGH 185

#define ASD_SECS 60
#define SLEEP_SECS 15
#define RANGE 0.5

piface_t *piface;
double temperature = INVALID_LOW;
int is_on = -1;
time_t last_off = 0;

typedef enum { TYPE_DOUBLE, TYPE_STRING } type_t;

static const char *temperature_fname = NULL;
static double target_temperature = 60;

static const struct {
    const char *name;
    type_t      type;
    void       *value;
} params[] = {
    { "temperature_filename",	TYPE_STRING,	&temperature_fname },
    { "target_temperature",	TYPE_DOUBLE,	&target_temperature },
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

static void
read_temperature()
{
    double new_temp;
    FILE *f = fopen(temperature_fname, "r");

    if (f == NULL) {
	perror(temperature_fname);
	exit(1);
    }

    if (fscanf(f, "%lf", &new_temp) != 1) {
	fprintf(stderr, "warning: failed to read temperature from %s\n", temperature_fname);
    } else if (new_temp > INVALID_LOW && new_temp < INVALID_HIGH) {
	temperature = new_temp;
    } else {
	fprintf(stderr, "warning: invalid temp read: %f\n", new_temp);
    }

    fclose(f);
}

static void
log_temperature()
{
    printf("%lu %d %.3f\n", (unsigned long) time(NULL), is_on, temperature);
    fflush(stdout);
}

static void
act_on_temperature()
{
    int new_on = -1;

    if (is_on < 0) {
	if (temperature > target_temperature + RANGE) new_on = 1;
	else new_on = 0;
    } else {
	if (is_on && temperature < target_temperature - RANGE) new_on = 0;
	if (! is_on && temperature > target_temperature + RANGE) new_on = 1;
    }

    if (new_on > 0 && time(NULL) - last_off < ASD_SECS) {
	fprintf(stderr, "ASD triggered\n");
    } else if (new_on >= 0) {
	is_on = new_on;
	piface_set(piface, 0, is_on == 1);
	if (! is_on) last_off = time(NULL);
    }
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

    while (1) {
	read_temperature();
	act_on_temperature();
	log_temperature();
	sleep(SLEEP_SECS);
   }

   return 0;
}
