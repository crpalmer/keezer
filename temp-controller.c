#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
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

static void
read_temperature(const char *fname)
{
    double new_temp;
    FILE *f = fopen(fname, "r");

    if (f == NULL) {
	perror(fname);
	exit(1);
    }

    if (fscanf(f, "%lf", &new_temp) != 1) {
	fprintf(stderr, "warning: failed to read temperature from %s\n", fname);
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
act_on_temperature(double set_point)
{
    int new_on = -1;

    if (is_on < 0) {
	if (temperature > set_point + RANGE) new_on = 1;
	else new_on = 0;
    } else {
	if (is_on && temperature < set_point - RANGE) new_on = 0;
	if (! is_on && temperature > set_point + RANGE) new_on = 1;
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
    double set_point;

    if (argc != 3) {
	fprintf(stderr, "usage: path-to-temperature set-point\n");
	exit(1);
    }

    if (sscanf(argv[2], "%lf", &set_point) != 1) {
	fprintf(stderr, "invalid set-point %s\n", argv[2]);
	exit(1);
    }

    piface = piface_new();

    while (1) {
	read_temperature(argv[1]);
	act_on_temperature(set_point);
	log_temperature();
	sleep(SLEEP_SECS);
   }

   return 0;
}
