#include "bsp.h"

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/ethip6.h"
#include "lwip/tcpip.h"
#include "netif/etharp.h"
#include "netif/ppp/pppoe.h"

static const char * TAG = "finta" ;

#define IPADDR4_INIT_BYTES(a,b,c,d) \
        ((u32_t)((d) & 0xff) << 24) | \
        ((u32_t)((c) & 0xff) << 16) | \
        ((u32_t)((b) & 0xff) << 8)  | \
         (u32_t)((a) & 0xff)

static const ip4_addr_t ipaddr = {
	IPADDR4_INIT_BYTES(169, 254, 1, 1)
} ;
static const ip4_addr_t netmask = {
	IP_CLASSB_NET
} ;
static const ip4_addr_t gway = {
	IPADDR_NONE
} ;

//  Called when a raw link packet is ready to be transmitted. This function should not add any more headers

static err_t finta_linkoutput_fn(struct netif *netif, struct pbuf *p)
{
	ESP_LOGI(TAG, "linkoutput") ;

	return ERR_OK ;
}

// Called by ip_output when a packet is ready for transmission.
// Any link headers will be added here.
// This function should call the myif_link_output function when the packet is ready.
// You must set netif->output to the address of this function.
// If your driver supports ARP, you can simply set netif->output to etharp_output
static err_t finta_output(struct netif *netif, struct pbuf *p, ip_addr_t *ipaddr)
{
	return ERR_OK ;
}

// When you have received a packet (do not remove link headers!), you should pass the packet to my_netif->input,
// which should have been set by netif_add to the appropriate function
// With NO_SYS=0, this must be set to tcpip_input for all netif types (whether ethernet, PPP, slipif, etc.)
// With NO_SYS=1,
//     For ethernet netifs, this must be set to ethernet_input (pass the pbuf including link headers)
//     For non-ethernet netifs, this must be set to ip_input (pass the pbuf without link headers, p->payload pointing to the IP header)
// -----

static err_t finta_init_fn(struct netif *netif)
{
	netif->state = NULL ;
#if 1
	netif->hwaddr_len = 6 ;
	netif->hwaddr[0] = 0x00 ;
	netif->hwaddr[1] = 0x11 ;
	netif->hwaddr[2] = 0x22 ;
	netif->hwaddr[3] = 0x33 ;
	netif->hwaddr[4] = 0x44 ;
	netif->hwaddr[5] = 0x00 ;
#else
	netif->hwaddr_len = 1 ;
	netif->hwaddr[0] = 0xAA ;
#endif
	netif->mtu = 1000 ;

	netif->name[0] = 'x' ;
	netif->name[1] = 'x' ;
	netif->num = 0 ;

	//netif->flags |= NETIF_FLAG_POINTTOPOINT | NETIF_FLAG_LINK_UP ;

	netif->output = etharp_output;
	netif->linkoutput = finta_linkoutput_fn ;

	return ERR_OK ;
}

bool FIF_beg(void)
{
	// the user allocates space for a new struct netif (but does not initialize any part of it) and calls netif_add:
	static struct netif u_netif ;

	if ( NULL != netif_add(&u_netif, &ipaddr, &netmask, &gway, NULL, finta_init_fn, tcpip_input) ) {
#if 0
		netif_set_up(&u_netif) ;
#elif 1
		netifapi_autoip_start(&u_netif) ;
#else
		netifapi_dhcp_start(&u_netif) ;
#endif
		return true ;
	}
	else
		return false ;
}
