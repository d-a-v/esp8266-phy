#ifndef LWIP_STUBBED_SYMBOLS_H
#define LWIP_STUBBED_SYMBOLS_H

#define LWIPDEBUGALL 1

#ifndef LWIPDEBUGALL
#error define LWIPDEBUGALL to 0 or 1
#endif

//#define LWIP_PLATFORM_DIAG(x...) do { bufprint x; } while (0)

//< lwip-linked-symbols-blob-list.txt sed 's,\(.*\),#define \1 \1_STUBBED,g'

// these symbols must be renamed in the new implementations
// if ever they exist

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

#if !LWIPDEBUGALL
#define sys_timeout sys_timeout_STUBBED
#endif

#define sys_untimeout sys_untimeout_STUBBED

#endif // LWIP_STUBBED_SYMBOLS_H

