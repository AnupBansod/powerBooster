#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <dirent.h>
#include "powerbooster.h"
void activate_usb_autosuspend(void)
{
	DIR *dir;
	struct dirent *dirent;
	FILE *file;
	char filename[PATH_MAX];
	int len;
	char linkto[PATH_MAX];
	dir = opendir("/sys/bus/usb/devices");
	if (!dir)
		return;

	while ((dirent = readdir(dir))) {
		if (dirent->d_name[0]=='.')
			continue;

		/* skip usb input devices */
		sprintf(filename, "/sys/bus/usb/devices/%s/driver", dirent->d_name);
		memset(linkto, 0, sizeof(linkto));
		len = readlink(filename, linkto, sizeof(link) - 1);
		if (strstr(linkto, "usbhid"))
			continue;

		sprintf(filename, "/sys/bus/usb/devices/%s/power/control", dirent->d_name);
		file = fopen(filename, "w");
		if (!file)
			continue;
		fprintf(file, "auto\n");
		fclose(file);
	}

	closedir(dir);
}

void suggest_usb_autosuspend(void)
{
	DIR *dir;
	struct dirent *dirent;
	FILE *file;
	char filename[PATH_MAX];
	char line[1024];
	int len;
	char linkto[PATH_MAX];
	int need_hint = 0;

	memset(linkto, 0, sizeof(linkto));

	dir = opendir("/sys/bus/usb/devices");
	if (!dir)
		return;

	while ((dirent = readdir(dir))) {
			if (dirent->d_name[0]=='.')
				continue;
	
			/* skip usb input devices */
			sprintf(filename, "/sys/bus/usb/devices/%s/driver", dirent->d_name);
			len = readlink(filename, linkto, sizeof(link) - 1);
			if (strstr(linkto, "usbhid"))
				continue;
	
			sprintf(filename, "/sys/bus/usb/devices/%s/power/control", dirent->d_name);
			file = fopen(filename, "r");
			if (!file)
				continue;
			memset(line, 0, 1024);
			if (fgets(line, 1023,file)==NULL) {
				fclose(file);
				continue;
			}
			if (strstr(line, "on"))
				need_hint = 1;
	
			fclose(file);
	}

	closedir(dir);

	if (need_hint) {
				printf("\n\nCurrently, USB is not in AUTO-SUSPEND mode\n\t->  USB turned to AUTO-SUSPEND mode.");
				activate_usb_autosuspend();	
			}
}
