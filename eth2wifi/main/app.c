/* Ethernet to WiFi data forwarding Example

   For other examples please check:
   https://github.com/espressif/esp-iot-solution/tree/master/examples

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
 */
#include <string.h>

#include "rete.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_event_loop.h"
#include "esp_event.h"
#include "esp_log.h"

#include "esp_eth.h"

#include "esp_wifi.h"
#include "esp_wifi_internal.h"

#include "nvs_flash.h"


#include "mobd.h"
#include "phy.h"

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


// AP: numero di stazioni connesse
// STA: 0/1
static int wifi_is_connected = 0 ;

// Vera se c'e' attivita': non so quanti sono ma ne esiste almeno uno
static bool ethernet_is_connected = false ;

bool eth_tx(void)
{
	return ethernet_is_connected ;
}

bool ap_tx(void)
{
	return wifi_is_connected > 0 ;
}


osMessageQDef(comes, 100, uint32_t) ;
osMessageQId comes = NULL ;

static esp_err_t event_handler(void* ctx, system_event_t* event)
{
    switch (event->event_id) {
//        case SYSTEM_EVENT_STA_START:
//            printf("SYSTEM_EVENT_STA_START\r\n");
//            break;
//
//        case SYSTEM_EVENT_STA_CONNECTED:
//            printf("SYSTEM_EVENT_STA_CONNECTED\r\n");
//            wifi_is_connected = 1 ;
//
//			{
//				uint8_t mac[6] = {0} ;
//				esp_wifi_get_mac(WIFI_IF_STA, mac) ;
//				ESP_LOGI(TAG, "STA: "MACSTR"", MAC2STR(mac)) ;
//			}
//
//			esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_STA, wifi_tcpip_input);
//            break;
//
//        case SYSTEM_EVENT_STA_GOT_IP:
//            printf("SYSTEM_EVENT_STA_GOT_IP\r\n");
//            break;
//
//        case SYSTEM_EVENT_STA_DISCONNECTED:
//            printf("SYSTEM_EVENT_STA_DISCONNECTED\r\n");
//            wifi_is_connected = 0 ;
//            esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_STA, NULL);
//            esp_wifi_connect();
//            break;

        case SYSTEM_EVENT_AP_STACONNECTED:
            printf("SYSTEM_EVENT_AP_STACONNECTED\r\n");
            if (0 == wifi_is_connected)
            	ap_attivo(true) ;

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
            	ap_attivo(false) ;
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

	// Scambio messaggi
	comes = osMessageCreate(osMessageQ(comes), NULL) ;
	assert(comes) ;

    esp_event_loop_init(event_handler, NULL);

    eth_iniz() ;

    ap_iniz() ;
#if 0
    br_iniz() ;
#endif
    while (true) {
    	osEvent event = osMessageGet(comes, osWaitForever) ;
    	assert(osEventMessage == event.status) ;

    	if (osEventMessage == event.status) {
    		switch (event.value.v) {
			case MSG_BRIP: {
//					bool valido = true ;
//					if (ip_addr_cmp(&br_addr, &ip_addr_any))
//						valido = false ;
//					(void) fsm_engine(fsm, E_IP, &valido) ;
				}
				break ;
			default: {
					UN_PKT * pP = event.value.p ;
					br_pkt(pP) ;
				}
				break ;
    		}
    	}
    }
}
