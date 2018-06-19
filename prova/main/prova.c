#include "prod.h"
#include "led.h"
#include "spc.h"
#include "tasto.h"

#include "driver/gpio.h"

#define CMD_ECO		((SPC_CMD) 0x0000)

void SPC_msg(SPC_CMD cmd, uint8_t * dati, int dim)
{
	switch (cmd) {
	case CMD_ECO:
		SPC_resp(cmd, dati, dim) ;
		break ;
	default:
		SPC_unk(cmd) ;
		break ;
	}
}

bool pres = false ;
bool rila = false ;

void tasto(bool premuto)
{
	if (premuto)
		pres = true ;
	else
		rila = true ;
}

void app_main()
{
    esp_log_level_set("*", ESP_LOG_INFO) ;
    gpio_install_isr_service(0) ;

	ESP_LOGI("main", ">>>> app_main") ;
#if 1
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
