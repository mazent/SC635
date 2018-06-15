#include "cmsis_os.h"

#include "prod.h"
#include "led.h"


#include "esp_log.h"
#include "esp_app_trace.h"

extern void ueem(void) ;

void app_main()
{
	ESP_LOGI("main", ">>>> app_main") ;
#if 1
	ueem() ;
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
