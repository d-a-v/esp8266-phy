
#ifndef GLUE_STUB_H
#define GLUE_STUB_H

#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "esp-missing.h"

#include "uprint.h"
#include "doprint.h"

#define UDEBUG 1	// 0 or 1

#if UDEBUG
#define uprint(x...)	do { doprint(x); } while (0)
#else
#define uprint(x...)	do { (void)0; } while (0)
#endif

#define uerror(x...)	do { doprint(x); } while (0)
#define uassert(ass...)	do { if ((ass) == 0) { doprint("assert fail: " #ass " @%s:%d\n", __FILE__, __LINE__); uhalt(); } } while (0)
//#define uhalt() 	do { esp_yield(); } while (1)
#define uhalt()		do { (void)0; } while (0)
#define nl()		do { uprint("\n"); } while (0)

#undef os_printf
#define os_printf uprint

typedef enum
{
	GLUE_ERR_OK         = 0,
	GLUE_ERR_MEM        = -1,
	GLUE_ERR_BUF        = -2,
	GLUE_ERR_TIMEOUT    = -3,
	GLUE_ERR_RTE        = -4,
	GLUE_ERR_INPROGRESS = -5,
	GLUE_ERR_VAL        = -6,
	GLUE_ERR_WOULDBLOCK = -7,
	GLUE_ERR_USE        = -8,
	GLUE_ERR_ALREADY    = -9,
	GLUE_ERR_ISCONN     = -10,
	GLUE_ERR_CONN       = -11,
	GLUE_ERR_IF         = -12,
	GLUE_ERR_ABRT       = -13,
	GLUE_ERR_RST        = -14,
	GLUE_ERR_CLSD       = -15,
	GLUE_ERR_ARG        = -16
} err_glue_t;

typedef enum
{
	GLUE_NETIF_FLAG_BROADCAST	= 1,
	GLUE_NETIF_FLAG_UP		= 2,
	GLUE_NETIF_FLAG_ETHARP		= 4,
	GLUE_NETIF_FLAG_IGMP		= 8,
	GLUE_NETIF_FLAG_LINK_UP		= 16,
} glue_netif_flags_t;

void		esp2glue_lwip_init		(void);
void		esp2glue_dhcps_start		(struct ip_info* info);
err_glue_t	esp2glue_dhcp_start		(void);
void		esp2glue_netif_updated		(int netif_idx, uint32_t ip, uint32_t mask, uint32_t gw, glue_netif_flags_t flags, size_t hwlen, const uint8_t* hw /*, void* state*/);
err_glue_t	esp2glue_ethernet_input		(int netif_idx, void* glue_pbuf);
void		esp2glue_alloc_for_recv		(size_t len, void** glue_pbuf, void** glue_data);
void		esp2glue_pbuf_freed		(void* ref_saved);
void		esp2glue_netif_set_default	(int netif_idx);

void		glue2esp_ifup			(int netif_idx, uint32_t ip, uint32_t mask, uint32_t gw);
err_glue_t	glue2esp_linkoutput		(int netif_idx, void* ref2save, void* data, size_t size);

#endif // GLUE_STUB_H
