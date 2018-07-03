#include "bsp.h"
#include "driver/uart.h"

//#include "lwip/opt.h"
//#include "lwip/def.h"
//#include "lwip/mem.h"
//#include "lwip/pbuf.h"
//#include "lwip/stats.h"
//#include "lwip/snmp.h"
//#include "lwip/ethip6.h"
//#include "netif/etharp.h"
//#include "netif/ppp/pppoe.h"

static const ip4_addr_t ipaddr = {
	IPADDR4_INIT_BYTES(169, 254, 1, 1)
} ;
static const ip4_addr_t netmask = {
	IPADDR4_INIT_BYTES(255, 255, 0, 0)
} ;
static const ip4_addr_t gway = {
	IPADDR4_INIT_BYTES(0, 0, 0, 0)
} ;

//  Called when a raw link packet is ready to be transmitted. This function should not add any more headers

static err_t uart_linkoutput_fn(struct netif *netif, struct pbuf *p)
{
	return ERR_OK ;
}

// Called by ip_output when a packet is ready for transmission.
// Any link headers will be added here.
// This function should call the myif_link_output function when the packet is ready.
// You must set netif->output to the address of this function.
// If your driver supports ARP, you can simply set netif->output to etharp_output
static err_t uart_output(struct netif *netif, struct pbuf *p, ip_addr_t *ipaddr)
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

static err_t uart_init_fn(struct netif *netif)
{
	netif->state = NULL ;
#if 0
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

	netif->flags |= NETIF_FLAG_POINTTOPOINT | NETIF_FLAG_LINK_UP ;

	netif->output = etharp_output;
	netif->linkoutput = uart_linkoutput_fn ;

	return ERR_OK ;
}

bool UIF_beg(void)
{
	// the user allocates space for a new struct netif (but does not initialize any part of it) and calls netif_add:
	static struct netif u_netif ;

	if ( NULL != netif_add(&u_netif, &ipaddr, &netmask, &gway, NULL, uart_init_fn, tcpip_input) ) {
#if 1
		netif_set_up(&u_netif) ;
#elif 0
		netifapi_autoip_start(&u_netif) ;
#else
		netifapi_dhcp_start(&u_netif) ;
#endif
	}
	else
		return false ;
}