#include "prod.h"
#include "led.h"
#include "spc.h"

#define CMD_ECO		((SPC_CMD) 0x0000)

void SPC_msg(SPC_CMD cmd, uint8_t * dati, int dim)
{
	switch (cmd) {
	case CMD_ECO:
		SPC_resp(cmd, dati, dim) ;
		break ;
	}
}

void app_main()
{
    esp_log_level_set("*", ESP_LOG_INFO) ;

	ESP_LOGI("main", ">>>> app_main") ;
#if 1
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
