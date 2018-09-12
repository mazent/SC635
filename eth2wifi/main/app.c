#include "phy.h"
#include "mobd.h"

#include "nvs_flash.h"
#include "driver/gpio.h"

extern void BR_start(void) ;

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

	CHECK_IT( PHY_beg() ) ;
	CHECK_IT( MOBD_beg() ) ;

	// una botta al phy
	PHY_reset(10) ;

	// collego maschio obd a eth
	MOBD_mobd_eth(true) ;
	// collego eth al micro
	MOBD_eth_esp32(true) ;

	BR_start() ;
}
