
#include <stdarg.h>
#include <osapi.h>
#include <stdint.h>

#include "bufprint.h"

static int in = 0;
static int out = 0;
static char rotbuf[2048]; // !!! power of 2 only

char bufprint_allow = 0;

static void printbuf (void)
{
	while (in != out)
		os_printf("%c", rotbuf[in = (in + 1) & ((sizeof rotbuf) - 1)]);
}

void bufprint (const char* format, ...)
{
	char buf[512];
	buf[0] = 0;
	va_list ap;
	va_start(ap, format);
	vsprintf(buf, format, ap);
	va_end(ap);
	
	const char* pbuf = buf;
	pbuf--;
	while ((rotbuf[out = (out + 1) & ((sizeof rotbuf) - 1)] = *++pbuf)) ;
	out--; // remove last 0
	
	if (bufprint_allow)
		printbuf();
}

void dump (const char* what, const char* data, size_t len)
{
	if (!bufprint_allow)
	{
		bufprint("WARN: won't dump data %s\n", what);
		return;
	}
	printbuf();

	size_t i, j;
	os_printf("DUMP %s: len=%d\n", what, len);
	for (i = 0; i < len; i += 8)
	{
		for (j = i; j < i + 8 && j < len; j++)
			os_printf("0x%02x ", data[j]);
		for (; j < i + 8; j++)
			os_printf("     ");
		for (j = i; j < i + 8 && j < len; j++)
			os_printf("%c", data[j] >= 32? data[j]: '.');
		os_printf("\n");
	}
}

void display_ip32 (const char* pre, uint32_t ip)
{
	bufprint("%s%d.%d.%d.%d",
		pre,
		(int)(ip >> 24),
		(int)((ip >> 16) & 0xff),
		(int)((ip >> 8) & 0xff),
		(int)(ip & 0xff));
}
