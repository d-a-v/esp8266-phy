
#include "bufprint.h"
#undef printf
#undef os_printf

#include <stdarg.h>
#include <osapi.h>
#include <stdint.h>
#include <mem.h>
#include <ets_sys.h>


#define ROTBUFLEN_BIT	10 // 11=2048
#define ROTBUFLEN_TEMP	512
#define ROTBUFLEN	(1 << (ROTBUFLEN_BIT))
#define ROTBUFLEN_MASK	(ROTBUFLEN - 1)

char bufprint_allow = 0;
static int rotin = 0;
static int rotout = 0;
static int rotsmall = 0;
static char* rotbuf = NULL;
static int bufprintputc (int c)
{
	rotbuf[rotout] = c;
	rotout = (rotout + 1) & ROTBUFLEN_MASK;
	if (rotout == rotin)
	{
		rotin = (rotin + 1) & ROTBUFLEN_MASK;
		rotsmall++;
	}
	return c;
}
int bufprint (const char* format, ...)
{
	int ret;
	int (*myputc)(int) = bufprintputc;
	
	if (bufprint_allow)
	{
		if (rotbuf)
		{
			ets_printf("<buffered:");
			if (rotsmall)
				ets_printf("(snipped-%d):", rotsmall);
			for (; rotin != rotout; ret++, rotin = (rotin + 1) & ROTBUFLEN_MASK)
				ets_putc(rotbuf[rotin]);
			ets_printf(":dereffub>");
			os_free(rotbuf);
			rotbuf = NULL;
		}
		
		myputc = ets_putc;
	}
	else if (!rotbuf && !(rotbuf = (char*)os_malloc(ROTBUFLEN)))
		return 0;

	va_list ap;
	va_start(ap, format);
	ret += ets_vprintf(myputc, format, ap);
	va_end(ap);
	
	return ret;
}

void dump (const char* what, const char* data, uint16_t len)
{
	size_t i, j;
	bufprint("DUMP %s: len=%d\n", what, len);
	for (i = 0; i < len; i += 8)
	{
		for (j = i; j < i + 8 && j < len; j++)
			bufprint("0x%02x ", data[j]);
		for (; j < i + 8; j++)
			bufprint("     ");
		for (j = i; j < i + 8 && j < len; j++)
			bufprint("%c", data[j] >= 32? data[j]: '.');
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
