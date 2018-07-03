#include "prod.h"
#include "led.h"
#include "spc.h"
#include "tasto.h"
#include "ap.h"
#include "gestore.h"

#include "driver/gpio.h"

#include "esp_log.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"

extern bool UIF_beg(void) ;

//#define CMD_ECO		((SPC_CMD) 0x0000)
//
//void SPC_msg(SPC_CMD cmd, uint8_t * dati, int dim)
//{
//	switch (cmd) {
//	case CMD_ECO:
//		SPC_resp(cmd, dati, dim) ;
//		break ;
//	default:
//		SPC_unk(cmd) ;
//		break ;
//	}
//}
//
//bool pres = false ;
//bool rila = false ;
//
//void tasto(bool premuto)
//{
//	if (premuto)
//		pres = true ;
//	else
//		rila = true ;
//}

static const char *TAG = "mz";


static void gst_conn(const char * ip, uint16_t porta)
{
	UNUSED(ip) ;
	UNUSED(porta) ;
}

static void gst_msg(void * v, int d)
{
	// eco
	(void) GST_tx(v, d) ;
}

static void gst_scon(void)
{
}

static S_GST_CB gstcb = {
	.conn = gst_conn, 
	.msg = gst_msg,
	.scon = gst_scon
} ;

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
	UNUSED(ctx) ;

	switch (event->event_id) {
    case SYSTEM_EVENT_WIFI_READY:               /**< ESP32 WiFi ready */
    	ESP_LOGI(TAG, "SYSTEM_EVENT_WIFI_READY");
    	break ;
    case SYSTEM_EVENT_SCAN_DONE:                /**< ESP32 finish scanning AP */
    	ESP_LOGI(TAG, "SYSTEM_EVENT_SCAN_DONE");
    	break ;
    case SYSTEM_EVENT_STA_START:                /**< ESP32 station start */
		ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START");
		break;
    case SYSTEM_EVENT_STA_STOP:                 /**< ESP32 station stop */
    	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_STOP");
    	break ;
    case SYSTEM_EVENT_STA_CONNECTED:            /**< ESP32 station connected to AP */
    	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_CONNECTED");
    	break ;
    case SYSTEM_EVENT_STA_DISCONNECTED:         /**< ESP32 station disconnected from AP */
		ESP_LOGI(TAG, "SYSTEM_EVENT_STA_DISCONNECTED");
		break;
    case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:      /**< the auth mode of AP connected by ESP32 station changed */
    	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_AUTHMODE_CHANGE");
    	break ;
    case SYSTEM_EVENT_STA_GOT_IP:               /**< ESP32 station got IP from connected AP */
		ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");
		break ;
    case SYSTEM_EVENT_STA_LOST_IP:              /**< ESP32 station lost IP and the IP is reset to 0 */
    	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_LOST_IP");
    	break ;
    case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:       /**< ESP32 station wps succeeds in enrollee mode */
    	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_WPS_ER_SUCCESS");
    	break ;
    case SYSTEM_EVENT_STA_WPS_ER_FAILED:        /**< ESP32 station wps fails in enrollee mode */
    	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_WPS_ER_FAILED");
    	break ;
    case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:       /**< ESP32 station wps timeout in enrollee mode */
    	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_WPS_ER_TIMEOUT");
    	break ;
    case SYSTEM_EVENT_STA_WPS_ER_PIN:           /**< ESP32 station wps pin code in enrollee mode */
    	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_WPS_ER_PIN");
    	break ;
    case SYSTEM_EVENT_AP_START:                 /**< ESP32 soft-AP start */
    	ESP_LOGI(TAG, "SYSTEM_EVENT_AP_START");
    	AP_evn(AP_EVN_START, &event->event_info) ;
    	CHECK_IT( GST_beg(&gstcb) ) ;
    	break ;
    case SYSTEM_EVENT_AP_STOP:                  /**< ESP32 soft-AP stop */
    	ESP_LOGI(TAG, "SYSTEM_EVENT_AP_STOP");
    	AP_evn(AP_EVN_STOP, &event->event_info) ;
    	GST_end() ;
    	break ;
    case SYSTEM_EVENT_AP_STACONNECTED:          /**< a station connected to ESP32 soft-AP */
    	ESP_LOGI(TAG, "SYSTEM_EVENT_AP_STACONNECTED");
    	AP_evn(AP_EVN_STACONNECTED, &event->event_info) ;
    	break ;
    case SYSTEM_EVENT_AP_STADISCONNECTED:       /**< a station disconnected from ESP32 soft-AP */
    	ESP_LOGI(TAG, "SYSTEM_EVENT_AP_STADISCONNECTED");
    	AP_evn(AP_EVN_STADISCONNECTED, &event->event_info) ;
    	break ;
    case SYSTEM_EVENT_AP_STAIPASSIGNED:         /**< ESP32 soft-AP assign an IP to a connected station */
    	ESP_LOGI(TAG, "SYSTEM_EVENT_AP_STAIPASSIGNED");
    	AP_evn(AP_EVN_STAIPASSIGNED, &event->event_info) ;
    	break ;
    case SYSTEM_EVENT_AP_PROBEREQRECVED:        /**< Receive probe request packet in soft-AP interface */
    	ESP_LOGI(TAG, "SYSTEM_EVENT_AP_PROBEREQRECVED");
    	break ;
    case SYSTEM_EVENT_GOT_IP6:                  /**< ESP32 station or ap or ethernet interface v6IP addr is preferred */
    	ESP_LOGI(TAG, "SYSTEM_EVENT_GOT_IP6");
    	break ;
    case SYSTEM_EVENT_ETH_START:                /**< ESP32 ethernet start */
    	ESP_LOGI(TAG, "SYSTEM_EVENT_ETH_START");
    	break ;
    case SYSTEM_EVENT_ETH_STOP:                 /**< ESP32 ethernet stop */
    	ESP_LOGI(TAG, "SYSTEM_EVENT_ETH_STOP");
    	break ;
    case SYSTEM_EVENT_ETH_CONNECTED:            /**< ESP32 ethernet phy link up */
    	ESP_LOGI(TAG, "SYSTEM_EVENT_ETH_CONNECTED");
    	break ;
    case SYSTEM_EVENT_ETH_DISCONNECTED:         /**< ESP32 ethernet phy link down */
    	ESP_LOGI(TAG, "SYSTEM_EVENT_ETH_DISCONNECTED");
    	break ;
    case SYSTEM_EVENT_ETH_GOT_IP:               /**< ESP32 ethernet got IP from connected AP */
    	ESP_LOGI(TAG, "SYSTEM_EVENT_ETH_GOT_IP");
    	break ;

	default:
		ESP_LOGE(TAG, "? evento %d %p ?", event->event_id, &event->event_info) ;
		break;
	}

	return ESP_OK;
}

void app_main()
{
    esp_log_level_set("*", ESP_LOG_INFO) ;

	// questa la fanno sempre
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
		ESP_ERROR_CHECK( nvs_flash_erase() );
		ESP_ERROR_CHECK( nvs_flash_init() );
	}

    gpio_install_isr_service(0) ;

    tcpip_adapter_init();

    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

	ESP_LOGI("main", ">>>> app_main") ;
#if 1
	// ap
	S_AP sap = {
		.ssid = "SC635",
		.max_connection = 2,
		.auth = AUTH_OPEN
	} ;
	CHECK_IT( AP_beg(&sap) ) ;

	CHECK_IT( UIF_beg() ) ;

#elif 0
	CHECK_IT( TST_beg(tasto) ) ;

	while (true) {
		osDelay(500) ;
		if (pres) {
			ESP_LOGI("tasto", "PREMUTO") ;
			pres = false ;
		}
		if (rila) {
			ESP_LOGI("tasto", "MOLLATO") ;
			rila = false ;
		}
	}
#elif 0
	esp_log_level_set("*", ESP_LOG_NONE) ;
	(void) SPC_begin() ;
#elif 0
	LED_begin() ;
#elif 0
	while (true) {
		PROD_BSN bsn = { 0 } ;

		(void) PROD_read_board(&bsn) ;

		PROD_PSN psn = { 0 } ;
		(void) PROD_read_product(&psn) ;

		(void) osDelay(1000) ;
	}
#else
#endif
	ESP_LOGI("main", "<<<< app_main") ;
}
