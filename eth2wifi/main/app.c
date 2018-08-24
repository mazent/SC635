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

static const char * TAG = "bridge";

//typedef struct {
//    void * buffer;
//    uint16_t len;
//    void * eb;
//} PKT ;
//
//osMailQDef(pkt, 10, PKT) ;

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

typedef struct {
    void* buffer;
    uint16_t len;
    void* eb;
} tcpip_adapter_eth_input_t;

static xQueueHandle eth_queue_handle;
static bool wifi_is_connected = false;
static bool ethernet_is_connected = false;


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

static esp_err_t tcpip_adapter_eth_input_sta_output(void* buffer, uint16_t len, void* eb)
{
    tcpip_adapter_eth_input_t msg;

    msg.buffer = buffer;
    msg.len = len;
    msg.eb = eb;

    if (xQueueSend(eth_queue_handle, &msg, portMAX_DELAY) != pdTRUE) {
        return ESP_FAIL;
    }

    return ESP_OK;
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

static esp_err_t tcpip_adapter_wifi_input_eth_output(void* buffer, uint16_t len, void* eb)
{
#if 0
	ETH_FRAME * pF = (ETH_FRAME *) buffer ;

	ESP_LOGI(TAG, "WiFi[%d] %02X:%02X:%02X:%02X:%02X:%02X -> %02X:%02X:%02X:%02X:%02X:%02X %04X",
			len, MAC2STR(pF->srg), MAC2STR(pF->dst), gira(pF->type)) ;
#endif

    if (ethernet_is_connected) {
        esp_eth_tx(buffer, len);
    }

    esp_wifi_internal_free_rx_buffer(eb);
    return ESP_OK;
}


static esp_err_t event_handler(void* ctx, system_event_t* event)
{
    switch (event->event_id) {
        case SYSTEM_EVENT_STA_START:
            printf("SYSTEM_EVENT_STA_START\r\n");
            break;

        case SYSTEM_EVENT_STA_CONNECTED:
            printf("SYSTEM_EVENT_STA_CONNECTED\r\n");
            wifi_is_connected = true;

			{			
				uint8_t mac[6] = {0} ;
				esp_wifi_get_mac(WIFI_IF_STA, mac) ;
				ESP_LOGI(TAG, "STA: "MACSTR"", MAC2STR(mac)) ;
			}

            //esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_STA, (wifi_rxcb_t)tcpip_adapter_sta_input_eth_output);
			esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_STA, tcpip_adapter_wifi_input_eth_output);
            break;

        case SYSTEM_EVENT_STA_GOT_IP:
            printf("SYSTEM_EVENT_STA_GOT_IP\r\n");
            break;

        case SYSTEM_EVENT_STA_DISCONNECTED:
            printf("SYSTEM_EVENT_STA_DISCONNECTED\r\n");
            wifi_is_connected = false;
            esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_STA, NULL);
            esp_wifi_connect();
            break;

        case SYSTEM_EVENT_AP_STACONNECTED:
            printf("SYSTEM_EVENT_AP_STACONNECTED\r\n");
            wifi_is_connected = true;

            //esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_AP, (wifi_rxcb_t)tcpip_adapter_ap_input_eth_output);
            esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_AP, tcpip_adapter_wifi_input_eth_output);
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
            wifi_is_connected = false;
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

			esp_wifi_start();
            break;

        case SYSTEM_EVENT_ETH_DISCONNECTED:
            printf("SYSTEM_EVENT_ETH_DISCONNECTED\r\n");

            ethernet_is_connected = false;

            esp_wifi_stop();
            break;

        default:
            break;
    }

    return ESP_OK;
}

void app_main()
{
	esp_log_level_set("*", ESP_LOG_INFO) ;
	
    ESP_ERROR_CHECK(nvs_flash_init());
	
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
    eth_queue_handle = xQueueCreate(CONFIG_DMA_RX_BUF_NUM, sizeof(tcpip_adapter_eth_input_t));

    esp_event_loop_init(event_handler, NULL);

    initialise_ethernet();

    initialise_wifi();

    // Bridge
    tcpip_adapter_eth_input_t msg;

    for (;;) {
        if (xQueueReceive(eth_queue_handle, &msg, (portTickType)portMAX_DELAY) == pdTRUE) {
            if (msg.len > 0) {
#if 0
            	ETH_FRAME * pF = (ETH_FRAME *) msg.buffer ;

            	ESP_LOGI(TAG, "ETH[%d] %02X:%02X:%02X:%02X:%02X:%02X -> %02X:%02X:%02X:%02X:%02X:%02X %04X",
            			msg.len, MAC2STR(pF->srg), MAC2STR(pF->dst), gira(pF->type)) ;
#endif
                if (wifi_is_connected)
                    esp_wifi_internal_tx(ESP_IF_WIFI_XXX, msg.buffer, msg.len - 4);
            }

            esp_eth_free_rx_buf(msg.buffer);
        }
    }
}
