
#include <stdarg.h>
#include <osapi.h>

#include "bufprint.h"

static int in = 0;
static int out = 0;
static char rotbuf[1024]; // !!! power of 2 only

char bufprint_allow = 0;

static void printbuf (void)
{
	while (in != out)
		os_printf("%c", rotbuf[in = (in + 1) & ((sizeof rotbuf) - 1)]);
}

void bufprint (const char* format, ...)
{
	char buf[256];
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

