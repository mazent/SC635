#include "rete.h"

#include "eth_phy/phy_lan8720.h"
#include "esp_eth.h"
#include "esp_wifi.h"
#include "esp_wifi_internal.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/snmp.h"
#include "netif/etharp.h"
#include "lwip/ethip6.h"
#include "lwip/dhcp.h"

static const char * TAG = "rete";

void stampa_eth(const char * t, const uint8_t * p, int dim)
{
	if (NULL == t)
		t = "" ;

#if 0
	ETH_FRAME * pF = (ETH_FRAME *) p ;
	const char * tipo = NULL ;
	uint16_t et = gira(pF->type) ;

	switch (et) {
	case 0x0800:
		tipo = "IPv4" ;
		break ;
	case 0x86DD:
		tipo = "IPv6" ;
		break ;
	case 0x0806:
		tipo = "ARP" ;
		break ;
	default: {
			static char xxx[20] ;
			sprintf(xxx, "? %04X ?", et) ;
			tipo = xxx ;
		}
		break ;
	}

	ESP_LOGI(TAG, "%s: [%d] %02X:%02X:%02X:%02X:%02X:%02X -> %02X:%02X:%02X:%02X:%02X:%02X %s",
			t, dim, MAC2STR(pF->srg), MAC2STR(pF->dst), tipo) ;
#endif
}

// ========= WIFI =============================================================

static esp_err_t wifi_tcpip_input(void* buffer, uint16_t len, void* eb)
{
	if (len > 0) {
		UN_PKT * pP = pkt_malloc(len) ;
		if (pP) {
			pP->tipo = DA_WIFI ;
			pP->len = len ;
			memcpy(pP->msg, buffer, len) ;
			if (osOK != osMessagePut(comes, (uint32_t) pP, 0)) {
				ESP_LOGE(TAG, "wifi non inviato!!!") ;
				pkt_free(pP) ;
			}
		}
	    else
	    	ESP_LOGE(TAG, "wifi malloc!!!") ;
	}
	else
		ESP_LOGE(TAG, "wifi len %d", len) ;

	esp_wifi_internal_free_rx_buffer(eb);

    return ESP_OK;
}

void ap_iniz(void)
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init_internal(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    wifi_config_t wifi_config = {
        .ap = {
        	.channel = 9,
            .max_connection = 1,
            .authmode = WIFI_AUTH_OPEN
        }
    } ;
    {
        uint8_t mac[6] = { 0 } ;
        char * ssid = (char *) wifi_config.ap.ssid ;

        esp_efuse_mac_get_default(mac) ;

        sprintf(ssid, "SC635_%02X%02X%02X", mac[3], mac[4], mac[5]) ;
        wifi_config.ap.ssid_len = strlen(ssid) ;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
#if CONFIG_MEZZA_BANDA
    ESP_ERROR_CHECK(ESP_OK != esp_wifi_set_bandwidth(ESP_IF_WIFI_AP, WIFI_BW_HT20)) ;
#endif

    esp_wifi_start() ;
}

void ap_attivo(bool si)
{
	if (si)
		esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_AP, wifi_tcpip_input) ;
	else
		esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_AP, NULL);
}

// ========= ETHERNET =========================================================

static void eth_gpio_config_rmii(void)
{
    // RMII data pins are fixed:
    // TXD0 = GPIO19
    // TXD1 = GPIO22
    // TX_EN = GPIO21
    // RXD0 = GPIO25
    // RXD1 = GPIO26
    // CLK == GPIO0
    phy_rmii_configure_data_interface_pins();
    // MDC is GPIO 23, MDIO is GPIO 18
    phy_rmii_smi_configure_pins(23, 18) ;
}

static esp_err_t eth_tcpip_input(void* buffer, uint16_t len, void* eb)
{
	if (len > 0) {
		UN_PKT * pP = pkt_malloc(len) ;
		if (pP) {
			pP->tipo = DA_ETH ;
			pP->len = len ;
			memcpy(pP->msg, buffer, len) ;
			if (osOK != osMessagePut(comes, (uint32_t) pP, 0)) {
				ESP_LOGE(TAG, "eth non inviato!!!") ;
				pkt_free(pP) ;
			}
		}
	    else
	    	ESP_LOGE(TAG, "eth malloc!!!") ;
	}
	else
		ESP_LOGE(TAG, "eth len %d", len) ;

	esp_eth_free_rx_buf(buffer) ;

    return ESP_OK;
}

void eth_iniz(void)
{
    eth_config_t config = phy_lan8720_default_ethernet_config ;

    config.phy_addr = 1;
    config.gpio_config = eth_gpio_config_rmii ;
    config.tcpip_input = eth_tcpip_input ;

    esp_eth_init_internal(&config);
    esp_eth_enable();
}

// ========= BRIDGE ===========================================================

static bool ini = false ;
static struct netif br ;
static ip_addr_t br_addr ;

void br_input(void *buffer, uint16_t len)
{
	struct pbuf *p;

	if (!ini)
		return ;

	if (buffer == NULL)
		return;

	if (ip_addr_cmp(&br.ip_addr, &ip_addr_any)) {
		ESP_LOGI(TAG, "! br_input: ip nullo !") ;
		return ;
	}

	//stampa_eth("??? -> BR", buffer, len) ;


#ifdef CONFIG_EMAC_L2_TO_L3_RX_BUF_MODE
	p = pbuf_alloc(PBUF_RAW, len, PBUF_RAM);
	if (p == NULL) {
		return;
	}
	p->l2_owner = NULL;
	memcpy(p->payload, buffer, len);

	/* full packet send to tcpip_thread to process */
	if (br->input(p, &br) != ERR_OK) {
		LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
		pbuf_free(p);
	}

#else
	p = pbuf_alloc(PBUF_RAW, len, PBUF_REF);
	if (p == NULL)
		return;

	p->payload = buffer;
	p->l2_owner = &br;
	p->l2_buf = buffer;

	/* full packet send to tcpip_thread to process */
	if (br.input(p, &br) != ERR_OK) {
		LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
		p->l2_owner = NULL;
		pbuf_free(p);
	}
#endif
}

static err_t br_output(struct netif *netif, struct pbuf *p)
{
	int conta = 0 ;

	if (ap_tx()) {
		esp_wifi_internal_tx(ESP_IF_WIFI_AP, p->payload, p->len) ;
		++conta ;
	}

	if (eth_tx()) {
		esp_eth_tx(p->payload, p->len) ;
		++conta ;
	}

	if (conta) {
		//stampa_eth("br_output", p->payload, p->len) ;
	}

	return ERR_OK;
}


static err_t br_if_init(struct netif *netif)
{
//#if LWIP_NETIF_HOSTNAME
//	/* Initialize interface hostname */
//	netif->hostname = "lwip";
//#endif /* LWIP_NETIF_HOSTNAME */

	/*
	 * Initialize the snmp variables and counters inside the struct netif.
	 * The last argument should be replaced with your link speed, in units
	 * of bits per second.
	 */
	MIB2_INIT_NETIF(netif, snmp_ifType_ethernet_csmacd, 100000000);

	netif->name[0] = 'b' ;
	netif->name[1] = 'r' ;
	netif->num = 0 ;

	/* We directly use etharp_output() here to save a function call.
	 * You can instead declare your own function an call etharp_output()
	 * from it if you have to do some checks before sending (e.g. if link
	 * is available...) */
	netif->output = etharp_output;
#if LWIP_IPV6
	netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */
	netif->linkoutput = br_output;

	/* set MAC hardware address length */
	netif->hwaddr_len = ETHARP_HWADDR_LEN;

	/* set MAC hardware address */
#if 1
	netif->hwaddr[0] = 0xAA ;
	netif->hwaddr[1] = 0xBB ;
	netif->hwaddr[2] = 0xCC ;
	netif->hwaddr[3] = 0xDD ;
	netif->hwaddr[4] = 0xEE ;
	netif->hwaddr[5] = 0xFF ;
#else
	// https://esp-idf.readthedocs.io/en/latest/api-reference/system/system.html#mac-address
	esp_efuse_mac_get_default(netif->hwaddr) ;
	// base + 0 = STA
	// base + 1 = AP
	// base + 2 = BT
	// base + 3 = ETH
	netif->hwaddr[5] += 2 ;
#endif
	/* maximum transfer unit */
	netif->mtu = 1500;

	/* device capabilities */
	/* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP ;

	return ERR_OK;
}

static void dhcp_cb(struct netif * nif)
{
	do {
		if (ip_addr_cmp(&nif->ip_addr, &ip_addr_any)) {
			// Non ho ancora un indirizzo
			ESP_LOGE(TAG, "br dhcp_cb") ;
			break ;
		}

		if (ip_addr_cmp(&nif->ip_addr, &br_addr)) {
			// Stesso di prima
			ESP_LOGE(TAG, "br dhcp_cb stesso ip") ;
			break ;
		}

		// Nuovo indirizzo!
		br_addr = nif->ip_addr ;
#if 1
		{
			char ip[20], msk[20], gw[20] ;

			strcpy(ip, ipaddr_ntoa(&nif->ip_addr)) ;
			strcpy(msk, ipaddr_ntoa(&nif->netmask)) ;
			strcpy(gw, ipaddr_ntoa(&nif->gw)) ;

			ESP_LOGI(TAG, "BR dhcp_cb %s %s %s", ip, msk, gw) ;
		}
#endif

		// Avviso
		CHECK_IT(osOK == osMessagePut(comes, MSG_BRIP, 0)) ;

	} while (false) ;
}

static void dhcps_cb(u8_t client_ip[4])
{
	ESP_LOGI(TAG, "dhcps_cb: %d.%d.%d.%d", client_ip[0], client_ip[1], client_ip[2], client_ip[3]) ;
}

void br_iniz(void)
{
	if (!ini) {
		tcpip_init(NULL, NULL) ;
		ini = true ;
	}

	br_addr = ip_addr_any ;
	netif_add(&br, &ip_addr_any.u_addr.ip4, &ip_addr_any.u_addr.ip4, &ip_addr_any.u_addr.ip4, NULL, br_if_init, tcpip_input) ;

	netif_set_up(&br) ;
	netif_set_link_up(&br);
#if 0
	dhcp_start(&br) ;
	dhcp_set_cb(&br, dhcp_cb);
#else
	ip4_addr_t br_ip, br_msk, br_gw ;

	IP4_ADDR(&br_gw, 0,0,0,0) ;

#if 1
    IP4_ADDR(&br_ip, 10,10,10,1) ;
#else
    IP4_ADDR(&br_ip, 169,254,10,1) ;
#endif
    // Comunque il dhcps fornisce questa maschera
    IP4_ADDR(&br_msk, 255,255,255,0) ;
    netif_set_addr(&br, &br_ip, &br_msk, &br_gw) ;

    dhcps_set_new_lease_cb(dhcps_cb) ;
    dhcps_start(&br, br_ip) ;
#endif
}

void br_fine(void)
{
	netif_set_down(&br) ;
}

bool br_valido(void)
{
	bool valido = true ;
	if (ip_addr_cmp(&br_addr, &ip_addr_any))
		valido = false ;

	return valido ;
}
