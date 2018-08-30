#if 0
/* Ethernet to WiFi data forwarding Example

   For other examples please check:
   https://github.com/espressif/esp-iot-solution/tree/master/examples

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
 */
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_event_loop.h"
#include "esp_event.h"
#include "esp_log.h"

#include "esp_eth.h"

#include "esp_wifi.h"
#include "esp_wifi_internal.h"

#include "nvs_flash.h"

#ifdef CONFIG_PHY_LAN8720
#include "eth_phy/phy_lan8720.h"
#define DEFAULT_ETHERNET_PHY_CONFIG phy_lan8720_default_ethernet_config
#endif
#ifdef CONFIG_PHY_TLK110
#include "eth_phy/phy_tlk110.h"
#define DEFAULT_ETHERNET_PHY_CONFIG phy_tlk110_default_ethernet_config
#endif

#ifdef CONFIG_ETH_TO_STATION_MODE
#	define ESP_IF_WIFI_XXX	ESP_IF_WIFI_STA
#else
#	define ESP_IF_WIFI_XXX 	ESP_IF_WIFI_AP
#endif


#include "mobd.h"
#include "phy.h"

#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/snmp.h"
#include "netif/etharp.h"
#include "lwip/ethip6.h"
#include "lwip/dhcp.h"

static const char * TAG = "bridge";

static void stampa_registri(void)
{
#if 0
    //phy_lan8720_dump_registers() ;
	uint16_t val ;

    ESP_LOGI(TAG, "LAN8720 Registers:");

    val = esp_eth_smi_read(0) ;
    ESP_LOGI(TAG, "    BCR    0x%04X", val);
    ESP_LOGI(TAG, "           %s", val & (1 << 14) ? "loopback" : "normal");
    if (val & (1 << 12))
    	ESP_LOGI(TAG, "           auto-negotiate");
    else {
    	ESP_LOGI(TAG, "           %s", val & (1 << 13) ? "100Mbps" : "10Mbps");
    	ESP_LOGI(TAG, "           %s", val & (1 << 8) ? "full duplex" : "half duplex");
    }
    ESP_LOGI(TAG, "           %s", val & (1 << 10) ? "electrical isolation of PHY from the RMII" : "NO electrical isolation of PHY from the RMII");

    val = esp_eth_smi_read(1) ;
    ESP_LOGI(TAG, "    BSR    0x%04X", val) ;
    ESP_LOGI(TAG, "           T4 able: %s", val & (1 << 15) ? "SI" : "no") ;
    ESP_LOGI(TAG, "           100BASE-TX Full Duplex: %s", val & (1 << 14) ? "SI" : "no") ;
    ESP_LOGI(TAG, "           100BASE-TX Half Duplex: %s", val & (1 << 13) ? "SI" : "no") ;
    ESP_LOGI(TAG, "           10BASE-T Full Duplex : %s", val & (1 << 12) ? "SI" : "no") ;
    ESP_LOGI(TAG, "           10BASE-T Half Duplex : %s", val & (1 << 11) ? "SI" : "no") ;
    ESP_LOGI(TAG, "           100BASE-T2 Full Duplex : %s", val & (1 << 10) ? "SI" : "no") ;
    ESP_LOGI(TAG, "           100BASE-T2 Half Duplex : %s", val & (1 <<  9) ? "SI" : "no") ;
    ESP_LOGI(TAG, "           Auto-Negotiate Complete : %s", val & (1 <<  5) ? "SI" : "no") ;
    ESP_LOGI(TAG, "           Remote Fault : %s", val & (1 <<  4) ? "SI" : "no") ;
    ESP_LOGI(TAG, "           Auto-Negotiate Ability : %s", val & (1 <<  3) ? "SI" : "no") ;
    ESP_LOGI(TAG, "           Link Status : %s", val & (1 <<  2) ? "UP" : "down") ;
    ESP_LOGI(TAG, "           Jabber Detect : %s", val & (1 <<  1) ? "SI" : "no") ;

    ESP_LOGI(TAG, "    PHY1   0x%04X", esp_eth_smi_read(2)) ;
    ESP_LOGI(TAG, "    PHY2   0x%04X", esp_eth_smi_read(3)) ;
    ESP_LOGI(TAG, "    ANAR   0x%04X", esp_eth_smi_read(4));
    ESP_LOGI(TAG, "    ANLPAR 0x%04X", esp_eth_smi_read(5));
    ESP_LOGI(TAG, "    ANER   0x%04X", esp_eth_smi_read(6));
    ESP_LOGI(TAG, "    MCSR   0x%04X", esp_eth_smi_read(17));
    ESP_LOGI(TAG, "    SM     0x%04X", esp_eth_smi_read(18));
    ESP_LOGI(TAG, "    SECR   0x%04X", esp_eth_smi_read(26));
    ESP_LOGI(TAG, "    CSIR   0x%04X", esp_eth_smi_read(27));
    ESP_LOGI(TAG, "    ISR    0x%04X", esp_eth_smi_read(29));
    ESP_LOGI(TAG, "    IMR    0x%04X", esp_eth_smi_read(30));

    val = esp_eth_smi_read(31) ;
    ESP_LOGI(TAG, "    PSCSR  0x%04X", val);
    ESP_LOGI(TAG, "           %s", val & (1 << 12) ? "Auto-negotiation is done" : "Auto-negotiation is not done or disabled (or not active)") ;
    val >>= 2 ;
    val &= 0x7 ;
    switch (val) {
    case 1:
    	ESP_LOGI(TAG, "           10BASE-T half-duplex") ;
    	break ;
    case 5:
    	ESP_LOGI(TAG, "           10BASE-T full-duplex") ;
    	break ;
    case 2:
    	ESP_LOGI(TAG, "           100BASE-TX half-duplex") ;
    	break ;
    case 6:
    	ESP_LOGI(TAG, "           100BASE-TX full-duplex") ;
    	break ;
    default:
    	ESP_LOGE(TAG, "           ? %02X ?", val) ;
    	break ;
    }
#endif
}


#define PIN_PHY_POWER CONFIG_PHY_POWER_PIN
#define PIN_SMI_MDC   CONFIG_PHY_SMI_MDC_PIN
#define PIN_SMI_MDIO  CONFIG_PHY_SMI_MDIO_PIN

// AP: numero di stazioni connesse
// STA: 0/1
static int wifi_is_connected = 0 ;

// Vera se c'e' attivita': non so quanti sono ma ne esiste almeno uno
static bool ethernet_is_connected = false ;

#define DIM_PKT		1600
#define NUM_PKT		30

typedef struct {
    uint8_t msg[DIM_PKT] ;
    uint16_t len;

    enum {
    	DA_ETH,
    	DA_WIFI,
		DA_BR
    } tipo ;
} UN_PKT ;

osMailQDef(pkt, NUM_PKT, UN_PKT) ;
static osMailQId pkt ;


#ifdef CONFIG_PHY_USE_POWER_PIN
/* This replaces the default PHY power on/off function with one that
   also uses a GPIO for power on/off.

   If this GPIO is not connected on your device (and PHY is always powered), you can use the default PHY-specific power
   on/off function rather than overriding with this one.
 */
static void phy_device_power_enable_via_gpio(bool enable)
{
    assert(DEFAULT_ETHERNET_PHY_CONFIG.phy_power_enable);

    if (!enable) {
        /* Do the PHY-specific power_enable(false) function before powering down */
        DEFAULT_ETHERNET_PHY_CONFIG.phy_power_enable(false);
    }

    gpio_pad_select_gpio(PIN_PHY_POWER);
    gpio_set_direction(PIN_PHY_POWER, GPIO_MODE_OUTPUT);

    if (enable == true) {
        gpio_set_level(PIN_PHY_POWER, 1);
        ESP_LOGD(TAG, "phy_device_power_enable(TRUE)");
    } else {
        gpio_set_level(PIN_PHY_POWER, 0);
        ESP_LOGD(TAG, "power_enable(FALSE)");
    }

    // Allow the power up/down to take effect, min 300us
    vTaskDelay(1);

    if (enable) {
        /* Run the PHY-specific power on operations now the PHY has power */
        DEFAULT_ETHERNET_PHY_CONFIG.phy_power_enable(true);
    }
}
#endif

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


typedef struct {
	uint8_t dst[6] ;
	uint8_t srg[6] ;
	uint16_t type ;
} ETH_FRAME ;

static uint16_t gira(uint16_t val)
{
	union {
		uint16_t x ;
		uint8_t b[2] ;
	} u ;
	uint8_t tmp ;

	u.x = val ;
	tmp = u.b[0] ;
	u.b[0] = u.b[1] ;
	u.b[1] = tmp ;

	return u.x ;
}


static esp_err_t tcpip_adapter_eth_input_sta_output(void* buffer, uint16_t len, void* eb)
{
	if (len > 0) {
		assert(len < DIM_PKT) ;
		UN_PKT * pP = (UN_PKT *) osMailAlloc(pkt, 0) ;
		if (pP) {
			pP->tipo = DA_ETH ;
			pP->len = len ;
			memcpy(pP->msg, buffer, len) ;
			if (osOK != osMailPut(pkt, pP)) {
				ESP_LOGE(TAG, "eth non inviato!!!") ;
				CHECK_IT(osOK == osMailFree(pkt, pP)) ;
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

static esp_err_t tcpip_adapter_wifi_input_eth_output(void* buffer, uint16_t len, void* eb)
{
	if (len > 0) {
		assert(len < DIM_PKT) ;
		UN_PKT * pP = (UN_PKT *) osMailAlloc(pkt, 0) ;
		if (pP) {
			pP->tipo = DA_WIFI ;
			pP->len = len ;
			memcpy(pP->msg, buffer, len) ;
			if (osOK != osMailPut(pkt, pP)) {
				ESP_LOGE(TAG, "wifi non inviato!!!") ;
				CHECK_IT(osOK == osMailFree(pkt, pP)) ;
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

static void initialise_wifi(void)
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init_internal(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
#ifdef CONFIG_ETH_TO_STATION_MODE
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = CONFIG_DEMO_SSID,
            .password = CONFIG_DEMO_PASSWORD,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
#else
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = CONFIG_DEMO_SSID,
            .password = CONFIG_DEMO_PASSWORD,
            .ssid_len = strlen(CONFIG_DEMO_SSID),
            .max_connection = 1,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
#endif
    esp_wifi_start() ;
}

static void initialise_ethernet(void)
{
    eth_config_t config = DEFAULT_ETHERNET_PHY_CONFIG;

    /* Set the PHY address in the example configuration */
    config.phy_addr = CONFIG_PHY_ADDRESS;
    config.gpio_config = eth_gpio_config_rmii;
    config.tcpip_input = tcpip_adapter_eth_input_sta_output;

#ifdef CONFIG_PHY_USE_POWER_PIN
    /* Replace the default 'power enable' function with an example-specific
       one that toggles a power GPIO. */
    config.phy_power_enable = phy_device_power_enable_via_gpio;
#endif
    esp_eth_init_internal(&config);
    esp_eth_enable();
}

static esp_err_t event_handler(void* ctx, system_event_t* event)
{
    switch (event->event_id) {
        case SYSTEM_EVENT_STA_START:
            printf("SYSTEM_EVENT_STA_START\r\n");
            break;

        case SYSTEM_EVENT_STA_CONNECTED:
            printf("SYSTEM_EVENT_STA_CONNECTED\r\n");
            wifi_is_connected = 1 ;

			{			
				uint8_t mac[6] = {0} ;
				esp_wifi_get_mac(WIFI_IF_STA, mac) ;
				ESP_LOGI(TAG, "STA: "MACSTR"", MAC2STR(mac)) ;
			}

			esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_STA, tcpip_adapter_wifi_input_eth_output);
            break;

        case SYSTEM_EVENT_STA_GOT_IP:
            printf("SYSTEM_EVENT_STA_GOT_IP\r\n");
            break;

        case SYSTEM_EVENT_STA_DISCONNECTED:
            printf("SYSTEM_EVENT_STA_DISCONNECTED\r\n");
            wifi_is_connected = 0 ;
            esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_STA, NULL);
            esp_wifi_connect();
            break;

        case SYSTEM_EVENT_AP_STACONNECTED:
            printf("SYSTEM_EVENT_AP_STACONNECTED\r\n");
            if (0 == wifi_is_connected)
            	esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_AP, tcpip_adapter_wifi_input_eth_output);

            ++wifi_is_connected ;
            break;
			
		case SYSTEM_EVENT_AP_START:                 
			ESP_LOGI(TAG, "SYSTEM_EVENT_AP_START");

			{			
				uint8_t mac[6] = {0} ;
				esp_wifi_get_mac(WIFI_IF_AP, mac) ;
				ESP_LOGI(TAG, "AP: "MACSTR"", MAC2STR(mac)) ;
			}
			break ;
			
        case SYSTEM_EVENT_AP_STADISCONNECTED:
            printf("SYSTEM_EVENT_AP_STADISCONNECTED\r\n");
            --wifi_is_connected ;
            if (0 == wifi_is_connected)
            	esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_AP, NULL);
            break;

        case SYSTEM_EVENT_ETH_CONNECTED:
            printf("SYSTEM_EVENT_ETH_CONNECTED\r\n");
            ethernet_is_connected = true;
			{			
				uint8_t mac[6] = {0} ;
				esp_eth_get_mac(mac) ;
				ESP_LOGI(TAG, "ETH: "MACSTR"", MAC2STR(mac)) ;

				stampa_registri() ;
			}
            break;

        case SYSTEM_EVENT_ETH_DISCONNECTED:
            printf("SYSTEM_EVENT_ETH_DISCONNECTED\r\n");

            ethernet_is_connected = false;
            break;

        default:
            break;
    }

    return ESP_OK;
}

static struct netif br ;

static void br_input(void *buffer, uint16_t len)
{
	struct pbuf *p;

#if 0
	ETH_FRAME * pF = (ETH_FRAME *) buffer ;

	ESP_LOGI(TAG, "BR in [%d] %02X:%02X:%02X:%02X:%02X:%02X -> %02X:%02X:%02X:%02X:%02X:%02X %04X",
			len, MAC2STR(pF->srg), MAC2STR(pF->dst), gira(pF->type)) ;
#endif

//	if(buffer== NULL || !netif_is_up(&br))
//		return;

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
#if 0
	ETH_FRAME * pF = (ETH_FRAME *) p->payload ;

	ESP_LOGI(TAG, "BR out [%d] %02X:%02X:%02X:%02X:%02X:%02X -> %02X:%02X:%02X:%02X:%02X:%02X %04X",
			p->len, MAC2STR(pF->srg), MAC2STR(pF->dst), gira(pF->type)) ;
#endif

	if (wifi_is_connected)
		esp_wifi_internal_tx(ESP_IF_WIFI_XXX, p->payload, p->len - 4) ;

	if (ethernet_is_connected)
		esp_eth_tx(p->payload, p->len) ;

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
	netif->hwaddr[5] += 1 ;
#endif
	/* maximum transfer unit */
	netif->mtu = 1500;

	/* device capabilities */
	/* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP ;

	return ERR_OK;
}

static ip_addr_t br_addr ;

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
		UN_PKT * pP = (UN_PKT *) osMailAlloc(pkt, 0) ;
		if (pP) {
			ip_addr_t * pi = (ip_addr_t *) pP->msg ;

			pP->tipo = DA_BR ;

			*pi = nif->ip_addr ;
			++pi ;
			*pi = nif->netmask ;
			++pi ;
			*pi = nif->gw ;

			if (osOK != osMailPut(pkt, pP)) {
				ESP_LOGE(TAG, "br dhcp_cb non inviato!!!") ;
				CHECK_IT(osOK == osMailFree(pkt, pP)) ;
			}
		}
		else
			ESP_LOGE(TAG, "br dhcp_cb malloc!!!") ;

	} while (false) ;
}

static void br_iniz(void)
{
	br_addr = ip_addr_any ;
	netif_add(&br, &ip_addr_any.u_addr.ip4, &ip_addr_any.u_addr.ip4, &ip_addr_any.u_addr.ip4, NULL, br_if_init, tcpip_input) ;

	netif_set_up(&br) ;
	netif_set_link_up(&br);

	dhcp_start(&br) ;
	dhcp_set_cb(&br, dhcp_cb);
	//autoip_start(&br) ;
}

static void br_fine(void)
{
	netif_set_down(&br) ;
}

void app_main()
{
	esp_log_level_set("*", ESP_LOG_INFO) ;
	
    ESP_ERROR_CHECK(nvs_flash_init());

    tcpip_init(NULL, NULL) ;
	
	CHECK_IT( PHY_beg() ) ;

	CHECK_IT( MOBD_beg() ) ;
	// collego maschio obd a eth
	MOBD_mobd_eth(true) ;
	// collego eth al micro
	MOBD_eth_esp32(true) ;

	{
		uint8_t mac[6] = {0} ;

	    esp_efuse_mac_get_default(mac) ;

	    ESP_LOGI(TAG, "base MAC: %02X:%02X:%02X:%02X:%02X:%02X", MAC2STR(mac)) ;
	}

	pkt = osMailCreate(osMailQ(pkt), NULL) ;

    esp_event_loop_init(event_handler, NULL);

    initialise_ethernet();

    initialise_wifi();

    br_iniz() ;

    while (true) {
    	osEvent evn = osMailGet(pkt, osWaitForever) ;
    	assert(osEventMail == evn.status) ;

    	UN_PKT * pP = evn.value.p ;
    	switch (pP->tipo) {
    	case DA_BR: {
    			ip_addr_t * ip = (ip_addr_t *) pP->msg ;
//    			ip_addr_t * msk = ip + 1 ;
//    			ip_addr_t * gw = ip + 2 ;

        		ESP_LOGI(TAG, "BR indirizzo %s", ipaddr_ntoa(&br.ip_addr)) ;
        		// Non devo fermarlo!
        		//dhcp_stop(&br) ;
    		}
    		break ;
    	case DA_ETH: {
#if 0
				ETH_FRAME * pF = (ETH_FRAME *) pP->msg ;

				ESP_LOGI(TAG, "ETH[%d] %02X:%02X:%02X:%02X:%02X:%02X -> %02X:%02X:%02X:%02X:%02X:%02X %04X",
						pP->len, MAC2STR(pF->srg), MAC2STR(pF->dst), gira(pF->type)) ;
#endif
				if (wifi_is_connected)
					esp_wifi_internal_tx(ESP_IF_WIFI_XXX, pP->msg, pP->len - 4);

				br_input(pP->msg, pP->len) ;
    		}
    		break ;
    	case DA_WIFI: {
#if 0
				ETH_FRAME * pF = (ETH_FRAME *) pP->msg ;

				ESP_LOGI(TAG, "WiFi[%d] %02X:%02X:%02X:%02X:%02X:%02X -> %02X:%02X:%02X:%02X:%02X:%02X %04X",
						pP->len, MAC2STR(pF->srg), MAC2STR(pF->dst), gira(pF->type)) ;
#endif
				if (ethernet_is_connected)
					esp_eth_tx(pP->msg, pP->len);

				br_input(pP->msg, pP->len) ;
    		}
    		break ;
    	}

    	CHECK_IT(osOK == osMailFree(pkt, evn.value.p)) ;
    }
}

#endif
