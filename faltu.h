#include <getopt.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <dirent.h>
#include <libintl.h>
#include <ctype.h>
#include <assert.h>
#include <locale.h>
#include <time.h>
#include <limits.h>
#include <sys/stat.h>

#include "powerbooster.h"


uint64_t start_usage[8], start_duration[8];
uint64_t last_usage[8], last_duration[8];
char cnames[8][16];

double ticktime = 15.0;

int interrupt_0, total_interrupt;

int showpids = 0;

static int maxcstate = 0;
int topcstate = 0;

int dump = 0;

#define IRQCOUNT 150

struct irqdata {
	int active;
	int number;
	uint64_t count;
};

struct irqdata interrupts[IRQCOUNT];

#define FREQ_ACPI 3579.545
static unsigned long FREQ;

int nostats;


struct line	*lines;
int		linehead;
int		linesize;
int		linectotal;


double last_bat_cap = 0;
double prev_bat_cap = 0;
time_t last_bat_time = 0;
time_t prev_bat_time = 0;

double displaytime = 0.0;


char cstate_lines[12][200];

void push_line(char *string, int count)
{
	int i;

	assert(string != NULL);
	for (i = 0; i < linehead; i++)
		if (strcmp(string, lines[i].string) == 0) {
			lines[i].count += count;
			return;
		}
	if (linehead == linesize)
		lines = realloc (lines, (linesize ? (linesize *= 2) : (linesize = 64)) * sizeof (struct line));
	memset(&lines[linehead], 0, sizeof(&lines[0]));
	lines[linehead].string = strdup (string);
	lines[linehead].count = count;
	lines[linehead].disk_count = 0;
	lines[linehead].pid[0] = 0;
	linehead++;
}

void push_line_pid(char *string, int cpu_count, int disk_count, char *pid)
{
	int i;
	assert(string != NULL);
	assert(strlen(string) > 0);
	for (i = 0; i < linehead; i++)
		if (strcmp(string, lines[i].string) == 0) {
			lines[i].count += cpu_count;
			lines[i].disk_count += disk_count;
			if (pid && strcmp(lines[i].pid, pid)!=0)
				lines[i].pid[0] = 0;
			return;
		}
	if (linehead == linesize)
		lines = realloc (lines, (linesize ? (linesize *= 2) : (linesize = 64)) * sizeof (struct line));
	memset(&lines[linehead], 0, sizeof(&lines[0]));
	lines[linehead].string = strdup (string);
	lines[linehead].count = cpu_count;
	lines[linehead].disk_count = disk_count;
	if (pid)
		strcpy(lines[linehead].pid, pid);
	linehead++;
}

void clear_lines(void)
{
	int i;
	for (i = 0; i < linehead; i++)
		free (lines[i].string);
	free (lines);
	linehead = linesize = 0;
	lines = NULL;
}

void count_lines(void)
{
	uint64_t q = 0;
	int i;
	for (i = 0; i < linehead; i++)
		q += lines[i].count;
	linectotal = q;
}
