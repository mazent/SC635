#include "cmsis_os.h"
#include "esp_log.h"

#include "prod.h"
#include "led.h"
#include "uspc.h"


void dati(void)
{
	static uint8_t tmp[100] ;

	ESP_LOGI("main", "dati") ;

	while (true) {
		const uint16_t LETTI = USPC_rx(tmp, sizeof(tmp)) ;
		if (LETTI)
			USPC_tx(tmp, LETTI) ;
		else
			break ;
	}
}

void app_main()
{
    esp_log_level_set("*", ESP_LOG_INFO);

	ESP_LOGI("main", ">>>> app_main") ;
#if 1
    USPC_beg(115200, dati) ;
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
