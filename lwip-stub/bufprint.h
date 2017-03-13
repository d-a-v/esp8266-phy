
#ifndef BUFPRINT_H
#define BUFPRINT_H

extern char bufprint_allow;
void bufprint (const char* format, ...) __attribute__ ((format (printf, 1, 2)));
#define nl() do { bufprint("\n"); } while (0)

#endif // BUFPRINT_H
