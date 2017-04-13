
/*

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met: 

1. Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer. 
2. Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation 
and/or other materials provided with the distribution. 
3. The name of the author may not be used to endorse or promote products 
derived from this software without specific prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS AND ANY EXPRESS OR IMPLIED 
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.

author: d. gauchard

*/

// version for esp8266 sdk-2.0.0(656edbf)

#ifndef LWIP2_ARCH_CC_H
#define LWIP2_ARCH_CC_H

#include "stdint.h"

///////////////////////////////
//// DEBUG
#if 0 // debug 1:on or 0

//#define LWIP_DBG_TYPES_ON		(LWIP_DBG_ON|LWIP_DBG_TRACE|LWIP_DBG_STATE|LWIP_DBG_FRESH|LWIP_DBG_HALT)
#define LWIP_DBG_TYPES_ON		(LWIP_DBG_ON|LWIP_DBG_TRACE|LWIP_DBG_STATE|LWIP_DBG_FRESH)
//#define LWIP_DBG_TYPES_ON		(LWIP_DBG_ON)

#define LWIP_DEBUG 1

//int doprint (const char* format, ...) __attribute__ ((format (printf, 1, 2)));
extern int os_printf_plus(const char * format, ...) __attribute__ ((format (printf, 1, 2)));
#define LWIP_PLATFORM_DIAG(x) do { os_printf x;} while(0)

#endif // debug

///////////////////////////////
//// MISSING 

#define sys_now millis		// arduino wire millis() definition returns 32 bits like sys_now() does
#define LWIP_RAND r_rand	// old lwip uses this useful undocumented function

///////////////////////////////
//// 

#ifdef LWIP_BUILD
// define LWIP_BUILD only when building LWIP
// otherwise include files below would conflict
// with standard headers like atoi()
#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "esp-missing.h"
#endif
#include "mem.h" // useful for os_malloc used in esp-arduino's mDNS

typedef int sys_prot_t;	// not really used
#define SYS_ARCH_DECL_PROTECT(lev)
#define SYS_ARCH_PROTECT(lev) os_intr_lock()
#define SYS_ARCH_UNPROTECT(lev) os_intr_unlock()

///////////////////////////////
//// PROVIDED TO USER

typedef struct ip4_addr ip4_addr_t;
extern int ntp_servers_number;
extern ip4_addr_t* ntp_servers;

///////////////////////////////
//// STUBS

//< lwip-linked-symbols-blob-list.txt sed 's,\(.*\),#define \1 \1_RENAMED,g'

// these symbols must be renamed in the new implementation
// because they are known/used in blobs

#define dhcp_cleanup dhcp_cleanup_RENAMED
#define dhcp_release dhcp_release_RENAMED
#define dhcp_start dhcp_start_RENAMED
#define dhcp_stop dhcp_stop_RENAMED
#define dhcps_start dhcps_start_RENAMED
#define dhcps_stop dhcps_stop_RENAMED
#define espconn_init espconn_init_RENAMED
#define etharp_output etharp_output_RENAMED
#define ethbroadcast ethbroadcast_RENAMED
#define ethernet_input ethernet_input_RENAMED
#define lwip_init lwip_init_RENAMED
#define netif_add netif_add_RENAMED
#define netif_default netif_default_RENAMED
#define netif_remove netif_remove_RENAMED
#define netif_set_addr netif_set_addr_RENAMED
#define netif_set_default netif_set_default_RENAMED
#define netif_set_down netif_set_down_RENAMED
#define netif_set_up netif_set_up_RENAMED
#define pbuf_alloc pbuf_alloc_RENAMED
#define pbuf_free pbuf_free_RENAMED
#define pbuf_ref pbuf_ref_RENAMED
//#define sys_check_timeouts sys_check_timeouts_RENAMED		// void(void)

#if !defined(LWIP_DEBUG) || !SYS_DEBUG
#define sys_timeout sys_timeout_RENAMED
#endif

#define sys_untimeout sys_untimeout_RENAMED

///////////////////////////////
#endif // LWIP2_ARCH_CC_H
