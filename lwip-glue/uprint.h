
#ifndef UPRINT_H
#define UPRINT_H

#include <stdint.h>

#include "lwipopts.h" // struct ip_info

void dump (const char* what, const char* data, uint16_t len);
void display_ip32 (const char* pre, uint32_t ip);
void display_ip_info (const struct ip_info* i);

#endif // BUFPRINT_H
