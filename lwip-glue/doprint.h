
#ifndef DOPRINT_H
#define DOPRINT_H

// os_printf/ets_printf works only with Serial.setDebugOutput(true)
// ets_putc always work (after Serial.begin())
// doprint uses ets_putc after doprint_allow gets true and bufferizes before that

extern int doprint_allow;
int doprint (const char* format, ...) __attribute__ ((format (printf, 1, 2)));


#endif // DOPRINT_H
