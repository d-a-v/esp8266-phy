
#include "lwipopts.h"
#include "lwip/err.h"
#include "lwip/netif.h"
#include "lwip/dhcp.h"
#include "lwip/etharp.h"
#include "netif/ethernet.h"

#include "glue.h"

static char hostname_sta[32];
static struct netif netif_new;

err_t glue2new_err (err_glue_t err)
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
	case GLUE_ERR_USE        : return ERR_USE;
	case GLUE_ERR_ALREADY    : return ERR_ALREADY;
	case GLUE_ERR_ISCONN     : return ERR_ISCONN;
	case GLUE_ERR_CONN       : return ERR_CONN;
	case GLUE_ERR_IF         : return ERR_IF;
	case GLUE_ERR_ABRT       : return ERR_ABRT;
	case GLUE_ERR_RST        : return ERR_RST;
	case GLUE_ERR_CLSD       : return ERR_CLSD;
	case GLUE_ERR_ARG        : return ERR_ARG;
	
	default: return ERR_ABRT;
	}
};	

err_glue_t new2glue_err (err_t err)
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
	case ERR_USE        : return GLUE_ERR_USE;
	case ERR_ALREADY    : return GLUE_ERR_ALREADY;
	case ERR_ISCONN     : return GLUE_ERR_ISCONN;
	case ERR_CONN       : return GLUE_ERR_CONN;
	case ERR_IF         : return GLUE_ERR_IF;
	case ERR_ABRT       : return GLUE_ERR_ABRT;
	case ERR_RST        : return GLUE_ERR_RST;
	case ERR_CLSD       : return GLUE_ERR_CLSD;
	case ERR_ARG        : return GLUE_ERR_ARG;

	default: return GLUE_ERR_ABRT;
	}
};	

u8_t glue2new_netif_flags (glue_netif_flags_t flags)
{
	glue_netif_flags_t copy = flags;
	u8_t nf = 0;
	#define CF(x)	do { if (flags & GLUE_NETIF_FLAG_##x) { nf |= NETIF_FLAG_##x; flags &= ~GLUE_NETIF_FLAG_##x; } } while (0)
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
		uerror("ERROR: old2glue_netif_flags: remaining flags not converted (0x%x->0x%x)\n", copy, flags);
	return nf;
}

static void new_display_netif_flags (int flags)
{
	#define IFF(x)	do { if (flags & NETIF_FLAG_##x) uerror("|" #x); } while (0)
	IFF(UP);
	IFF(BROADCAST);
	IFF(LINK_UP);
	IFF(ETHARP);
	IFF(ETHERNET);
	IFF(IGMP);
	IFF(MLD6);
	#undef IFF
}

err_glue_t glue_oldcall_dhcp_start ()
{
	uprint("new_dhcp_start netif@%p\n", &netif_new);
	err_t err = dhcp_start(&netif_new);
	uprint("new_dhcp_start returns %d\n", err);
	return new2glue_err(err);
}

void dhcp_set_ntp_servers(u8_t num_ntp_servers, const ip4_addr_t* ntp_server_addrs)
{
	(void)ntp_server_addrs;
	os_printf("NEW: %dx ntp server address received\n", num_ntp_servers);
}

err_t new_linkoutput (struct netif *netif, struct pbuf *p)
{
	uprint("NEW linkoutput netif@%p pbuf@%p len=%d totlen=%d type=%d\n", netif, p, p->len, p->tot_len, p->type);
	dump("SENDING", p->payload, p->len);
	pbuf_ref(p); // freed by glue2new_pbuf_wrapper_free() below
	return glue2new_err(glue2old_linkoutput(p, p->payload, p->len));
}

void glue2new_pbuf_wrapper_free (void* pbuf)
{
	pbuf_free((struct pbuf*)pbuf);
}

#if 0
err_t new_ipv4output (struct netif *netif, struct pbuf *p, const ip4_addr_t *ipaddr)
{
	uprint("NEW ipv4-output netif@%p pbuf@%p len=%d totlen=%d type=%d\n", netif, p, p->len, p->tot_len, p->type);
	display_ip32("dstip=", ipaddr->addr);
	nl();
	//dump("pbuf", p->payload, p->len);
	
	return etharp_output(netif, p, ipaddr);
}
#endif

static err_t new_input (struct pbuf *p, struct netif *inp)
{
	(void)p;
	(void)inp;
	uerror("internal error, new-netif->input() cannot be called\n");
	return ERR_ABRT;
}

static void netif_status_callback (struct netif* netif)
{
	uprint("NEW: netif status callback: flags=");
	new_display_netif_flags(netif->flags);
	display_ip32(" ip=", netif->ip_addr.addr);
	display_ip32(" mask=", netif->netmask.addr);
	display_ip32(" gw=", netif->gw.addr);
	uprint("\n");
	
	if (netif->flags & NETIF_FLAG_LINK_UP)
	{
		// tell ESP that link is up
		glue_new2esp_ifup(netif->ip_addr.addr, netif->netmask.addr, netif->gw.addr);

		// this is our default route
		netif_set_default(netif);
	}
}

static char setup_new_netif (void)
{
	static char initialized = 0;
	if (initialized)
		return 0;
	initialized = 1;

	#if !LWIP_SINGLE_NETIF
	netif_new.next = NULL;
	#endif

	#if LWIP_IPV4
	/** IP address configuration in network byte order */
	netif_new.ip_addr.addr = 0;
	netif_new.netmask.addr = 0;
	netif_new.gw.addr = 0;
	#endif /* LWIP_IPV4 */
	
		#if LWIP_IPV6
		#error
		//ip_addr_t ip6_addr[LWIP_IPV6_NUM_ADDRESSES];
		//u8_t ip6_addr_state[LWIP_IPV6_NUM_ADDRESSES];
		#if LWIP_IPV6_ADDRESS_LIFETIMES
		/** Remaining valid and preferred lifetime of each IPv6 address, in seconds.
		   * For valid lifetimes, the special value of IP6_ADDR_LIFE_STATIC (0)
		   * indicates the address is static and has no lifetimes. */
		//u32_t ip6_addr_valid_life[LWIP_IPV6_NUM_ADDRESSES];
		//u32_t ip6_addr_pref_life[LWIP_IPV6_NUM_ADDRESSES];
		#endif /* LWIP_IPV6_ADDRESS_LIFETIMES */
		#endif /* LWIP_IPV6 */

	netif_new.input = new_input;
	netif_new.output = etharp_output; //new_ipv4output;
	netif_new.linkoutput = new_linkoutput;

		#if LWIP_IPV6
		#error
		netif_new.output_ip6 = blah
		#endif /* LWIP_IPV6 */

	#if LWIP_NETIF_STATUS_CALLBACK
	netif_new.status_callback = netif_status_callback;
	#endif /* LWIP_NETIF_STATUS_CALLBACK */

		#if LWIP_NETIF_LINK_CALLBACK
		#error
		netif_new.link_callback = blah
		#endif /* LWIP_NETIF_LINK_CALLBACK */

		#if LWIP_NETIF_REMOVE_CALLBACK
		#error
		netif_new.remove_callback = blah
		#endif /* LWIP_NETIF_REMOVE_CALLBACK */
	
	netif_new.state = NULL;

	#ifdef netif_get_client_data
	// defined
	//XXX what is this
	//void* client_data[LWIP_NETIF_CLIENT_DATA_INDEX_MAX + LWIP_NUM_NETIF_CLIENT_DATA];
	#endif
	
	#if LWIP_NETIF_HOSTNAME
	netif_new.hostname = hostname_sta;
	#endif /* LWIP_NETIF_HOSTNAME */
	
	netif_new.chksum_flags = NETIF_CHECKSUM_ENABLE_ALL;
	netif_new.mtu = TCP_MSS + 40;
	
	//hwaddr[NETIF_MAX_HWADDR_LEN];
	netif_new.hwaddr_len = 0;

	netif_new.flags = 0;

	/** descriptive abbreviation */
	netif_new.name[0] = 'w';
	netif_new.name[1] = 'l';
	/** number of this interface. Used for @ref if_api and @ref netifapi_netif,
	* as well as for IPv6 zones */
	netif_new.num = 0;

		#if LWIP_IPV6_AUTOCONFIG
		#error
		/** is this netif enabled for IPv6 autoconfiguration */
		u8_t ip6_autoconfig_enabled;
		#endif /* LWIP_IPV6_AUTOCONFIG */
		
		#if LWIP_IPV6_SEND_ROUTER_SOLICIT
		#error
		/** Number of Router Solicitation messages that remain to be sent. */
		u8_t rs_count;
		#endif /* LWIP_IPV6_SEND_ROUTER_SOLICIT */
	
		#if MIB2_STATS
		#error
		/** link type (from "snmp_ifType" enum from snmp_mib2.h) */
		u8_t link_type;
		/** (estimate) link speed */
		u32_t link_speed;
		/** timestamp at last change made (up/down) */
		u32_t ts;
		/** counters */
		struct stats_mib2_netif_ctrs mib2_counters;
		#endif /* MIB2_STATS */

	#if LWIP_IPV4 && LWIP_IGMP
	  /** This function could be called to add or delete an entry in the multicast
	      filter table of the ethernet MAC.*/
	netif_new.igmp_mac_filter = NULL;
	#endif /* LWIP_IPV4 && LWIP_IGMP */

		#if LWIP_IPV6 && LWIP_IPV6_MLD
		#error
		  /** This function could be called to add or delete an entry in the IPv6 multicast
		      filter table of the ethernet MAC. */
		  netif_mld_mac_filter_fn mld_mac_filter;
		#endif /* LWIP_IPV6 && LWIP_IPV6_MLD */

		#if LWIP_NETIF_HWADDRHINT
		#error
		  u8_t *addr_hint;
		#endif /* LWIP_NETIF_HWADDRHINT */

		#if ENABLE_LOOPBACK
		#error
		  /* List of packets to be queued for ourselves. */
		  struct pbuf *loop_first;
		  struct pbuf *loop_last;
		#if LWIP_LOOPBACK_MAX_PBUFS
		  u16_t loop_cnt_current;
		#endif /* LWIP_LOOPBACK_MAX_PBUFS */
		#endif /* ENABLE_LOOPBACK */

	return 1;
};

void old2glue_oldnetif_updated (uint32_t ip, uint32_t mask, uint32_t gw, uint16_t flags, uint8_t hwlen, const uint8_t* hw, void* state)
{

//XXX blorgl here. netif can be updated from both side. two-way update to setup

	if (setup_new_netif())
	{
		netif_new.ip_addr.addr = ip;
		netif_new.netmask.addr = mask;
		netif_new.gw.addr = gw;
		netif_new.state = state; // useless: new-lwip does not use it
	}
	// LINK_UP is always brought by ESP
	// but it is really set once netif's status callback is called
	netif_new.flags = glue2new_netif_flags(flags);

	if (netif_new.hwaddr_len != 6)
	{
		uassert(hwlen == 0 || hwlen == 6);
		netif_new.hwaddr_len = hwlen;
		if (hwlen == 6)
		{
			os_memcpy(netif_new.hwaddr, hw, hwlen);
			sprintf(hostname_sta, "esp8266_%02x%02x%02x%02x%02x%02x", hw[0], hw[1], hw[2], hw[3], hw[4], hw[5]);
		}
		else
			hostname_sta[0] = 0;
	}
	
	// this was not done in old lwip and is needed for dhcp
	netif_new.flags |= NETIF_FLAG_UP;
	
	uprint("NEW: netif set up by ESP: flags=");
	new_display_netif_flags(netif_new.flags);
	nl();
}

void glue_alloc_received (uint16_t len, void** pbuf, void** data)
{
	*pbuf = pbuf_alloc(PBUF_RAW, len, PBUF_RAM);
	if (*pbuf)
		*data = ((struct pbuf*)*pbuf)->payload;
}

err_glue_t glue_oldcall_ethernet_input (void* received)
{
	// allocate a ram pbuf
	return new2glue_err(ethernet_input((struct pbuf*)received, &netif_new));
}