
#include "arch/cc.h"
#include "lwip/err.h"
#include "lwip/netif.h"

#include "glue-stub.h"

struct netif netif_global;
struct netif_glue netif_glue_global;

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

#if 0
struct netif* glue2new_netif (struct netif_glue* glue)
{
	// so far: do nothing for netif
	(void)glue;
	
	static char initialized = 0;
	if (!initialized)
	{
		initialized = 1;
		os_memset(&netif_global, 0, sizeof netif_global);
		//netif_global.flags |= NETIF_FLAG_UP;
	}
	
	return &netif_global;
}
#endif

err_glue_t glue_oldcall_dhcp_start ()
{
	return new2glue_err(dhcp_start(netif_global));
}

void dhcp_set_ntp_servers(u8_t num_ntp_servers, ip_addr_t* ntp_server_addrs)
{
	(void)ntp_server_addrs;
	os_printf("NEW: %dx ntp server address received\n", num_ntp_servers);
}

void old2glue_oldnetif_updated (uint32_t ip, uint32_t mask, uint32_t gw, uint16_t flags, void* state)
{
	bufprint("TODO old2glue_oldnetif_updated\n");
}