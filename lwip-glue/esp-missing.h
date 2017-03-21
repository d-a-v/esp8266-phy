
#ifndef ESP_MISSING_H
#define ESP_MISSING_H

// these are guessed

uint32_t r_rand (void);
void ets_bzero (void*, size_t);
int ets_memcmp (const void*, const void*, size_t n);
void* pvPortZalloc (size_t, const char*, int);
struct netif* eagle_lwip_getif (int);

struct ip_addr;
void wifi_softap_set_station_info (uint8* mac, struct ip_addr*);

#endif
