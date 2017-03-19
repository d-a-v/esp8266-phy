
#ifndef DOPRINT_H
#define DOPRINT_H

int doprint (const char* format, ...) __attribute__ ((format (printf, 1, 2)));

// ets_printf works only with Serial.setDebugOutput(true)
// ets_putc always work <- doprint uses it

#endif // DOPRINT_H
