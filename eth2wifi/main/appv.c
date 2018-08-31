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
#include "fsm.h"

#include "rete.h"

#include "driver/gpio.h"

#include "esp_log.h"
#include "esp_event_loop.h"
#include "esp_wifi.h"
#include "esp_wifi_internal.h"
#include "esp_eth.h"
#include "nvs_flash.h"

extern void esegui(RX_SPC *, TX_SPC *) ;

static const char * TAG = "bridge";

#ifdef NDEBUG
const uint32_t VERSIONE = (1 << 24) + VER ;
#else
const uint32_t VERSIONE = VER ;
#endif
const char * DATA = __DATE__ ;

// coda dei messaggi
osMessageQDef(comes, 100, uint32_t) ;
osMessageQId comes = NULL ;
	// speciali (da 0x50002000 in su, indirizzi riservati)
#define MSG_TASTO		0x51B56557
#define MSG_CAVO		0x518AB86D
#define MSG_RIDE		0x514C0DE7
#define MSG_ARIMB		0x5176FD23
#define MSG_STAZ		0x51B1E36B
#define MSG_ETH 		0x511AC90C //65 E7 AD

// Evito i rimbalzi meccanici del cavo RJ
#define ANTIRIMBALZO	50

static void antirimb(void * v)
{
	UNUSED(v) ;

	CHECK_IT(osOK == osMessagePut(comes, MSG_ARIMB, 0)) ;
}

osTimerDef(timArimb, antirimb) ;
static osTimerId timArimb = NULL ;


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

static int stazioni = 0 ;
static bool ethernet = false ;

bool eth_tx(void)
{
	return ethernet ;
}

bool ap_tx(void)
{
	return stazioni > 0 ;
}


static esp_err_t event_handler(void *ctx, system_event_t *event)
{
	UNUSED(ctx) ;

	switch (event->event_id) {
    case SYSTEM_EVENT_WIFI_READY:               /**< ESP32 WiFi ready */
    	ESP_LOGI(TAG, "SYSTEM_EVENT_WIFI_READY");
    	break ;
//    case SYSTEM_EVENT_SCAN_DONE:                /**< ESP32 finish scanning AP */
//    	ESP_LOGI(TAG, "SYSTEM_EVENT_SCAN_DONE");
//    	break ;
//    case SYSTEM_EVENT_STA_START:                /**< ESP32 station start */
//		ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START");
//		break;
//    case SYSTEM_EVENT_STA_STOP:                 /**< ESP32 station stop */
//    	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_STOP");
//    	break ;
//    case SYSTEM_EVENT_STA_CONNECTED:            /**< ESP32 station connected to AP */
//    	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_CONNECTED");
//    	break ;
//    case SYSTEM_EVENT_STA_DISCONNECTED:         /**< ESP32 station disconnected from AP */
//		ESP_LOGI(TAG, "SYSTEM_EVENT_STA_DISCONNECTED");
//		break;
//    case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:      /**< the auth mode of AP connected by ESP32 station changed */
//    	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_AUTHMODE_CHANGE");
//    	break ;
//    case SYSTEM_EVENT_STA_GOT_IP:               /**< ESP32 station got IP from connected AP */
//		ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");
//		break ;
//    case SYSTEM_EVENT_STA_LOST_IP:              /**< ESP32 station lost IP and the IP is reset to 0 */
//    	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_LOST_IP");
//    	break ;
//    case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:       /**< ESP32 station wps succeeds in enrollee mode */
//    	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_WPS_ER_SUCCESS");
//    	break ;
//    case SYSTEM_EVENT_STA_WPS_ER_FAILED:        /**< ESP32 station wps fails in enrollee mode */
//    	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_WPS_ER_FAILED");
//    	break ;
//    case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:       /**< ESP32 station wps timeout in enrollee mode */
//    	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_WPS_ER_TIMEOUT");
//    	break ;
//    case SYSTEM_EVENT_STA_WPS_ER_PIN:           /**< ESP32 station wps pin code in enrollee mode */
//    	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_WPS_ER_PIN");
//    	break ;

	case SYSTEM_EVENT_AP_START:
		ESP_LOGI(TAG, "SYSTEM_EVENT_AP_START");

		{
			uint8_t mac[6] = {0} ;
			esp_wifi_get_mac(WIFI_IF_AP, mac) ;
			ESP_LOGI(TAG, "AP: %02X:%02X:%02X:%02X:%02X:%02X", MAC2STR(mac)) ;
		}
		break ;
    case SYSTEM_EVENT_AP_STACONNECTED:
        printf("SYSTEM_EVENT_AP_STACONNECTED\r\n");
        ++stazioni ;
        if (1 == stazioni) {
        	ap_attivo(true) ;

        	CHECK_IT(osOK == osMessagePut(comes, MSG_STAZ, 0)) ;
        }
        break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
        printf("SYSTEM_EVENT_AP_STADISCONNECTED\r\n");
        --stazioni ;
        if (0 == stazioni) {
        	ap_attivo(false) ;

        	CHECK_IT(osOK == osMessagePut(comes, MSG_STAZ, 0)) ;
        }
        break;


//    case SYSTEM_EVENT_AP_STOP:                  /**< ESP32 soft-AP stop */
//    	ESP_LOGI(TAG, "SYSTEM_EVENT_AP_STOP");
//    	AP_evn(AP_EVN_STOP, &event->event_info) ;
//    	GST_end() ;
//    	break ;
//    case SYSTEM_EVENT_AP_STAIPASSIGNED:         /**< ESP32 soft-AP assign an IP to a connected station */
//    	ESP_LOGI(TAG, "SYSTEM_EVENT_AP_STAIPASSIGNED");
//    	AP_evn(AP_EVN_STAIPASSIGNED, &event->event_info) ;
//    	break ;
//    case SYSTEM_EVENT_AP_PROBEREQRECVED:        /**< Receive probe request packet in soft-AP interface */
//    	ESP_LOGI(TAG, "SYSTEM_EVENT_AP_PROBEREQRECVED");
//    	break ;
//    case SYSTEM_EVENT_GOT_IP6:                  /**< ESP32 station or ap or ethernet interface v6IP addr is preferred */
//    	ESP_LOGI(TAG, "SYSTEM_EVENT_GOT_IP6");
//    	break ;

    case SYSTEM_EVENT_ETH_START:
    	ESP_LOGI(TAG, "SYSTEM_EVENT_ETH_START");
    	break ;
    case SYSTEM_EVENT_ETH_STOP:
    	ESP_LOGI(TAG, "SYSTEM_EVENT_ETH_STOP");
    	break ;
    case SYSTEM_EVENT_ETH_CONNECTED:
    	ESP_LOGI(TAG, "SYSTEM_EVENT_ETH_CONNECTED");
        ethernet = true;
		{
			uint8_t mac[6] = {0} ;
			esp_eth_get_mac(mac) ;
			ESP_LOGI(TAG, "ETH: "MACSTR"", MAC2STR(mac)) ;
		}
		CHECK_IT(osOK == osMessagePut(comes, MSG_ETH, 0)) ;
        break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
    	ESP_LOGI(TAG, "SYSTEM_EVENT_ETH_DISCONNECTED");
    	ethernet = false ;
    	CHECK_IT(osOK == osMessagePut(comes, MSG_ETH, 0)) ;
    	break ;

//    case SYSTEM_EVENT_ETH_GOT_IP:               /**< ESP32 ethernet got IP from connected AP */
//    	ESP_LOGI(TAG, "SYSTEM_EVENT_ETH_GOT_IP");
//    	break ;

	default:
		ESP_LOGE(TAG, "? evento %d %p ?", event->event_id, &event->event_info) ;
		break;
	}

	return ESP_OK;
}

//static RX_SPC rxSock = {
//	.DIM_RX = DIM_BUFFER
//} ;
//static TX_SPC txSock = {
//	.DIM_TX = DIM_BUFFER,
//	.ftx = GST_tx
//} ;
//
//static RX_SPC rxUart = {
//	.DIM_RX = DIM_BUFFER
//} ;
//static TX_SPC txUart = {
//	.DIM_TX = DIM_BUFFER,
//	.ftx = USPC_tx
//} ;




// ========= MACCHINA A STATI =================================================

static fsm_t * fsm = NULL ;


void * fsm_malloc(size_t dim)
{
	return os_malloc(dim) ;
}

void fsm_free(void * v)
{
	os_free(v) ;
}

uint32_t fsm_adesso(void)
{
	return osKernelSysTick() ;
}

// Eventi
typedef enum {
	TASTO = 0,
	CAVO_RJ,
	DIAG_RIL,
	E_AP,
	E_ETH,
	E_IP,
	DATI
} E_VENTO ;

static const event_description_t desc_evn[] = {
	{ TASTO, "tasto" },
	{ CAVO_RJ, "cavo rj45" },
	{ DIAG_RIL, "fine ril. diagnosi" },
	{ E_AP, "stazione connessa" },
	{ E_ETH, "eth connessa" },
	{ E_IP, "bridge ip" },
	{ DATI, "dati" },
	{ FSM_NULL_EVENT_ID, NULL }
} ;

// Stati
typedef enum {
	ATTESA = 0,
	RIL_DOIP,
	ETH,
	BRIDGE,
	SRV
} S_TATO ;

static const state_description_t desc_stt[] = {
	{ ATTESA, "Attesa" },
	{ RIL_DOIP, "Rileva doip" },
	{ ETH, "Attiva ethernet" },
	{ BRIDGE, "Bridge" },
	{ SRV, "Servizio" },
	{ FSM_NULL_STATE_ID, NULL }
} ;

// Attesa ----------------------

static RC_FSM_t attesa_ap(void * prm)
{
	int * stazioni = (int *) prm ;

	if (stazioni) {
		// Prima stazione connessa!
		RID_stop() ;

#if 1
		// Senza doip
		fsm_set_exception_state(fsm, ETH) ;
#endif
		return RC_FSM_OK ;
	}
	else
		return RC_FSM_IGNORE_EVENT ;
}

static const event_tuple_t stt_attesa[] = {
	{ TASTO,    NULL, FSM_NULL_STATE_ID },
	{ CAVO_RJ,  NULL, FSM_NULL_STATE_ID },
	{ DIAG_RIL, NULL, FSM_NULL_STATE_ID },
	{ E_AP,		attesa_ap, RIL_DOIP },
	{ E_ETH,    NULL, FSM_NULL_STATE_ID },
	{ E_IP,     NULL, FSM_NULL_STATE_ID },
	{ DATI,     NULL, FSM_NULL_STATE_ID }
} ;

// Rileva diagnosi -------------

static void rid_entra(void)
{
	(void) RID_start() ;
}

static RC_FSM_t rid_ril(void * prm)
{
	bool * doip = (bool *) prm ;

	if (doip) {
		// Ottimo
		return RC_FSM_OK ;
	}
	else {
		// Classica
		return RC_FSM_IGNORE_EVENT ;
	}
}


static const event_tuple_t stt_rid[] = {
	{ TASTO,    NULL, FSM_NULL_STATE_ID },
	{ CAVO_RJ,  NULL, FSM_NULL_STATE_ID },
	{ DIAG_RIL, rid_ril, ETH },
	{ E_AP,		NULL, FSM_NULL_STATE_ID },
	{ E_ETH,    NULL, FSM_NULL_STATE_ID },
	{ E_IP,     NULL, FSM_NULL_STATE_ID },
	{ DATI,     NULL, FSM_NULL_STATE_ID }
} ;

// Ethernet --------------------

static void eth_entra(void)
{
	// collego maschio obd a eth
	MOBD_mobd_eth(true) ;

	// collego eth al micro
	MOBD_eth_esp32(true) ;

	eth_iniz() ;
}

static RC_FSM_t eth_eth(void * prm)
{
	bool * eth = (bool *) prm ;

	if (eth) {
		// Ottimo
		return RC_FSM_OK ;
	}
	else {
		// ???
		return RC_FSM_IGNORE_EVENT ;
	}
}

static const event_tuple_t stt_eth[] = {
	{ TASTO,    NULL, FSM_NULL_STATE_ID },
	{ CAVO_RJ,  NULL, FSM_NULL_STATE_ID },
	{ DIAG_RIL, NULL, FSM_NULL_STATE_ID },
	{ E_AP,		NULL, FSM_NULL_STATE_ID },
	{ E_ETH,    eth_eth, BRIDGE },
	{ E_IP,		NULL, FSM_NULL_STATE_ID },
	{ DATI,     NULL, FSM_NULL_STATE_ID }
} ;

// Bridge ----------------------

static void bri_entra(void)
{
	br_iniz() ;
}

static RC_FSM_t bri_ip(void * prm)
{
	bool * eth = (bool *) prm ;

	if (eth) {
		// Ottimo
		return RC_FSM_OK ;
	}
	else {
		// ???
		return RC_FSM_IGNORE_EVENT ;
	}
}

static const event_tuple_t stt_bri[] = {
	{ TASTO,    NULL, FSM_NULL_STATE_ID },
	{ CAVO_RJ,  NULL, FSM_NULL_STATE_ID },
	{ DIAG_RIL, NULL, FSM_NULL_STATE_ID },
	{ E_AP,		NULL, FSM_NULL_STATE_ID },
	{ E_ETH,    NULL, FSM_NULL_STATE_ID },
	{ E_IP,		bri_ip, SRV },
	{ DATI,     NULL, FSM_NULL_STATE_ID }
} ;

// Servizio --------------------

static void srv_entra(void)
{
}

static RC_FSM_t srv_dati(void * prm)
{
	UN_PKT * pP = (UN_PKT *) prm ;

	br_input(pP->msg, pP->len) ;

	return RC_FSM_OK ;
}


static const event_tuple_t stt_srv[] = {
	{ TASTO,    NULL, FSM_NULL_STATE_ID },
	{ CAVO_RJ,  NULL, FSM_NULL_STATE_ID },
	{ DIAG_RIL, NULL, FSM_NULL_STATE_ID },
	{ E_AP,		NULL, FSM_NULL_STATE_ID },
	{ E_ETH,    NULL, FSM_NULL_STATE_ID },
	{ E_IP,		NULL, FSM_NULL_STATE_ID },
	{ DATI,     srv_dati, SRV }
} ;


// Riassunto -------------------

static const state_tuple_t fsm_tab[] = {
	{ ATTESA,   stt_attesa, NULL, NULL },
	{ RIL_DOIP, stt_rid,    rid_entra, NULL },
	{ ETH,      stt_eth,    eth_entra, NULL },
	{ BRIDGE,   stt_bri,    bri_entra, NULL },
	{ SRV,   	stt_srv,    srv_entra, NULL },
	{ FSM_NULL_STATE_ID, NULL }
} ;


// ============================================================================


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
	comes = osMessageCreate(osMessageQ(comes), NULL) ;
	assert(comes) ;

	// Varie
    gpio_install_isr_service(0) ;

    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    timArimb = osTimerCreate(osTimer(timArimb), osTimerOnce, NULL) ;
    assert(timArimb) ;

    // Scheda
    CHECK_IT( TST_beg(tasto) ) ;
    CHECK_IT( CRJ_beg(cavo) ) ;
    CHECK_IT( MOBD_beg() ) ;
    CHECK_IT( LED_beg() ) ;
    CHECK_IT( RID_beg(rid) ) ;
    CHECK_IT( PHY_beg() ) ;

    // Ciccio
	CHECK_IT(
			RC_FSM_OK ==
			fsm_create(&fsm,
					   "bridge fsm",
					   ATTESA,
					   desc_stt,
					   desc_evn,
					   fsm_tab)
		   ) ;

	// Inizio con l'ap
    ap_iniz() ;

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
				ESP_LOGI(TAG, "tasto premuto") ;
				break ;
			case MSG_CAVO:
				CHECK_IT(osOK == osTimerStart(timArimb, ANTIRIMBALZO)) ;
				break ;
			case MSG_ARIMB:
				if (CRJ_in())
					ESP_LOGI(TAG, "cavo RJ inserito") ;
				else
					ESP_LOGI(TAG, "cavo RJ estratto") ;
				break ;
			case MSG_RIDE: {
					bool doip ;
					(void) RID_doip(&doip) ;

					ESP_LOGI(TAG, "fine rilevazione diagnosi: %s", doip ? "DoIP" : "classica") ;

					(void) fsm_engine(fsm, DIAG_RIL, &doip) ;
				}
				break ;
			case MSG_STAZ:
				(void) fsm_engine(fsm, E_AP, &stazioni) ;
				break ;
			case MSG_ETH:
				(void) fsm_engine(fsm, E_ETH, &ethernet) ;
				break ;
			case MSG_BRIP: {
					bool valido = br_valido() ;

					(void) fsm_engine(fsm, E_IP, &valido) ;
				}
				break ;
			default: {
					UN_PKT * pP = event.value.p ;
					switch (pP->tipo) {
					case DA_ETH: {
#if 0
							ETH_FRAME * pF = (ETH_FRAME *) pP->msg ;

							ESP_LOGI(TAG, "ETH[%d] %02X:%02X:%02X:%02X:%02X:%02X -> %02X:%02X:%02X:%02X:%02X:%02X %04X",
									pP->len, MAC2STR(pF->srg), MAC2STR(pF->dst), gira(pF->type)) ;
#endif
							if (stazioni)
								esp_wifi_internal_tx(ESP_IF_WIFI_AP, pP->msg, pP->len - 4);

							(void) fsm_engine(fsm, DATI, pP) ;
						}
						break ;
					case DA_WIFI: {
#if 0
							ETH_FRAME * pF = (ETH_FRAME *) pP->msg ;

							ESP_LOGI(TAG, "WiFi[%d] %02X:%02X:%02X:%02X:%02X:%02X -> %02X:%02X:%02X:%02X:%02X:%02X %04X",
									pP->len, MAC2STR(pF->srg), MAC2STR(pF->dst), gira(pF->type)) ;
#endif
							if (ethernet)
								esp_eth_tx(pP->msg, pP->len);

							(void) fsm_engine(fsm, DATI, pP) ;
						}
						break ;
					}
					pkt_free(pP) ;
				}
				break ;
			}
		}
	}
}
