#ifndef CONF_H_
#define CONF_H_

/*
 * Mette a posto le macro di component.mk
 */

// attiva la scheda bridge
#ifndef BRIDGE
	// no
#	define BRIDGE		0
#endif

// la scheda bridge puo' essere dhcp client o server
#ifndef BRIDGE_DHCP_CLN
	// predefinito client
#	define BRIDGE_DHCP_CLN		1
#endif

// quando ethernet e' connessa, stampa i registri dell'8720
#ifndef STAMPA_REGISTRI
#	define STAMPA_REGISTRI		0
#endif

// stampa il pacchetto wifi
#ifndef STAMPA_WIFI
#	define STAMPA_WIFI		0
#endif

// stampa il pacchetto ethernet
#ifndef STAMPA_ETH
#	define STAMPA_ETH		0
#endif


#endif
