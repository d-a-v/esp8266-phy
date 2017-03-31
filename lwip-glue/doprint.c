
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

static int nl = 0;

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

//#define PUTC ets_putc		// no line number
#define PUTC nl_putc		// show line number

static int nl_putc (int c);

static int doprint_direct (const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	int ret = ets_vprintf(PUTC, format, ap);
	va_end(ap);
	return ret;
}

static int nl_putc (int c)
{
	ets_putc(c);
	if (c == '\n')
		doprint_direct("%d:", nl++);
	return c;
}

extern uint32_t millis (void);

int doprint (const char* format, ...)
{
	int ret;
	int (*myputc)(int);

static uint32_t glok = 0;
if (glok == 0) glok = millis();
doprint_allow = (millis() - glok) > 2000;

	
	if (doprint_allow)
	{
		if (rotbuf)
		{
			doprint_direct("\n<buffered:");
			if (rotsmall)
				doprint_direct("(%dcharslost):", rotsmall);
			PUTC('\n');
			for (; rotin != rotout; ret++, rotin = (rotin + 1) & ROTBUFLEN_MASK)
				PUTC(rotbuf[rotin]);
			doprint_direct(":dereffub>\n");
			os_free(rotbuf);
			rotbuf = NULL;
		}
		
		myputc = PUTC;
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
