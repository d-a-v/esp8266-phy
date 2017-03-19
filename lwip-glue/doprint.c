
#include "doprint.h"

#include <stdarg.h>
#include <osapi.h>
#include <stdint.h>
#include <mem.h>
#include <ets_sys.h>

int doprint (const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	int ret = ets_vprintf(ets_putc, format, ap);
	va_end(ap);
	return ret;
}
