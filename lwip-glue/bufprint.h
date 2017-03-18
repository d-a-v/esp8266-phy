
#ifndef BUFPRINT_H
#define BUFPRINT_H

#include <stdint.h>

#define nl() do { bufprint("\n"); } while (0)

extern char bufprint_allow;
int bufprint (const char* format, ...) __attribute__ ((format (printf, 1, 2)));
void dump (const char* what, const char* data, uint16_t len);
void display_ip32 (const char* pre, uint32_t ip);

//#define printf bufprint
#undef os_printf
#define os_printf bufprint

#endif // BUFPRINT_H
