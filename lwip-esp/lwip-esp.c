
#include "arch/cc.h"
#include "lwip/timers.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "lwip/pbuf.h"
#include "netif/etharp.h"
#include "lwip/mem.h"

#include "glue.h"

#define STUB(x) do { uerror("STUB: " #x "\n"); } while (0)

// guessed interface, esp blobs
void system_pp_recycle_rx_pkt (void*);
void system_station_got_ip_set(ip_addr_t* ip, ip_addr_t* mask, ip_addr_t* gw);

// ethbroadcast linked from blobs
const struct eth_addr ethbroadcast = {{0xff,0xff,0xff,0xff,0xff,0xff}};
// linked from blobs
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
// netif

#define netif_sta netif_esp[STATION_IF]
#define netif_ap  netif_esp[SOFTAP_IF]
static struct netif* netif_esp[2] = { NULL, NULL };

///////////////////////////////////////
// glue converters

err_t glue2esp_err (err_glue_t err)
{
	switch (err)
	{
	case GLUE_ERR_OK         : return ERR_OK;
	case GLUE_ERR_MEM        : return ERR_MEM;
	case GLUE_ERR_BUF        : return ERR_BUF;
	case GLUE_ERR_TIMEOUT    : return ERR_TIMEOUT;
	case GLUE_ERR_RTE        : return ERR_RTE;
	case GLUE_ERR_INPROGRESS : return ERR_INPROGRESS;
	case GLUE_ERR_VAL        : return ERR_VAL;
	case GLUE_ERR_WOULDBLOCK : return ERR_WOULDBLOCK;
	case GLUE_ERR_ABRT       : return ERR_ABRT;
	case GLUE_ERR_RST        : return ERR_RST;
	case GLUE_ERR_CLSD       : return ERR_CLSD;
	case GLUE_ERR_CONN       : return ERR_CONN;
	case GLUE_ERR_ARG        : return ERR_ARG;
	case GLUE_ERR_USE        : return ERR_USE;
	case GLUE_ERR_IF         : return ERR_IF;
	case GLUE_ERR_ISCONN     : return ERR_ISCONN;

	/* old does not have: */
	case GLUE_ERR_ALREADY    : return ERR_ABRT;

	default: return ERR_ABRT;
	}
};

err_glue_t esp2glue_err (err_glue_t err)
{
	switch (err)
	{
	case ERR_OK         : return GLUE_ERR_OK;
	case ERR_MEM        : return GLUE_ERR_MEM;
	case ERR_BUF        : return GLUE_ERR_BUF;
	case ERR_TIMEOUT    : return GLUE_ERR_TIMEOUT;
	case ERR_RTE        : return GLUE_ERR_RTE;
	case ERR_INPROGRESS : return GLUE_ERR_INPROGRESS;
	case ERR_VAL        : return GLUE_ERR_VAL;
	case ERR_WOULDBLOCK : return GLUE_ERR_WOULDBLOCK;
	case ERR_ABRT       : return GLUE_ERR_ABRT;
	case ERR_RST        : return GLUE_ERR_RST;
	case ERR_CLSD       : return GLUE_ERR_CLSD;
	case ERR_CONN       : return GLUE_ERR_CONN;
	case ERR_ARG        : return GLUE_ERR_ARG;
	case ERR_USE        : return GLUE_ERR_USE;
	case ERR_IF         : return GLUE_ERR_IF;
	case ERR_ISCONN     : return GLUE_ERR_ISCONN;

	default: return GLUE_ERR_ABRT;
	}
};

glue_netif_flags_t esp2glue_netif_flags (u8_t flags)
{

//XXXFIXME this is the paranoia mode
// make it simpler in non-debug mode

	u8_t copy = flags;
	glue_netif_flags_t gf = 0;
	#define CF(x)	do { if (flags & NETIF_FLAG_##x) { gf |= GLUE_NETIF_FLAG_##x; flags &= ~NETIF_FLAG_##x; } } while (0)
	CF(UP);
	CF(BROADCAST);
	//CF(POINTTOPOINT);
	//CF(DHCP);
	CF(LINK_UP);
	CF(ETHARP);
	//CF(ETHERNET);
	CF(IGMP);
	#undef CF

	if (flags)
		uerror("ERROR: esp2glue_netif_flags: remaining flags not converted (0x%x->0x%x)\n", copy, flags);
	return gf;
}

///////////////////////////////////////
// display helpers

#define stub_display_ip(pre,ip) display_ip32(pre, (ip).addr)

static void stub_display_netif_flags (int flags)
{
	#define IFF(x)	do { if (flags & NETIF_FLAG_##x) uprint("|" #x); } while (0)
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

static void stub_display_netif (struct netif* netif)
{
	uassert(netif == netif_sta || netif == netif_ap);
	uprint("esp-@%p %s name=%c%c%d mtu=%d state=%p ",
		netif,
		netif == netif_ap? "AP": "STA",
		netif->name[0], netif->name[1], netif->num,
		netif->mtu,
		netif->state);
	if (netif->hwaddr_len == 6)
		display_mac(netif->hwaddr);
	stub_display_netif_flags(netif->flags);
	display_ip32(" ip=", netif->ip_addr.addr);
	display_ip32(" mask=", netif->netmask.addr);
	display_ip32(" gw=", netif->gw.addr);
	uprint("\n");
}

void pbuf_info (const char* what, pbuf_layer layer, u16_t length, pbuf_type type)
{
	uerror("WRAP: %s layer=%s(%d) len=%d type=%s(%d)\n",
		what,
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
}

///////////////////////////////////////
// quick pool to store references to data sent

#define PBUF_CUSTOM_TYPE_STATIC 0x42 // must not conflict with PBUF_* (pbuf types)
#define PBUF_CUSTOM_NUMBER 8
struct pbuf_wrapper
{
	void* ref;	// for glue2git_linkoutput_pbuf_released(void*)
	struct pbuf pbuf;
} pbuf_wrappers[PBUF_CUSTOM_NUMBER];

struct pbuf_wrapper* get_free_pbuf_wrapper (void)
{
	int i = 0;
	while (i < PBUF_CUSTOM_NUMBER && pbuf_wrappers[i].ref)
		i++;
	if (i < PBUF_CUSTOM_NUMBER)
		return &pbuf_wrappers[i];
	uerror("ERROR: not enough pbuf_wrapper\n");
	return NULL;
}

// output real packet here:
err_glue_t glue2esp_linkoutput (int netif_idx, void* ref2save, char* rawdata, size_t size)
{
	// get a free pbuf wrapper
	struct pbuf_wrapper* p = get_free_pbuf_wrapper();
	if (!p)
		return GLUE_ERR_ABRT;
	
	// reference lwip buffer to send
	p->ref = ref2save;
	p->pbuf.payload = rawdata;
	p->pbuf.len = p->pbuf.tot_len = size;
	p->pbuf.next = NULL;
	p->pbuf.type = PBUF_CUSTOM_TYPE_STATIC;
	p->pbuf.ref = 0;
	p->pbuf.flags = 0;

	struct netif* netif = netif_esp[netif_idx];

	uprint("LINKOUTPUT: real packet sent to wilderness (%dB pb=%p netif=esp-%p)\n",
		(int)size,
		&p->pbuf,
		netif);
	
	// call blobs
	// blobs will call pbuf_free() back later
	// we will retreive our ref2save and give it back to glue
	return esp2glue_err(netif->linkoutput(netif, &p->pbuf));
}

void blobs_getinfo (void)
{
	struct netif* test_netif_sta = eagle_lwip_getif(STATION_IF);
	struct netif* test_netif_ap = eagle_lwip_getif(SOFTAP_IF);
	
	uassert(!netif_sta || test_netif_sta == netif_sta);
	uassert(!netif_ap || test_netif_ap == netif_ap);
	
	netif_sta = test_netif_sta;
	netif_ap = test_netif_ap;
}

// some checks + give netif index
static int esp_netif_update (struct netif* netif)
{
	blobs_getinfo();

	uassert(netif->input == ethernet_input);
	uassert(netif->output == etharp_output);

	int netif_idx;
	if (netif == netif_sta)
		netif_idx = STATION_IF;
	else if (netif == netif_ap)
		netif_idx = SOFTAP_IF;
	else
		return -1;

	esp2glue_netif_updated(
		netif_idx,
		netif->ip_addr.addr,
		netif->netmask.addr,
		netif->gw.addr,
		esp2glue_netif_flags(netif->flags),
		netif->hwaddr_len,
		netif->hwaddr
		/*netif->state*/);
	
	return netif_idx;
}

///////////////////////////////////////
// STUBS / wrappers

void lwip_init_RENAMED (void); //XXX should use esp2glue_lwip_init()
void lwip_init (void)
{
	for (int i = 0; i < PBUF_CUSTOM_NUMBER; i++)
		pbuf_wrappers[i].ref = NULL;
	
	blobs_getinfo();
	
	esp2glue_lwip_init();
	
	uprint("WRAP lwip_init\n");
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
err_t etharp_output (struct netif* netif, struct pbuf* q, ip_addr_t* ipaddr)
{
	(void)netif; (void)q; (void)ipaddr;
	//STUB(etharp_output);
	uerror("ERROR: STUB etharp_output should not be called\n");
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
err_t ethernet_input (struct pbuf* p, struct netif* netif)
{
	uprint("WRAP: received (pbuf: %dB ref=%d eb=%p) on netif ", p->tot_len, p->ref, p->eb);
	stub_display_netif(netif);
	
	int netif_idx = esp_netif_update(netif);

	uassert(p->tot_len == p->len && p->ref == 1);
	
#if UDEBUG
	if (   memcmp((const char*)p->payload, netif->hwaddr, 6) == 0
	    || memcmp((const char*)p->payload, ethbroadcast.addr, 6) == 0)
	{
		dump("ethinput", p->payload, p->len);
	}
#endif

	// copy esp pbuf to glue pbuf

	void* glue_pbuf;
	void* glue_data;

	// ask glue for space to store payload into
	esp2glue_alloc_for_recv(p->len, &glue_pbuf, &glue_data);
	if (!glue_pbuf)
	{
		pbuf_free(p);
		return ERR_MEM;
	}

	// copy data
	os_memcpy(glue_data, p->payload, p->len);
	// release asap blob's buffer
	// thus avoiding the BMOD "LmacRxBlk:1"
	pbuf_free(p);
	// pass to new ip stack
	return glue2esp_err(esp2glue_ethernet_input(netif_idx, glue_pbuf));
}

void dhcps_start (struct ip_info* info)
{
	// at that point, assume that serial port is open for printing debug output
	doprint_allow = 1;

	uprint("WRAP dhcps_start ");
	display_ip_info(info);
	uprint("\n");
	
	blobs_getinfo();

 	esp2glue_dhcps_start(info);
 	
	if (netif_ap)
		///XXX this is mandatory for blobs to be happy
		// but we should get this info back through glue
	 	netif_ap->flags |= NETIF_FLAG_UP | NETIF_FLAG_LINK_UP;
}

void dhcps_stop (void)
{
	// not implemented yet
	
	STUB(dhcps_stop);
}

void espconn_init (void)
{
	// not implemented yet
	
	STUB(espconn_init);

	blobs_getinfo();
}

void dhcp_cleanup (struct netif *netif)
{
	// not implemented yet

	STUB(dhcp_cleanup);
	stub_display_netif(netif); nl();
}

err_t dhcp_release (struct netif *netif)
{
	// not implemented yet
	
	STUB(dhcp_release);
	stub_display_netif(netif); nl();
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
	// at that point, assume that serial port is open for printing debug output
	doprint_allow = 1;

	blobs_getinfo();
	
	uprint("WRAP: dhcp_start ");
	stub_display_netif(netif);

	esp_netif_update(netif);
	return glue2esp_err(esp2glue_dhcp_start());
}

void dhcp_stop (struct netif *netif)
{
	// not implemented yet

	STUB(dhcp_stop);
	stub_display_netif(netif); nl();
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
 
struct netif* netif_add (struct netif *netif, ip_addr_t *ipaddr, ip_addr_t *netmask, ip_addr_t *gw, void *state, netif_init_fn init, netif_input_fn packet_incoming)
{
	// netif->output is "packet_ougtoing" already initialized
	// packet_incoming is given for us to put in *netif
	//STUB(netif_add);

	//display_ip32("STUB: netif_add ip=", *ipaddr);
	//display_ip32(" mask=", *netmask);
	//display_ip32(" gw=", *gw);
	//uprint(" state=%p init=%p input=%p ", state, init, packet_incoming);
	//stub_display_netif(netif);
	//uprint("\n");
	
	blobs_getinfo();
	
	//////////////////////////////
	// this is revisited ESP lwip implementation
	netif->ip_addr.addr = 0;
	netif->netmask.addr = 0;
	netif->gw.addr = 0;
	netif->flags = 0;
	#if LWIP_DHCP
	// ok
	netif->dhcp = NULL;
	netif->dhcps_pcb = NULL;
	#endif /* LWIP_DHCP */
		#if LWIP_AUTOIP
		#error
		netif->autoip = NULL;
		#endif /* LWIP_AUTOIP */
		#if LWIP_NETIF_STATUS_CALLBACK
		#error
		netif->status_callback = NULL;
		#endif /* LWIP_NETIF_STATUS_CALLBACK */
		#if LWIP_NETIF_LINK_CALLBACK
		#error
		netif->link_callback = NULL;
		#endif /* LWIP_NETIF_LINK_CALLBACK */
	#if LWIP_IGMP
	// ok
	netif->igmp_mac_filter = NULL;
	#endif /* LWIP_IGMP */
		#if ENABLE_LOOPBACK
		#error
		netif->loop_first = NULL;
		netif->loop_last = NULL;
		#endif /* ENABLE_LOOPBACK */
	netif->state = state;
	netif->num = 1;//netifnum++;
	netif->input = packet_incoming; // = (old above)ethernet_input(never called)
		#if LWIP_NETIF_HWADDRHINT
		#error
		netif->addr_hint = NULL;
		#endif /* LWIP_NETIF_HWADDRHINT*/
		#if ENABLE_LOOPBACK && LWIP_LOOPBACK_MAX_PBUFS
		#error
		netif->loop_cnt_current = 0;
		#endif /* ENABLE_LOOPBACK && LWIP_LOOPBACK_MAX_PBUFS */

	//XXX fixme init() is from blobs to call blobs, unknown yet
	if (init(netif) != ERR_OK)
	{
		uprint("ERROR netif_add: caller's init() failed\n");
		return NULL;
	}
	netif->next = NULL; //netif_list;
	//netif_list = netif;
//XXX	snmp_inc_iflist();
	#if LWIP_IGMP
	// ok
//XXX	if (netif->flags & NETIF_FLAG_IGMP)
//XXX		igmp_start(netif);
	#endif /* LWIP_IGMP */
	//////////////////////////////

	uprint("WRAP: netif add:\n");
	netif_set_addr(netif, ipaddr, netmask, gw);
	
	return netif;
}


/**
 * Remove a network interface from the list of lwIP netifs.
 *
 * @param netif the network interface to remove
 */
void netif_remove (struct netif *netif)
{
	uprint("STUB netif_remove ");
	stub_display_netif(netif); nl();
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
void netif_set_addr (struct netif* netif, ip_addr_t* ipaddr, ip_addr_t* netmask, ip_addr_t* gw)
{
	blobs_getinfo();

	netif->ip_addr.addr = ipaddr->addr;
	netif->netmask.addr = netmask->addr;
	netif->gw.addr = gw->addr;
	int netif_idx = esp_netif_update(netif);

	// tell blobs
	struct ip_info set;
	set.ip.addr = ipaddr->addr;
	set.netmask.addr = netmask->addr;
	set.gw.addr = gw->addr;
	wifi_set_ip_info(netif_idx, &set);

	uprint("WRAP: netif_set_addr ");
	stub_display_netif(netif);
}

/**
 * Set a network interface as the default network interface
 * (used to output all packets for which no specific route is found)
 *
 * @param netif the default network interface
 */
void netif_set_default (struct netif* netif)
{
	uprint("WRAP: netif_set_default ");
	stub_display_netif(netif);
	netif_default = netif;

	esp_netif_update(netif);
}

/**
 * Bring an interface down, disabling any traffic processing.
 *
 * @note: Enabling DHCP on a down interface will make it come
 * up once configured.
 * 
 * @see dhcp_start()
 */ 
void netif_set_down (struct netif* netif)
{
	uprint("WRAP: netif_set_down  ");
	stub_display_netif(netif);
	
	netif->flags &= ~NETIF_FLAG_UP;
	esp_netif_update(netif);
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
void netif_set_up (struct netif* netif)
{
	uprint("WRAP: netif_set_up ");
	stub_display_netif(netif);

	netif->flags |= NETIF_FLAG_UP;
	esp_netif_update(netif);
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

struct pbuf* pbuf_alloc (pbuf_layer layer, u16_t length, pbuf_type type)
{
	// pbuf creation from blobs
	// copy parts of original code matching specific requests

	//STUB(pbuf_alloc);
//	pbuf_info("pbuf_alloc", layer, length, type);
	
	u16_t offset = 0;
	if (layer == PBUF_RAW && type == PBUF_RAM)
	{
		offset += EP_OFFSET;
		
		/* If pbuf is to be allocated in RAM, allocate memory for it. */
		size_t alloclen = LWIP_MEM_ALIGN_SIZE(SIZEOF_STRUCT_PBUF + offset) + LWIP_MEM_ALIGN_SIZE(length);
		struct pbuf* p = (struct pbuf*)mem_malloc(alloclen);
		if (p == NULL)
			return NULL;
		/* Set up internal structure of the pbuf. */
		p->payload = LWIP_MEM_ALIGN((void *)((u8_t *)p + SIZEOF_STRUCT_PBUF + offset));
		p->len = p->tot_len = length;
		p->next = NULL;
		p->type = type;
		p->eb = NULL;
		p->ref = 1;
		p->flags = 0;
//		uprint("WRAP: pbuf_alloc-> %p (%d bytes)\n", p, alloclen);
		return p;
	}
	
	if (layer == PBUF_RAW && type == PBUF_REF)
	{
		//unused: offset += EP_OFFSET;
		size_t alloclen = LWIP_MEM_ALIGN_SIZE(SIZEOF_STRUCT_PBUF);
		struct pbuf* p = (struct pbuf*)mem_malloc(alloclen);
		if (p == NULL)
			return NULL;
		p->payload = NULL;
		p->len = p->tot_len = length;
		p->next = NULL;
		p->type = type;
		p->eb = NULL;
		p->ref = 1;
		p->flags = 0;
//		uprint("WRAP: pbuf_alloc-> %p (%d bytes)\n", p, alloclen);
		return p;
	}

	uerror("WRAP: pbuf_alloc BAD CASE\n");
		
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

u8_t pbuf_free (struct pbuf *p)
{
	//STUB(pbuf_free);
	//uprint("WRAP: pbuf_free(%p) ref=%d type=%d\n", p, p->ref, p->type);
//	pbuf_info("pbuf_free", -1, p->len, p->type);
//	uprint("pbuf@%p ref=%d tot_len=%d eb=%p\n", p, p->ref, p->tot_len, p->eb);
	
	#if LWIP_SUPPORT_CUSTOM_PBUF
	#error LWIP_SUPPORT_CUSTOM_PBUF is defined
	#endif

	if (p->type == PBUF_CUSTOM_TYPE_STATIC)
	{
		struct pbuf_wrapper* pw = (struct pbuf_wrapper*)( (char*)p - ((char*)&pbuf_wrappers[0].pbuf - (char*)&pbuf_wrappers[0]) );
		// pw->ref is the lwip2 pbuf to release, the current lwip1 pbuf points inside it
		uprint("WRAP: pbuf_free release lwip2 pbuf %p lwip1 %p\n", pw->ref, &pw->pbuf);
		esp2glue_ref_freed(pw->ref);
		pw->ref = NULL; // release our pooled static pbuf_wrapper

		if (pw->pbuf.ref != 1)
			uprint("ERROR bad pbuf_wrapper ref=%d\n", pw->pbuf.ref);
		
		return 1;
	}
		
	if (!p->next && p->ref == 1)
	{
		if (p->eb)
			system_pp_recycle_rx_pkt(p->eb);
		if (p->type == PBUF_RAM || p->type == PBUF_REF || p->type == PBUF_ESF_RX)
		{
			mem_free(p);
			return 1;
		}
	}

	uerror("BAD CASE %p ref=%d tot_len=%d eb=%p\n", p, p->ref, p->tot_len, p->eb);
	pbuf_info("BAD CASE", -1, p->len, p->type);
	return 0;
}

/**
 * Increment the reference count of the pbuf.
 *
 * @param p pbuf to increase reference counter of
 *
 */
void pbuf_ref (struct pbuf *p)
{
	//STUB(pbuf_ref);
	uprint("WRAP: pbuf_ref(%p) ref=%d\n", p, p->ref);
	if (p)
		++(p->ref);
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
	(void)msecs; (void)handler; (void)arg;
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
(void)handler; (void)arg;
	STUB(sys_untimeout);
}

void glue2esp_ifup (int netif_idx, uint32_t ip, uint32_t mask, uint32_t gw)
{
	struct netif* netif = netif_esp[netif_idx];

	// backup old esp ips
	ip_addr_t oldip, oldmask, oldgw;
	oldip = netif->ip_addr;
	oldmask = netif->netmask;
	oldgw = netif->gw;
	        
	// change ips
	netif->ip_addr.addr = ip;
	netif->netmask.addr = mask;
	netif->gw.addr = gw;
	// set up
	netif->flags |= NETIF_FLAG_UP;

	// tell esp to check it has changed (by giving old ones)
	system_station_got_ip_set(&oldip, &oldmask, &oldgw);
}
