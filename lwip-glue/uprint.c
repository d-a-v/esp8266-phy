
#include "uprint.h"
#include "glue.h"

void dump (const char* what, const char* data, uint16_t len)
{
	size_t i, j;
	uprint("DUMP %s: len=%d\n", what, len);
	for (i = 0; i < len; i += 8)
	{
		for (j = i; j < i + 8 && j < len; j++)
			uprint("0x%02x ", data[j]);
		for (; j < i + 8; j++)
			uprint("     ");
		for (j = i; j < i + 8 && j < len; j++)
			uprint("%c", (data[j] >= 32 && data[j] < 127)? data[j]: '.');
		uprint("\n");
	}
}

void display_ip32 (const char* pre, uint32_t ip)
{
	uprint("%s%d.%d.%d.%d",
		pre,
		(int)(ip >> 24),
		(int)((ip >> 16) & 0xff),
		(int)((ip >> 8) & 0xff),
		(int)(ip & 0xff));
}
