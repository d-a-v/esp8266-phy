
#ifndef GLUE_STUB_H
#define GLUE_STUB_H

#include "osapi.h"

struct netif_glue
{
};

extern struct netif_glue netif_glue_global;

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

err_glue_t glue_oldcall_dhcp_start (void);

void old2glue_oldnetif_updated (uint32_t ip, uint32_t mask, uint32_t gw, uint16_t flags, uint8_t hwlen, const uint8_t* hw, void* state);

#endif // GLUE_STUB_H
