
#ifndef BUFPRINT_H
#define BUFPRINT_H

#define nl() do { bufprint("\n"); } while (0)

extern char bufprint_allow;
void bufprint (const char* format, ...) __attribute__ ((format (printf, 1, 2)));
void dump (const char* what, const char* data, size_t len);
void display_ip32 (const char* pre, uint32_t ip);


#endif // BUFPRINT_H
