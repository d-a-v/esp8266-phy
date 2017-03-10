
#include "arch/cc.h"
#include "lwip/timers.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "lwip/pbuf.h"
#include "netif/etharp.h"
#include "lwip/mem.h"

#define STUB(x) do { os_printf("STUB: " #x "\n"); } while (0)

const struct eth_addr ethbroadcast = {{0xff,0xff,0xff,0xff,0xff,0xff}};
struct netif *netif_default;

///////////////////////////////////////
// from pbuf.c
#define SIZEOF_STRUCT_PBUF	LWIP_MEM_ALIGN_SIZE(sizeof(struct pbuf))
// from pbuf.h
#ifndef PBUF_RSV_FOR_WLAN
#error PBUF_RSV_FOR_WLAN should be defined
#endif
#ifndef EBUF_LWIP
#error EBUF_LWIP should be defined
#endif
#define EP_OFFSET 36

///////////////////////////////////////
// helpers

void stub_display_ip (const char* pre, ip_addr_t ip)
{
	os_printf("%s%d.%d.%d.%d",
		pre,
		ip.addr >> 24,
		(ip.addr >> 16) & 0xff,
		(ip.addr >> 8) & 0xff,
		ip.addr & 0xff);
}

void stub_display_ip_info (struct ip_info* i)
{
	stub_display_ip("ip=", i->ip);
	stub_display_ip(" mask=", i->gw);
	stub_display_ip(" gw=", i->gw);
}

void stub_display_netif_flags (int flags)
{
	#define IFF(x)	os_printf("|%s" #x, (flags & NETIF_FLAG_##x)? "!": "")
	IFF(UP);
	IFF(BROADCAST);
	IFF(POINTTOPOINT);
	IFF(DHCP);
	IFF(LINK_UP);
	IFF(ETHARP);
	IFF(ETHERNET);
	IFF(IGMP);
	#undef IFF
}

void stub_display_netif (const char* pre, struct netif* netif)
{
	os_printf("%s@%p name=%c%c idx=%d mtu=%d flags=",
		pre,
		netif,
		netif->name[0], netif->name[1],
		netif->num,
		netif->mtu);
	stub_display_netif_flags(netif->flags);
}

///////////////////////////////////////



void dhcps_start (struct ip_info *info)
{
	//STUB(dhcps_start);
	os_printf("STUB: dhcps_start ");
	stub_display_ip_info(info);
	os_printf("\n");
}

void dhcps_stop (void)
{
	STUB(dhcps_stop);
}

void espconn_init (void)
{
	STUB(espconn_init);
}

void dhcp_cleanup(struct netif *netif)
{
	STUB(dhcp_cleanup);
}

err_t dhcp_release(struct netif *netif)
{
	STUB(dhcp_release);
	return ERR_ABRT;
}

/**
 * Start DHCP negotiation for a network interface.
 *
 * If no DHCP client instance was attached to this interface,
 * a new client is created first. If a DHCP client instance
 * was already present, it restarts negotiation.
 *
 * @param netif The lwIP network interface
 * @return lwIP error code
 * - ERR_OK - No error
 * - ERR_MEM - Out of memory
 */
err_t dhcp_start (struct netif* netif)
{
	//STUB(dhcp_start);
	os_printf("STUB: dhcp_start (");
	stub_display_ip("netif: ip=", netif->ip_addr);
	stub_display_ip(" mask=", netif->netmask);
	stub_display_ip(" gw=", netif->gw);
	stub_display_netif(" ", netif);
	os_printf(")\n");
	// there are really no interesting data to translate
	// STUB: dhcp_start (netif: ip=4.18.15.0 mask=0.60.0.60 gw=0.0.0.1 @0x3ffeffac name=ew idx=0 mtu=0 flags=|UP|BROADCAST|POINTTOPOINT|DHCP|LINK_UP|ETHARP|ETHERNET|IGMP)
	
	return ERR_ABRT;
}

void dhcp_stop(struct netif *netif)
{
	STUB(dhcp_stop);
}

/**
 * Perform Sanity check of user-configurable values, and initialize all modules.
 */
void lwip_init(void)
{
	STUB(lwip_init);
}

/**
 * Add a network interface to the list of lwIP netifs.
 *
 * @param netif a pre-allocated netif structure
 * @param ipaddr IP address for the new netif
 * @param netmask network mask for the new netif
 * @param gw default gateway IP address for the new netif
 * @param state opaque data passed to the new netif
 * @param init callback function that initializes the interface
 * @param input callback function that is called to pass
 * ingress packets up in the protocol layer stack.
 *
 * @return netif, or NULL if failed.
 */
struct netif* netif_add(struct netif *netif, ip_addr_t *ipaddr, ip_addr_t *netmask, ip_addr_t *gw, void *state, netif_init_fn init, netif_input_fn input)
{
	STUB(netif_add);
	return NULL;
}

/**
 * Remove a network interface from the list of lwIP netifs.
 *
 * @param netif the network interface to remove
 */
void netif_remove(struct netif *netif)
{
	STUB(netif_remove);
}

/**
 * Change IP address configuration for a network interface (including netmask
 * and default gateway).
 *
 * @param netif the network interface to change
 * @param ipaddr the new IP address
 * @param netmask the new netmask
 * @param gw the new default gateway
 */
void netif_set_addr(struct netif *netif, ip_addr_t *ipaddr, ip_addr_t *netmask, ip_addr_t *gw)
{
	STUB(netif_set_addr);
}

/**
 * Set a network interface as the default network interface
 * (used to output all packets for which no specific route is found)
 *
 * @param netif the default network interface
 */
void netif_set_default(struct netif *netif)
{
	STUB(netif_set_default);
}

/**
 * Bring an interface down, disabling any traffic processing.
 *
 * @note: Enabling DHCP on a down interface will make it come
 * up once configured.
 * 
 * @see dhcp_start()
 */ 
void netif_set_down(struct netif *netif)
{
	STUB(netif_set_down);
}

/**
 * Bring an interface up, available for processing
 * traffic.
 * 
 * @note: Enabling DHCP on a down interface will make it come
 * up once configured.
 * 
 * @see dhcp_start()
 */ 
void netif_set_up(struct netif *netif)
{
	STUB(netif_set_up);
}


/**
 * Allocates a pbuf of the given type (possibly a chain for PBUF_POOL type).
 *
 * The actual memory allocated for the pbuf is determined by the
 * layer at which the pbuf is allocated and the requested size
 * (from the size parameter).
 *
 * @param layer flag to define header size
 * @param length size of the pbuf's payload
 * @param type this parameter decides how and where the pbuf
 * should be allocated as follows:
 *
 * - PBUF_RAM: buffer memory for pbuf is allocated as one large
 *             chunk. This includes protocol headers as well.
 * - PBUF_ROM: no buffer memory is allocated for the pbuf, even for
 *             protocol headers. Additional headers must be prepended
 *             by allocating another pbuf and chain in to the front of
 *             the ROM pbuf. It is assumed that the memory used is really
 *             similar to ROM in that it is immutable and will not be
 *             changed. Memory which is dynamic should generally not
 *             be attached to PBUF_ROM pbufs. Use PBUF_REF instead.
 * - PBUF_REF: no buffer memory is allocated for the pbuf, even for
 *             protocol headers. It is assumed that the pbuf is only
 *             being used in a single thread. If the pbuf gets queued,
 *             then pbuf_take should be called to copy the buffer.
 * - PBUF_POOL: the pbuf is allocated as a pbuf chain, with pbufs from
 *              the pbuf pool that is allocated during pbuf_init().
 *
 * @return the allocated pbuf. If multiple pbufs where allocated, this
 * is the first pbuf of a pbuf chain.
 */
struct pbuf* pbuf_alloc(pbuf_layer layer, u16_t length, pbuf_type type)
{
	// pbuf creation from blobs
	// copy parts of original code matching specific requests

	//STUB(pbuf_alloc);
	os_printf("STUB: pbuf_alloc layer=%s(%d) len=%d type=%s(%d)"
		"\n",
		layer==PBUF_TRANSPORT? "transport":
		layer==PBUF_IP? "ip":
		layer==PBUF_LINK? "link":
		layer==PBUF_RAW? "raw":
		"???", (int)layer,
		length,
		type==PBUF_RAM? "ram":
		type==PBUF_ROM? "rom":
		type==PBUF_REF? "ref":
		type==PBUF_POOL? "pool":
		type==PBUF_ESF_RX? "esp-wlan":
		"???", (int)type);
	
	u16_t offset = 0;
	if (layer == PBUF_RAW && type == PBUF_RAM)
	{
		offset += EP_OFFSET;
		
		/* If pbuf is to be allocated in RAM, allocate memory for it. */
		uint16_t alloclen = LWIP_MEM_ALIGN_SIZE(SIZEOF_STRUCT_PBUF + offset) + LWIP_MEM_ALIGN_SIZE(length);
		struct pbuf* p = (struct pbuf*)mem_malloc(alloclen);
		if (p == NULL)
			return NULL;
		/* Set up internal structure of the pbuf. */
		p->payload = LWIP_MEM_ALIGN((void *)((u8_t *)p + SIZEOF_STRUCT_PBUF + offset));
		p->len = p->tot_len = length;
		p->next = NULL;
		p->type = type;
		p->eb = NULL;
		/* set reference count */
		p->ref = 1;
		/* set flags */
		p->flags = 0;
		
		os_printf("STUB: ret 0x%p (%d bytes)\n", p, alloclen);
		
		return p;
	}

	os_printf("STUB: pbuf_alloc BAD CASE\n");
		
	return NULL;
}

/**
 * Dereference a pbuf chain or queue and deallocate any no-longer-used
 * pbufs at the head of this chain or queue.
 *
 * Decrements the pbuf reference count. If it reaches zero, the pbuf is
 * deallocated.
 *
 * For a pbuf chain, this is repeated for each pbuf in the chain,
 * up to the first pbuf which has a non-zero reference count after
 * decrementing. So, when all reference counts are one, the whole
 * chain is free'd.
 *
 * @param p The pbuf (chain) to be dereferenced.
 *
 * @return the number of pbufs that were de-allocated
 * from the head of the chain.
 *
 * @note MUST NOT be called on a packet queue (Not verified to work yet).
 * @note the reference counter of a pbuf equals the number of pointers
 * that refer to the pbuf (or into the pbuf).
 *
 * @internal examples:
 *
 * Assuming existing chains a->b->c with the following reference
 * counts, calling pbuf_free(a) results in:
 * 
 * 1->2->3 becomes ...1->3
 * 3->3->3 becomes 2->3->3
 * 1->1->2 becomes ......1
 * 2->1->1 becomes 1->1->1
 * 1->1->1 becomes .......
 *
 */
u8_t pbuf_free(struct pbuf *p)
{
	//STUB(pbuf_free);
	os_printf("STUB: pbuf_free(%p) ref=%d\n", p, p->ref);
	
	#if LWIP_SUPPORT_CUSTOM_PBUF
	#error LWIP_SUPPORT_CUSTOM_PBUF is defined
	#endif
	
	if (!p->next && p->ref == 1)
	{
		mem_free(p);
		return 1;
	}

	os_printf("STUB: pbuf_free BAD CASE\n");
	return 0;
}

/**
 * Increment the reference count of the pbuf.
 *
 * @param p pbuf to increase reference counter of
 *
 */
void pbuf_ref(struct pbuf *p)
{
	STUB(pbuf_ref);
}


/** Handle timeouts for NO_SYS==1 (i.e. without using
 * tcpip_thread/sys_timeouts_mbox_fetch(). Uses sys_now() to call timeout
 * handler functions when timeouts expire.
 *
 * Must be called periodically from your main loop.
 */
void sys_check_timeouts(void)
{
#if 1
	static uint8_t r = 0;
	if ((++r & 63) == 0)
	{
		r = 0;
		os_printf(".");
	}
#else
	static const char* p = "-\\|/";
	static uint8_t q = 0;
	static uint8_t r = 0;
	if ((++r & 15) == 0)
	{
		r = 0;
		q = (q + 1) & 3;
		os_printf("%c\b", p[q]);
	}
#endif
}

/**
 * Create a one-shot timer (aka timeout). Timeouts are processed in the
 * following cases:
 * - while waiting for a message using sys_timeouts_mbox_fetch()
 * - by calling sys_check_timeouts() (NO_SYS==1 only)
 *
 * @param msecs time in milliseconds after that the timer should expire
 * @param handler callback function to call when msecs have elapsed
 * @param arg argument to pass to the callback function
 */
void sys_timeout(u32_t msecs, sys_timeout_handler handler, void *arg)
{
	STUB(sys_timeout);
}

/**
 * Go through timeout list (for this task only) and remove the first matching
 * entry, even though the timeout has not triggered yet.
 *
 * @note This function only works as expected if there is only one timeout
 * calling 'handler' in the list of timeouts.
 *
 * @param handler callback function that would be called by the timeout
 * @param arg callback argument that would be passed to handler
*/
void sys_untimeout(sys_timeout_handler handler, void *arg)
{
	STUB(sys_untimeout);
}

/**
 * Resolve and fill-in Ethernet address header for outgoing IP packet.
 *
 * For IP multicast and broadcast, corresponding Ethernet addresses
 * are selected and the packet is transmitted on the link.
 *
 * For unicast addresses, the packet is submitted to etharp_query(). In
 * case the IP address is outside the local network, the IP address of 
 * the gateway is used.
 *
 * @param netif The lwIP network interface which the IP packet will be sent on.
 * @param q The pbuf(s) containing the IP packet to be sent.
 * @param ipaddr The IP address of the packet destination.  
 *
 * @return
 * - ERR_RTE No route to destination (no gateway to external networks),
 * or the return type of either etharp_query() or etharp_send_ip().
 */
err_t etharp_output(struct netif *netif, struct pbuf *q, ip_addr_t *ipaddr)
{
	STUB(etharp_output);
	return ERR_ABRT;
}
                   
 /**
 * Process received ethernet frames. Using this function instead of directly
 * calling ip_input and passing ARP frames through etharp in ethernetif_input,
 * the ARP cache is protected from concurrent access.
 *
 * @param p the recevied packet, p->payload pointing to the ethernet header
 * @param netif the network interface on which the packet was received
 */
err_t ethernet_input(struct pbuf *p, struct netif *netif)
{
	STUB(ethernet_input);
	return ERR_ABRT;
}
