
#include "doprint.h"

#include <stdarg.h>
#include <osapi.h>
#include <stdint.h>
#include <mem.h>
#include <ets_sys.h>

char doprint_allow = 0;

#if 0

// do not bufferize

int doprint (const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	int ret = ets_vprintf(ets_putc, format, ap);
	va_end(ap);
	return ret;
}

#else

// bufferize

#define ROTBUFLEN_BIT	11 // 11=2048
#define ROTBUFLEN	(1 << (ROTBUFLEN_BIT))
#define ROTBUFLEN_MASK	((ROTBUFLEN) - 1)

static int rotin = 0;
static int rotout = 0;
static int rotsmall = 0;
static char* rotbuf = NULL;
static int bufputc (int c)
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

static int doprint_direct (const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	int ret = ets_vprintf(ets_putc, format, ap);
	va_end(ap);
	return ret;
}

int doprint (const char* format, ...)
{
	int ret;
	int (*myputc)(int);
	
	if (doprint_allow)
	{
		if (rotbuf)
		{
			doprint_direct("<buffered:");
			if (rotsmall)
				doprint_direct("(%dcharslost):", rotsmall);
			ets_putc('\n');
			for (; rotin != rotout; ret++, rotin = (rotin + 1) & ROTBUFLEN_MASK)
				ets_putc(rotbuf[rotin]);
			doprint_direct(":dereffub>\n");
			os_free(rotbuf);
			rotbuf = NULL;
		}
		
		myputc = ets_putc;
	}
	else if (!rotbuf && !(rotbuf = (char*)os_malloc(ROTBUFLEN)))
		return 0;
	else
		myputc = bufputc;

	va_list ap;
	va_start(ap, format);
	ret += ets_vprintf(myputc, format, ap);
	va_end(ap);
	
	return ret;
}

#endif
