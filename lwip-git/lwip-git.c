
#include "lwipopts.h"
#include "lwip/err.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/dhcp.h"
#include "lwip/etharp.h"
#include "netif/ethernet.h"
#include "lwip/app/dhcpserver.h"

#include "glue.h"
#include "lwip-helper.h"

static char hostname_sta[32];

#define netif_sta (&netif_git[STATION_IF])
#define netif_ap  (&netif_git[SOFTAP_IF])
struct netif netif_git[2];
int netif_git_initialized[2] = { 0, 0 }; //XXX or char?
const char netif_name[2][8] = { "station", "soft-ap" };

int ntp_servers_number = 0;
ip4_addr_t* ntp_servers = NULL;

err_t glue2git_err (err_glue_t err)
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

u8_t glue2git_netif_flags (glue_netif_flags_t flags)
{

//XXXFIXME this is the paranoia mode
// make it simpler in non-debug mode

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
		uerror("ERROR: glue2git_netif_flags: remaining flags not converted (0x%x->0x%x)\n", copy, flags);
	return nf;
}

#if UDEBUG

static void new_display_netif_flags (int flags)
{
	#define IFF(x)	do { if (flags & NETIF_FLAG_##x) uprint("|" #x); } while (0)
	IFF(UP);
	IFF(BROADCAST);
	IFF(LINK_UP);
	IFF(ETHARP);
	IFF(ETHERNET);
	IFF(IGMP);
	IFF(MLD6);
	#undef IFF
}

static const char* new_netif_name (struct netif* netif)
{
	uassert(netif == netif_sta || netif == netif_ap);
	return netif == netif_ap? "AP": "STA";
}

static void new_display_netif (struct netif* netif)
{
	
	uprint("lwip-@%p %s name=%c%c%d mtu=%d state=%p ",
		netif,
		new_netif_name(netif),
		netif->name[0], netif->name[1], netif->num,
		netif->mtu,
		netif->state);
	if (netif->hwaddr_len == 6)
		display_mac(netif->hwaddr);
	new_display_netif_flags(netif->flags);
	display_ip32(" ip=", netif->ip_addr.addr);
	display_ip32(" mask=", netif->netmask.addr);
	display_ip32(" gw=", netif->gw.addr);
	uprint("\n");
}

#else // !UDEBUG

#define new_display_netif_flags(x) do { (void)0; } while (0)
#define new_display_netif(x) do { (void)0; } while (0)

#endif // !UDEBUG

int lwiperr_check (const char* what, err_t err)
{
	if (err != ERR_OK)
	{
		uerror("ERROR: %s (error %d)\n", what, err);
		return 0;
	}
	return 1;
}

err_glue_t esp2glue_dhcp_start ()
{
	uprint("GLUE: dhcp_start netif: ");
	new_display_netif(netif_sta);
	err_t err = dhcp_start(netif_sta);
	uprint("GLUE: new_dhcp_start returns %d\n", err);
	return new2glue_err(err);
}

void dhcp_set_ntp_servers (u8_t number, const ip4_addr_t* ntp_server_addrs)
{
	uprint("GLUE: %d ntp-server known\n", number);
	if (ntp_servers)
		mem_free(ntp_servers);
	if (!number)
	{
		ntp_servers = NULL;
		ntp_servers_number = 0;
		return;
	}
	size_t size = number * sizeof(ip4_addr_t);
	ntp_servers = mem_malloc(size);
	if (!ntp_servers)
	{
		uerror("ERROR alloc(%d) failed for %d ntp server address\n", (int)size, number);
		ntp_servers_number = 0;
		return;
	}
	ntp_servers_number = number;
	os_memcpy(ntp_servers, ntp_server_addrs, size);
#if UDEBUG
	for (int i = 0; i < number; i++)
	{
		display_ip32("ntp: ", ntp_servers[i].addr);
		nl();
	}
#endif
}

#if 1
void check_chain (const char* pre, struct pbuf* head)
{
uprint("%s:A", pre);
	struct pbuf* qq = head;
	size_t rr = qq->tot_len;
uprint("B");
	while (rr > 0)
	{
		rr -= qq->len;
uprint("C(qq=%p next=%p)", qq, qq->next);
		qq = qq->next;
uprint("D");

// chain is damaged. check esp-pbuf->eb and EP_OFFSET / PBUF_LINK_ENCAPSULATION_HLEN honoured in esp-pbuf_alloc
// check if in lwip-git/pbuf_alloc PBUF_LINK_ENCAPSULATION_HLEN is honoured as in original esp-pbuf_alloc

	}
	if (qq)
		uprint("ERROR IN CHAIN AFTER %p\n", qq);
	else
		uprint("CHAIN OK\n");
}
#endif

err_t new_linkoutput (struct netif* netif, struct pbuf* head)
{

uassert(head->next == NULL); ///XXXFIXME REWRITE/SIMPLIFY FOR ONE UNCHAINED PBUF

	int netif_idx = netif == netif_sta? STATION_IF: SOFTAP_IF;
	struct pbuf* p;
	void* esp_head = NULL;
	
	// reserve pbuf in esp side
	void* first = NULL;
	void* last = NULL;
	for (p = head; p; p = p->next)
		if (glue2esp_reserve_pbuf_chain(&first, &last, head, p->payload, p->tot_len, p->len) == GLUE_ERR_MEM)
			return ERR_MEM;

check_chain("1", head);
	if (p)
		return ERR_MEM;
	
	// ref pbuf in our side
	// esp will be free them with esp2glue_ref_freed() below
	for (p = head; p; p = p->next)
		pbuf_ref(p);

	// tell esp the chain is ready to be sent
	err_t err = glue2git_err(glue2esp_linkoutput(first, netif == netif_sta? STATION_IF: SOFTAP_IF));
check_chain("2", head);
	if (err != ERR_OK)
	{
		pbuf_free(head);
		uprint("GLUE: linkoutput error sending pbuf@%p\n", p);
		return err;
	}
		
check_chain("3", head);

	return ERR_OK;
}

void esp2glue_pbuf_freed (void* pbuf)
{
	uprint("GLUE: blobs release lwip-pbuf (ref=%d) @%p\n", ((struct pbuf*)pbuf)->ref, pbuf);
check_chain("4", (struct pbuf*)pbuf);
	pbuf_free((struct pbuf*)pbuf);
}

static err_t new_input (struct pbuf *p, struct netif *inp)
{
	(void)p;
	(void)inp;
	uerror("internal error, new-netif->input() cannot be called\n");
	return ERR_ABRT;
}

static void netif_status_callback (struct netif* netif)
{
	uprint("GLUE: netif status callback ");
	new_display_netif(netif);
	
	if (netif->flags & NETIF_FLAG_LINK_UP)
	{
		// tell ESP that link is up
		glue2esp_ifup(netif == netif_sta? STATION_IF: SOFTAP_IF, netif->ip_addr.addr, netif->netmask.addr, netif->gw.addr);

		if (netif == netif_sta)
			// this is our default route
			netif_set_default(netif);
	}
}

static void setup_netif (int netif_idx)
{
	if (netif_git_initialized[netif_idx])
		return;
	netif_git_initialized[netif_idx] = 1;
	struct netif* netif = &netif_git[netif_idx];

	#if !LWIP_SINGLE_NETIF
	netif->next = NULL;
	#endif

	#if LWIP_IPV4
	/** IP address configuration in network byte order */
	netif->ip_addr.addr = 0;
	netif->netmask.addr = 0;
	netif->gw.addr = 0;
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

	netif->input = new_input;
	netif->output = etharp_output; //new_ipv4output;
	netif->linkoutput = new_linkoutput;

		#if LWIP_IPV6
		#error
		netif->output_ip6 = blah
		#endif /* LWIP_IPV6 */

	#if LWIP_NETIF_STATUS_CALLBACK
	netif->status_callback = netif_status_callback;
	#endif /* LWIP_NETIF_STATUS_CALLBACK */

		#if LWIP_NETIF_LINK_CALLBACK
		#error
		netif->link_callback = blah
		#endif /* LWIP_NETIF_LINK_CALLBACK */

		#if LWIP_NETIF_REMOVE_CALLBACK
		#error
		netif->remove_callback = blah
		#endif /* LWIP_NETIF_REMOVE_CALLBACK */
	
	netif->state = NULL;

	#ifdef netif_get_client_data
	// defined
	//XXX what is this
	//void* client_data[LWIP_NETIF_CLIENT_DATA_INDEX_MAX + LWIP_NUM_NETIF_CLIENT_DATA];
	#endif
	
	#if LWIP_NETIF_HOSTNAME
	netif->hostname = hostname_sta;
	#endif /* LWIP_NETIF_HOSTNAME */
	
	netif->chksum_flags = NETIF_CHECKSUM_ENABLE_ALL;
	netif->mtu = TCP_MSS + 40;
	
	//hwaddr[NETIF_MAX_HWADDR_LEN];
	netif->hwaddr_len = 0;

	netif->flags = 0;

	/** descriptive abbreviation */
	if (netif_idx == STATION_IF)
	{
		netif->name[0] = 's';
		netif->name[1] = 't';
	}
	else
	{
		netif->name[0] = 'a';
		netif->name[1] = 'p';
	}
	
	/** number of this interface. Used for @ref if_api and @ref netifapi_netif,
	* as well as for IPv6 zones */
	netif->num = 0;

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
	netif->igmp_mac_filter = NULL;
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
};

void esp2glue_lwip_init (void)
{
	memset(&netif_git[0], 0, sizeof(netif_git[0]));
	memset(&netif_git[1], 0, sizeof(netif_git[1]));

	lwip_init();
}

void esp2glue_netif_updated (int netif_idx, uint32_t ip, uint32_t mask, uint32_t gw, glue_netif_flags_t flags, uint32_t hwlen, const uint8_t* hw /*, void* state*/)
{

//XXX blorgl here. netif can be updated from both side. two-way update to setup

	struct netif* netif = &netif_git[netif_idx];

	if (!netif_git_initialized[netif_idx])
		setup_netif(netif_idx);
	
	if (!netif->ip_addr.addr)
	{
		netif->ip_addr.addr = ip;
		netif->netmask.addr = mask;
		netif->gw.addr = gw;
		//netif->state = state; // do not overwrite, dhcps uses it as a udp_pcb
	}

	netif->flags |= glue2git_netif_flags(flags);
	// this was not done in old lwip and is needed for dhcp client
	netif->flags |= NETIF_FLAG_UP;

	if (netif->hwaddr_len != 6)
	{
		uassert(hwlen == 0 || hwlen == 6);
		netif->hwaddr_len = hwlen;
		if (hwlen == 6)
		{
			os_memcpy(netif->hwaddr, hw, hwlen);
			sprintf(hostname_sta, "esp8266_%02x%02x%02x%02x%02x%02x", hw[0], hw[1], hw[2], hw[3], hw[4], hw[5]);
		}
		else
			hostname_sta[0] = 0;
	}
	
	uprint("GLUE: netif updated: ");
	new_display_netif(netif);
}

void esp2glue_alloc_for_recv (size_t len, void** pbuf, void** data)
{
	*pbuf = pbuf_alloc(PBUF_RAW, len, PBUF_RAM);
	if (*pbuf)
		*data = ((struct pbuf*)*pbuf)->payload;
}

err_glue_t esp2glue_ethernet_input (int netif_idx, void* received)
{
	// this input is allocated by esp2glue_alloc_for_recv()

	//uprint("GLUE: input idx=%d netif-flags=0x%x ", netif_idx, netif_git[netif_idx].flags);
	//display_ip32(" ip=", netif_git[netif_idx].ip_addr.addr);
	//nl();
	
	return new2glue_err(ethernet_input((struct pbuf*)received, &netif_git[netif_idx]));
}

void esp2glue_dhcps_start (struct ip_info* info)
{
	dhcps_start(info);
}
