
#include "uprint.h"
#include "glue.h"

#if UDEBUG

void dump (const char* what, const char* data, uint16_t len)
{
	#define N 16
	size_t i, j;
	uprint("DUMP %s: len=%d\n", what, len);
	for (i = 0; i < len; i += N)
	{
		for (j = i; j < i + N && j < len; j++)
			uprint("%02x ", data[j]);
		for (; j < i + N; j++)
			uprint("   ");
		for (j = i; j < i + N && j < len; j++)
			uprint("%c", (data[j] >= 32 && data[j] < 127)? data[j]: '.');
		uprint("\n");
	}
}

void display_ip32 (const char* pre, uint32_t ip)
{
	uprint("%s%d.%d.%d.%d",
		pre,
		(int)(ip & 0xff),
		(int)((ip >> 8) & 0xff),
		(int)((ip >> 16) & 0xff),
		(int)(ip >> 24));
}

void display_mac (const uint8_t* mac)
{
	uprint("%02x:%02x:%02x:%02x:%02x:%02x",
		mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void display_ip_info (const struct ip_info* i)
{
	display_ip32("ip=", i->ip.addr);
	display_ip32(" mask=", i->netmask.addr);
	display_ip32(" gw=", i->gw.addr);
}

#endif
