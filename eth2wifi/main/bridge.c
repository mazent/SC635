#include "bsp.h"
#include "stampa.h"
#include "conf.h"
#include "tcpsrv.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#include "esp_event_loop.h"
#include "esp_event.h"
#include "esp_log.h"

#include "esp_eth.h"

#include "esp_wifi.h"
#include "esp_wifi_internal.h"

#include "eth_phy/phy_lan8720.h"


#include "esp_wifi_internal.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/snmp.h"
#include "netif/etharp.h"
#include "lwip/ethip6.h"
#include "lwip/dhcp.h"


#define PIN_SMI_MDC   23
#define PIN_SMI_MDIO  18

static const char * TAG = "bridge";


typedef enum {
	DA_ETH,
	DA_WIFI,
	DA_BR,
	E_WIFI_CONN,
	E_WIFI_DISC,
	E_ETH_CONN,
	E_ETH_DISC,
	DHCPC_IP,
	ECO_MSG
} TIPO_MSG ;

typedef struct {
    void* buffer;
    uint16_t len;
    void* eb;
    TIPO_MSG tipo ;
} tcpip_adapter_eth_input_t;

static xQueueHandle eth_queue_handle;

static void invia_msg(TIPO_MSG t)
{
	tcpip_adapter_eth_input_t msg = {
		.tipo = t
	} ;

    if (xQueueSend(eth_queue_handle, &msg, 0) != pdTRUE) {
    	ESP_LOGE(TAG, "msg non inviato!!!") ;
    }
}

// ========= ECO ==============================================================

static osPoolId mp = NULL ;

static void eco_conn(const char * ip)
{
	ESP_LOGI(TAG, "eco connesso a %s", ip) ;
}

static void eco_msg(TCPSRV_MSG * pM)
{
	tcpip_adapter_eth_input_t msg = {
		.tipo = ECO_MSG,
		.buffer = pM
	} ;

    if (xQueueSend(eth_queue_handle, &msg, 0) != pdTRUE) {
    	ESP_LOGE(TAG, "eco: msg non inviato!!!") ;
    }
}

static void eco_scon(void)
{
	ESP_LOGI(TAG, "eco disconnesso") ;
}

static TCPSRV_CFG ecoCfg = {
	.porta = 7,

	.conn = eco_conn,
	.msg = eco_msg,
	.scon = eco_scon
} ;

static TCP_SRV * ecoSrv = NULL ;

static void inizia_eco(void)
{
	if (NULL == mp) {
		osPoolDef(mp, 100, TCPSRV_MSG) ;
		mp = osPoolCreate(osPool(mp)) ;
		assert(mp) ;
		ecoCfg.mp = mp ;
	}
	if (NULL == ecoSrv)
		ecoSrv = TCPSRV_beg(&ecoCfg) ;
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
    phy_rmii_smi_configure_pins(PIN_SMI_MDC, PIN_SMI_MDIO);
}

static esp_err_t tcpip_adapter_eth_input_sta_output(void* buffer, uint16_t len, void* eb)
{
	if (len > 0) {
	    tcpip_adapter_eth_input_t msg = {
	    	.tipo = DA_ETH
	    } ;

	    msg.buffer = malloc(len);
	    if (msg.buffer) {
	    	memcpy(msg.buffer, buffer, len) ;

		    msg.len = len;
		    msg.eb = eb;

		    if (xQueueSend(eth_queue_handle, &msg, 0) != pdTRUE) {
		    	ESP_LOGE(TAG, "eth non inviato!!!") ;
		    	free(msg.buffer) ;
		    }
	    }
	    else
	    	ESP_LOGE(TAG, "eth malloc!!!") ;
	}

	esp_eth_free_rx_buf(buffer) ;

    return ESP_OK;
}

static void initialise_ethernet(void)
{
    eth_config_t config = phy_lan8720_default_ethernet_config;

    /* Set the PHY address in the example configuration */
    config.phy_addr = 1;
    config.gpio_config = eth_gpio_config_rmii;
    config.tcpip_input = tcpip_adapter_eth_input_sta_output;

    esp_eth_init_internal(&config);
    esp_eth_enable();
}

// ========= WIFI =============================================================

static esp_err_t tcpip_adapter_wifi_input_eth_output(void* buffer, uint16_t len, void* eb)
{
	if (len > 0) {
	    tcpip_adapter_eth_input_t msg = {
	    	.tipo = DA_WIFI
	    } ;

	    msg.buffer = malloc(len);
	    if (msg.buffer) {
	    	memcpy(msg.buffer, buffer, len) ;

		    msg.len = len;
		    msg.eb = eb;

		    if (xQueueSend(eth_queue_handle, &msg, 0) != pdTRUE) {
		    	ESP_LOGE(TAG, "wifi non inviato!!!") ;
		    	free(msg.buffer) ;
		    }
	    }
	    else {
	    	ESP_LOGE(TAG, "wifi malloc!!!") ;
	    }
	}

	esp_wifi_internal_free_rx_buffer(eb);

    return ESP_OK;
}

static void initialise_wifi(void)
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init_internal(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    wifi_config_t wifi_config = {
        .ap = {
        	//.channel = 9,
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
//    ESP_ERROR_CHECK(ESP_OK != esp_wifi_set_bandwidth(ESP_IF_WIFI_AP, WIFI_BW_HT20)) ;

    esp_wifi_start() ;
}

// ========= BRIDGE ===========================================================

static bool ini = false ;
static struct netif br ;
static ip_addr_t br_addr ;

static void br_input(void *buffer, uint16_t len)
{
#if BRIDGE > 0
	struct pbuf *p;

	if (!ini)
		return ;

	if (buffer == NULL)
		return;

	if (ip_addr_cmp(&br.ip_addr, &ip_addr_any)) {
		//ESP_LOGI(TAG, "! br_input: ip nullo !") ;
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
#else
	p = pbuf_alloc(PBUF_RAW, len, PBUF_REF);
	if (p == NULL)
		return;

	p->payload = buffer;
	p->l2_owner = &br;
	p->l2_buf = buffer;
#endif

	/* full packet send to tcpip_thread to process */
	if (br.input(p, &br) != ERR_OK) {
		LWIP_DEBUGF(NETIF_DEBUG, ("br_input: IP input error\n"));
		p->l2_owner = NULL;
		pbuf_free(p);
	}
#endif
}

static err_t br_output(struct netif *netif, struct pbuf *p)
{
    tcpip_adapter_eth_input_t msg = {
    	.tipo = DA_BR,
    	.len = p->len
    } ;

    msg.buffer = malloc(p->len);
    if (msg.buffer) {
    	memcpy(msg.buffer, p->payload, p->len) ;

	    if (xQueueSend(eth_queue_handle, &msg, 0) != pdTRUE) {
	    	ESP_LOGE(TAG, "br out non inviato!!!") ;
	    	free(msg.buffer) ;
	    }
    }
    else
    	ESP_LOGE(TAG, "br out malloc!!!") ;

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
	MIB2_INIT_NETIF(netif, snmp_ifType_ethernet_csmacd, 100);

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
#if 0
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

#if BRIDGE_DHCP_CLN > 0

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

		// Avviso
		invia_msg(DHCPC_IP) ;

	} while (false) ;
}

#else

static void dhcps_cb(u8_t client_ip[4])
{
	ESP_LOGI(TAG, "dhcps_cb: %d.%d.%d.%d", client_ip[0], client_ip[1], client_ip[2], client_ip[3]) ;
}

#endif

#if BRIDGE == 0
static void br_iniz(void) {}
#else
static void br_iniz(void)
{
	if (!ini) {
		tcpip_init(NULL, NULL) ;
		ini = true ;
	}

	br_addr = ip_addr_any ;
	netif_add(&br, &ip_addr_any.u_addr.ip4, &ip_addr_any.u_addr.ip4, &ip_addr_any.u_addr.ip4, NULL, br_if_init, tcpip_input) ;

	netif_set_up(&br) ;
	netif_set_link_up(&br);
#if BRIDGE_DHCP_CLN > 0
	ESP_LOGI(TAG, "br_iniz dhcp client") ;
	dhcp_start(&br) ;
	dhcp_set_cb(&br, dhcp_cb);
#else
	ip4_addr_t br_ip, br_msk, br_gw ;

	IP4_ADDR(&br_gw, 0,0,0,0) ;

#if BRIDGE_DHCPS_AUTOIP == 0
    IP4_ADDR(&br_ip, 10, 1, 1, 1) ;
#else
    IP4_ADDR(&br_ip, 169, 254, 1, 1) ;
#endif
    // Comunque il dhcps fornisce questa maschera
    IP4_ADDR(&br_msk, 255,255,255,0) ;
    netif_set_addr(&br, &br_ip, &br_msk, &br_gw) ;

    dhcps_set_new_lease_cb(dhcps_cb) ;
    ESP_LOGI(TAG, "br_iniz dhcp server") ;
    dhcps_start(&br, br_ip) ;

    inizia_eco() ;
#endif
}
#endif

static void br_fine(void)
{
	netif_set_down(&br) ;
}

static esp_err_t event_handler(void* ctx, system_event_t* event)
{
    switch (event->event_id) {
//        case SYSTEM_EVENT_STA_START:
//            printf("SYSTEM_EVENT_STA_START\r\n");
//            break;
//
//        case SYSTEM_EVENT_STA_CONNECTED:
//            printf("SYSTEM_EVENT_STA_CONNECTED\r\n");
//            wifi_is_connected = true;
//
//			{
//				uint8_t mac[6] = {0} ;
//				esp_wifi_get_mac(WIFI_IF_STA, mac) ;
//				ESP_LOGI(TAG, "STA: "MACSTR"", MAC2STR(mac)) ;
//			}
//
//			esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_AP, tcpip_adapter_wifi_input_eth_output);
//            break;
//
//        case SYSTEM_EVENT_STA_GOT_IP:
//            printf("SYSTEM_EVENT_STA_GOT_IP\r\n");
//            break;
//
//        case SYSTEM_EVENT_STA_DISCONNECTED:
//            printf("SYSTEM_EVENT_STA_DISCONNECTED\r\n");
//            wifi_is_connected = false;
//            esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_AP, NULL);
//            esp_wifi_connect();
//            break;

		case SYSTEM_EVENT_AP_START:
			ESP_LOGI(TAG, "SYSTEM_EVENT_AP_START");

			{
				uint8_t mac[6] = {0} ;
				esp_wifi_get_mac(WIFI_IF_AP, mac) ;
				ESP_LOGI(TAG, "AP: "MACSTR"", MAC2STR(mac)) ;
			}
			break ;

        case SYSTEM_EVENT_AP_STACONNECTED:
            printf("SYSTEM_EVENT_AP_STACONNECTED\r\n");

            invia_msg(E_WIFI_CONN) ;
            break;

        case SYSTEM_EVENT_AP_STADISCONNECTED:
            printf("SYSTEM_EVENT_AP_STADISCONNECTED\r\n");

            invia_msg(E_WIFI_DISC) ;
            break;

        case SYSTEM_EVENT_ETH_CONNECTED:
            printf("SYSTEM_EVENT_ETH_CONNECTED\r\n");

            invia_msg(E_ETH_CONN) ;
            break;

        case SYSTEM_EVENT_ETH_DISCONNECTED:
            printf("SYSTEM_EVENT_ETH_DISCONNECTED\r\n");

            invia_msg(E_ETH_DISC) ;
            break;

        default:
            break;
    }

    return ESP_OK;
}

void BR_start(void)
{
	{
		uint8_t mac[6] = {0} ;

	    esp_efuse_mac_get_default(mac) ;

	    ESP_LOGI(TAG, "base MAC: %02X:%02X:%02X:%02X:%02X:%02X", MAC2STR(mac)) ;
	}

    eth_queue_handle = xQueueCreate(100, sizeof(tcpip_adapter_eth_input_t));

    esp_event_loop_init(event_handler, NULL);

    initialise_ethernet();

    initialise_wifi();

    br_iniz() ;

    // Bridge
    tcpip_adapter_eth_input_t msg;
    int wifi_stations = 0 ;
    bool ethernet_is_connected = false;

    while (true) {
    	if (xQueueReceive(eth_queue_handle, &msg, (portTickType)portMAX_DELAY) == pdTRUE) {
    		switch (msg.tipo) {
    		case DA_ETH:
    			stampa_eth(msg.buffer, msg.len - 4) ;

    			if (wifi_stations)
    				esp_wifi_internal_tx(ESP_IF_WIFI_AP, msg.buffer, msg.len - 4);

    			br_input(msg.buffer, msg.len - 4) ;
    			break ;
    		case DA_WIFI:
    			stampa_wifi(msg.buffer, msg.len) ;

    		    if (ethernet_is_connected)
    		        esp_eth_tx(msg.buffer, msg.len);

    		    br_input(msg.buffer, msg.len) ;
    		    break ;
    		case DA_BR:
				if (wifi_stations)
					esp_wifi_internal_tx(ESP_IF_WIFI_AP, msg.buffer, msg.len) ;

				if (ethernet_is_connected)
					esp_eth_tx(msg.buffer, msg.len) ;
    			break ;
			case E_WIFI_CONN:
				++wifi_stations ;
				if (1 == wifi_stations)
					esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_AP, tcpip_adapter_wifi_input_eth_output);
				break ;
			case E_WIFI_DISC:
				--wifi_stations ;
				if (0 == wifi_stations)
					esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_AP, NULL);
				break ;
			case E_ETH_CONN:
				ethernet_is_connected = true ;
				{
					uint8_t mac[6] = {0} ;
					esp_eth_get_mac(mac) ;
					ESP_LOGI(TAG, "ETH: "MACSTR"", MAC2STR(mac)) ;

					stampa_registri() ;
				}
				break ;
			case E_ETH_DISC:
				ethernet_is_connected = false ;
				break ;
			case DHCPC_IP:
				// Il dhcp client ha ottenuto l'indirizzo
#if 1
				{
					char ip[20], msk[20], gw[20] ;

					strcpy(ip, ipaddr_ntoa(br.ip_addr)) ;
					strcpy(msk, ipaddr_ntoa(br.netmask)) ;
					strcpy(gw, ipaddr_ntoa(br.gw)) ;

					ESP_LOGI(TAG, "dhcp ip %s %s %s", ip, msk, gw) ;
				}
#endif
				inizia_eco() ;
				break ;
			case ECO_MSG: {
					TCPSRV_MSG * pM = msg.buffer ;
					if ( !TCPSRV_tx(ecoSrv, pM->mem, pM->dim) )
						ESP_LOGE(TAG, "TCPSRV_tx") ;
					CHECK_IT(osOK == osPoolFree(ecoCfg.mp, pM)) ;
					msg.buffer = NULL ;
				}
				break ;
    		}

    		if (msg.buffer)
    			free(msg.buffer) ;
    	}
    }
}
