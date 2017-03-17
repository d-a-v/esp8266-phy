#ifndef LWIP_STUBBED_SYMBOLS_H
#define LWIP_STUBBED_SYMBOLS_H

#include "ets_sys.h"
#include "osapi.h"

///////////////////////////////
//// DEBUG
#if 0 // debug 1:on or 0

//#define LWIP_DBG_TYPES_ON		(LWIP_DBG_ON|LWIP_DBG_TRACE|LWIP_DBG_STATE|LWIP_DBG_FRESH|LWIP_DBG_HALT)
//#define LWIP_DBG_TYPES_ON		(LWIP_DBG_ON|LWIP_DBG_TRACE|LWIP_DBG_STATE|LWIP_DBG_FRESH)
#define LWIP_DBG_TYPES_ON		(LWIP_DBG_ON)

#define LWIP_DEBUG 1

#include "bufprint.h" // for debugging messages *before* Serial.setDebugOutput(true) called

#endif // debug
///////////////////////////////
//// MISSING 

#define sys_now millis		// arduino wire millis() definition returns 32 bits like sys_now() does
#define LWIP_RAND r_rand	// old lwip uses this useful undocumented function
uint32_t r_rand (void);		// esp blobs

// ip_addr / ip_info: do not exist in lwip2, lwip1.4 only, used in blobs:
// esp8266/tools//sdk/include/user_interface.h:bool wifi_get_ip_info(uint8 if_index, struct ip_info *info);
// copied from lwip-esp/1.4
struct ip_addr {
  uint32_t addr;
};
struct ip_info {
    struct ip_addr ip;
    struct ip_addr netmask;
    struct ip_addr gw;
};
///////////////////////////////
//// STUBS

//< lwip-linked-symbols-blob-list.txt sed 's,\(.*\),#define \1 \1_STUBBED,g'

// these symbols must be renamed in the new implementations
// because they are known/used in blobs

#define dhcp_cleanup dhcp_cleanup_STUBBED
#define dhcp_release dhcp_release_STUBBED
#define dhcp_start dhcp_start_STUBBED
#define dhcp_stop dhcp_stop_STUBBED
#define dhcps_start dhcps_start_STUBBED
#define dhcps_stop dhcps_stop_STUBBED
#define espconn_init espconn_init_STUBBED
#define etharp_output etharp_output_STUBBED
#define ethbroadcast ethbroadcast_STUBBED
#define ethernet_input ethernet_input_STUBBED
//#define lwip_init lwip_init_STUBBED				// void(void)
#define netif_add netif_add_STUBBED
#define netif_default netif_default_STUBBED
#define netif_remove netif_remove_STUBBED
#define netif_set_addr netif_set_addr_STUBBED
#define netif_set_default netif_set_default_STUBBED
#define netif_set_down netif_set_down_STUBBED
#define netif_set_up netif_set_up_STUBBED
#define pbuf_alloc pbuf_alloc_STUBBED
#define pbuf_free pbuf_free_STUBBED
#define pbuf_ref pbuf_ref_STUBBED
//#define sys_check_timeouts sys_check_timeouts_STUBBED		// void(void)

#ifndef LWIP_DEBUG
#define sys_timeout sys_timeout_STUBBED
#endif

#define sys_untimeout sys_untimeout_STUBBED

///////////////////////////////
//// REWORK
#ifndef LWIP_REWORK

//#define dns_gethostbyname_addrtype	reworked_dns_gethostbyname_addrtype
//#define dns_gethostbyname		reworked_dns_gethostbyname
//#define ip_addr_t ip4_addr_t

#endif // LWIP_REWORK
///////////////////////////////
#endif // LWIP_STUBBED_SYMBOLS_H
