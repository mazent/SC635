#include "cmsis_os.h"

#include "prod.h"
#include "led.h"


#include "esp_log.h"
#include "esp_app_trace.h"


void app_main()
{
	esp_log_set_vprintf(esp_apptrace_vprintf);
	ESP_LOGI("main", ">>>> app_main") ;
#if 1
	LED_begin() ;
#else
	while (true) {
		PROD_BSN bsn = { 0 } ;

		(void) PROD_read_board(&bsn) ;

		PROD_PSN psn = { 0 } ;
		(void) PROD_read_product(&psn) ;

		(void) osDelay(1000) ;
	}
#endif
	ESP_LOGI("main", "<<<< app_main") ;
}
