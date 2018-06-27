#include "mobd.h"

#define MOBD_SEL		GPIO_SEL_33
#define MOBD    		GPIO_NUM_33

#define ETH_SEL			GPIO_SEL_5
#define ETH    			GPIO_NUM_5

static const gpio_config_t cfg = {
	.pin_bit_mask = MOBD_SEL | ETH_SEL,
	.mode = GPIO_MODE_OUTPUT,
	.intr_type = GPIO_INTR_DISABLE,
	.pull_up_en = GPIO_PULLUP_DISABLE,
	.pull_down_en = GPIO_PULLDOWN_DISABLE
};


bool MOBD_beg(void)
{
	return ESP_OK == gpio_config(&cfg) ;
}

void MOBD_end(void)
{
	CHECK_IT(ESP_OK == gpio_reset_pin(MOBD)) ;
	CHECK_IT(ESP_OK == gpio_reset_pin(ETH)) ;
}

void MOBD_mobd_eth(bool eth)
{
	gpio_set_level(MOBD, eth ? 1 : 0) ;
}

void MOBD_eth_esp32(bool esp32)
{
	gpio_set_level(ETH, esp32 ? 1 : 0) ;
}

