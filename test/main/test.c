#include "prod.h"
#include "spc.h"
#include "tasto.h"
#include "ap.h"
#include "gestore.h"
#include "uspc.h"
#include "cavo.h"
#include "mobd.h"
#include "led.h"
#include "rid.h"
#include "phy.h"
#include "versione.h"

#include "driver/gpio.h"

#include "esp_log.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"

extern void esegui(RX_SPC *, TX_SPC *) ;

static const char * TAG = "test";

#ifdef NDEBUG
const uint32_t VERSIONE = (1 << 24) + VER ;
#else
const uint32_t VERSIONE = VER ;
#endif
const char * DATA = __DATE__ ;

// memoria per i messaggi
osPoolDef(pbcid, NUM_BUFFER, UN_BUFFER) ;
static osPoolId pbcid = NULL ;

// coda dei messaggi
osMessageQDef(comes, 2 * NUM_BUFFER, UN_BUFFER *) ;
static osMessageQId comes = NULL ;
	// speciali
#define MSG_TASTO		0x90B56557
#define MSG_CAVO		0xCA8AB86D
#define MSG_RIDE		0xA74C0DE7

int cntTst = 0 ;

static void tasto(void)
{
	CHECK_IT(osOK == osMessagePut(comes, MSG_TASTO, 0)) ;
}

static void cavo(void)
{
	CHECK_IT(osOK == osMessagePut(comes, MSG_CAVO, 0)) ;
}

static void rid(void)
{
	CHECK_IT(osOK == osMessagePut(comes, MSG_RIDE, 0)) ;
}

static void gst_conn(const char * ip)
{
	UNUSED(ip) ;

	ESP_LOGI(TAG, "connesso %s", ip) ;
}

static void gst_msg(UN_BUFFER * msg)
{
	CHECK_IT(osOK == osMessagePut(comes, (uint32_t) msg, 0)) ;
}

static void gst_scon(void)
{
	ESP_LOGI(TAG, "sconnesso") ;
}

static S_GST_CFG gstcb = {
	.conn = gst_conn, 
	.msg = gst_msg,
	.scon = gst_scon
} ;

static S_USPC_CFG ucfg = {
	.msg = gst_msg
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

static RX_SPC rxSock = {
	.DIM_RX = DIM_BUFFER
} ;
static TX_SPC txSock = {
	.DIM_TX = DIM_BUFFER,
	.ftx = GST_tx
} ;

static RX_SPC rxUart = {
	.DIM_RX = DIM_BUFFER
} ;
static TX_SPC txUart = {
	.DIM_TX = DIM_BUFFER,
	.ftx = USPC_tx
} ;

void app_main()
{
    esp_log_level_set("*", ESP_LOG_INFO) ;

	// questa la fanno sempre
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
		ESP_ERROR_CHECK( nvs_flash_erase() );
		ESP_ERROR_CHECK( nvs_flash_init() );
	}

	// Scambio messaggi
	pbcid = osPoolCreate(osPool(pbcid)) ;
	assert(pbcid) ;
	gstcb.mp = pbcid ;
	ucfg.mp = pbcid ;

	comes = osMessageCreate(osMessageQ(comes), NULL) ;
	assert(comes) ;

	// Varie
    gpio_install_isr_service(0) ;

    tcpip_adapter_init();

    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    // Scheda
    CHECK_IT( TST_beg(tasto) ) ;
    CHECK_IT( CRJ_beg(cavo) ) ;
    CHECK_IT( MOBD_beg() ) ;
    CHECK_IT( LED_beg() ) ;
    CHECK_IT( RID_beg(rid) ) ;
    CHECK_IT( PHY_beg() ) ;

    // Comunicazione
    CHECK_IT( SPC_ini_rx(&rxSock) ) ;
    CHECK_IT( SPC_ini_tx(&txSock) ) ;

    CHECK_IT( SPC_ini_rx(&rxUart) ) ;
    CHECK_IT( SPC_ini_tx(&txUart) ) ;
    	// uart
    CHECK_IT( USPC_open(&ucfg) ) ;
		// ap
	S_AP sap = {
		.ssid = "SC635",
		.max_connection = 1,
		.auth = AUTH_OPEN
	} ;
	CHECK_IT( AP_beg(&sap) ) ;

#ifdef NDEBUG
	ESP_LOGI(TAG, "vers %d", VER) ;
#else
	ESP_LOGI(TAG, "vers %d (dbg)", VER) ;
#endif
	ESP_LOGI(TAG, "data %s", DATA) ;

	// Eseguo i comandi
	while (true) {
		osEvent event = osMessageGet(comes, osWaitForever) ;
		assert(osEventMessage == event.status) ;

		if (osEventMessage == event.status) {
			switch (event.value.v) {
			case MSG_TASTO:
				++cntTst ;
				break ;
			case MSG_CAVO:
				break ;
			case MSG_RIDE:
				break ;
			default: {
					// Comando
					UN_BUFFER * msg = (UN_BUFFER *) event.value.p ;
					RX_SPC * prx = &rxUart ;
					TX_SPC * ptx = &txUart ;

					if (SOCKET == msg->orig) {
						prx = &rxSock ;
						ptx = &txSock ;
					}

					if ( SPC_esamina(prx, msg) )
						esegui(prx, ptx) ;

					CHECK_IT(osOK == osPoolFree(pbcid, msg)) ;
				}
				break ;
			}
		}
	}
}
