/*
POWERBOOSTER
Low Power Consumption Policies for MeeGo Operating System

Authors:
	Bansod Anup   <anup.bansod@gmail.com>
	Bhute Moheet  <moheet.bhute@gmail.com>
	Chavan Manish <tomeetmanish@gmail.com>
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include "powerbooster.h"
#include "faltu.h"

struct device_info bluetooth;
struct device_info wifi;
struct device_info usb;
struct device_info alsa;
struct device_info ahci;
struct device_info ac97;
struct device_info hda;
struct device_info writeback;
struct device_node *ondemand;
struct device_info ethernet;
struct device_info noatime;
struct device_info watchdog;

//********************************************BLUETOOTH***********************************************************
int bluetoothCount=20, bluetoothInteraction=0,bluetoothFlag;
void *bluetoothTimer(void)
{
	while(!kbhit())
	{
		while(bluetoothCount!=0)
		{
			sleep(1);
			sprintf(bluetooth.remark,"BLUETOOTH IS IDLE");
			if(bluetoothInteraction)
			{
				bluetoothCount=20;
				sprintf(bluetooth.remark,"DATA IS BEING EXCHANGED 1");					
				bluetoothInteraction=0;
			}
			bluetoothCount--;
			bluetooth.remtime=bluetoothCount;
			bluetoothFlag=1;
		}
		if(bluetoothFlag)
		{
			turn_bluetooth_off();
			bluetoothFlag=0;
		}
		sleep(4);
		
	}
}

void *bluetooth_care_taker(void)
{
	printf("\nbluetooth is being taken care of....\n");
	sprintf(bluetooth.devname,"BLUETOOTH");
	while(!kbhit())
	{
		suggest_bluetooth_off();
		sleep(5);
		
	}
	printf("thread for bluetooth care taker is exiting....");
}

//*************************************************WIFI***********************************************************
int wifiCount=0, wifiInteraction=0, wifiFlag;
void *wifiTimer(void)
{
	while(!kbhit())
	{
		while(wifiCount!=0)
		{
			sleep(1);
						
			sprintf(wifi.remark,"WIFI INTERACE IS TURNED OFF");
			if(wifiInteraction)
			{
				wifiCount=10;
				sprintf(wifi.remark,"DATA IS BEING EXCHANGED");
				wifiInteraction=0;
			}
			wifiCount--;
			wifiFlag=1;
			wifi.remtime=wifiCount;
		}
		if(wifiFlag)
		{
			dummy_activate_down_suggestion();
			wifiFlag=0;
		}
		sleep(4);
		
	}
	printf("thread for wifiTIMER is exiting....");
}

void *wifi_care_taker(void)
{
	sprintf(wifi.devname,"WIFI");
	while(!kbhit())
	{
		suggest_wifi_new_powersave();
		sleep(4);
	}
}


void *usb_care_taker(void)
{
	printf("\nUSB is being taken care of....\n");
	while(!kbhit())
	{
		suggest_usb_autosuspend();
		sleep(4);
	}

}


void *ondemand_care_taker(void)
{
	printf("\nOndemand is being taken care of....\n");
	while(!kbhit())
	{
		suggest_ondemand_governor();
		sleep(4);	
	}
}

void *other_care_taker()
{
	count_usb_urbs();
	count_usb_urbs();
	do_alsa_stats();
	do_alsa_stats();
	do_ahci_stats();
	do_ahci_stats();

	while(!kbhit())
	{
		count_usb_urbs();
		do_alsa_stats();
		do_ahci_stats();
		suggest_ac97_powersave();
		suggest_hda_powersave();
		suggest_ondemand_governor();
		suggest_noatime();
		suggest_sata_alpm();
		suggest_writeback_time();
		suggest_usb_autosuspend();
		usb_activity_hint();
		alsa_activity_hint();
		ahci_activity_hint();
		sleep(4);
	}
}

void *displayInfo(void)
{
	struct device_node *node1;
	FILE *file;
	char *which;
	char buffer[1024];
	file = fopen("/proc/cpuinfo","r");
	if (!file)
		return;

	while(!kbhit())
	{
		system("clear");
	//	printf("\n%s : %d : %s : %s\n",bluetooth.devname,bluetooth.remtime,bluetooth.activity,bluetooth.remark);
	//	printf("\n%s : %d : %s : %s\n",wifi.devname,wifi.remtime,wifi.activity,wifi.remark);
		
		node1=ondemand->next;
		printf("\nHurray0\n");
		while(node1!=NULL)
		{
			printf("\nHurray1\n");
			printf("%s : : %s @ ",node1->devinfo.devname,node1->devinfo.remark);
		/*	while (!feof(file)) 
			{
				memset(buffer, 0, 1024);
				if (!fgets(buffer, 1023, file))
					break;
				printf("\nHurray2\n");
				if ((which=strstr(buffer, "cpu MHz")))
				{
					printf("\nCPU FREQUENCY : %s MHz",which+10);
								printf("\nHurray3\n");
					break;
				}
			}
		*/
			node1=node1->next;
		}
				
		
	//	printf("\nWRITEBACK TIME : %s : %s\n",writeback.activity,writeback.remark);
	//	printf("\nACCESS TIME : %s : %s\n",noatime.activity,noatime.remark);

	//	printf("\nAC97 : ");
	//	if(ac97.detected)
	//		printf("DETECTED : %s : %s\n",ac97.activity,ac97.remark);
	//	else
	//		printf("NOT DETECTED");
//
//		printf("\nHIGH DEFINITION AUDIO : ");
//		if(hda.detected)
//			printf("DETECTED : %s : %s\n",hda.activity,hda.remark);
//		else
//			printf("NOT DETECTED");
//
		

		sleep(1);
	}
	fclose(file);
}

int main()
{
	pthread_t displayThread;
	pthread_t blueThread, blueTimeThread;
	pthread_t wifiThread, wifiTimeThread;
	pthread_t others;

	pthread_create(&displayThread, NULL, displayInfo, "displayThread");
	pthread_create(&blueThread, NULL, bluetooth_care_taker,"blueThread");
	pthread_create(&blueTimeThread, NULL, bluetoothTimer, "btimeThread");
	pthread_create(&wifiThread, NULL, wifi_care_taker, "wifiThread");
	pthread_create(&wifiTimeThread, NULL, wifiTimer, "wtimeThread");
	pthread_create(&others, NULL, other_care_taker, "otherdevices");

	pthread_join(wifiThread, NULL);
	pthread_join(wifiTimeThread,NULL);
	pthread_join(others, NULL);

	pthread_join(displayThread, NULL);

	return 0;
}
