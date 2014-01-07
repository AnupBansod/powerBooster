#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <dirent.h>
#include <linux/types.h>
#include <net/if.h>
#include <linux/sockios.h>
#include <sys/ioctl.h>

#define u64 __u64
#define u32 __u32
#define u16 __u16
#define u8 __u8
#include <linux/ethtool.h>
#undef u64
#undef u32
#undef u16
#undef u8


#include "powerbooster.h"

void activate_WOL_suggestion(void)
{
	int sock;
	struct ifreq ifr;
	struct ethtool_wolinfo wol;
	int ret;

	memset(&ifr, 0, sizeof(struct ifreq));

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock<0)
		return;

	strcpy(ifr.ifr_name, "eth0");

	/* Check if the interface is up */
	ret = ioctl(sock, SIOCGIFFLAGS, &ifr);
	if (ret<0) {
		close(sock);
		return;
	}

	if (ifr.ifr_flags & (IFF_UP | IFF_RUNNING)) {
		close(sock);
		return;
	}

	memset(&wol, 0, sizeof(wol));

	wol.cmd = ETHTOOL_GWOL;
	ifr.ifr_data = (caddr_t)&wol;
        ioctl(sock, SIOCETHTOOL, &ifr);
	wol.cmd = ETHTOOL_SWOL;
	wol.wolopts = 0;
        ioctl(sock, SIOCETHTOOL, &ifr);

	close(sock);
}



void suggest_WOL_off(void)
{
	int sock;
	struct ifreq ifr;
	struct ethtool_wolinfo wol;
	int ret;

	memset(&ifr, 0, sizeof(struct ifreq));

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock<0)
		return;

	strcpy(ifr.ifr_name, "eth0");

	/* Check if the interface is up */
	ret = ioctl(sock, SIOCGIFFLAGS, &ifr);
	if (ret<0) {
		close(sock);
		return;
	}

	if (ifr.ifr_flags & (IFF_UP | IFF_RUNNING)) {
		close(sock);
		return;
	}

	memset(&wol, 0, sizeof(wol));

	wol.cmd = ETHTOOL_GWOL;
	ifr.ifr_data = (caddr_t)&wol;
        ioctl(sock, SIOCETHTOOL, &ifr);

	if (wol.wolopts) {
		activate_WOL_suggestion();
	}

	close(sock);
}
