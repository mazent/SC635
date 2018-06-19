#include "tasto.h"

#include "driver/gpio.h"

#if 0
	// SC635
#	define BUTTON_SEL		GPIO_SEL_39
#	define BUTTON    		GPIO_NUM_39
#else
#	define BUTTON_SEL		GPIO_SEL_0
#	define BUTTON    		GPIO_NUM_0
#endif

static const gpio_config_t cfg = {
	.pin_bit_mask = BUTTON_SEL,
	.mode = GPIO_MODE_INPUT,
	.intr_type = GPIO_INTR_ANYEDGE
//		// No pull
//		.pull_up_en
//		.pull_down_en
};


static void tst_vuota(bool x)
{
	UNUSED(x) ;
}

static PF_TST cbTst = tst_vuota ;

static void IRAM_ATTR button_isr(void * v)
{
	UNUSED(v) ;

	// Attivo basso
	cbTst( 0 == gpio_get_level(BUTTON) ? true : false) ;
}


bool TST_beg(PF_TST cb)
{
	bool esito = false ;

	assert(cb) ;
	if (cb)
		cbTst = cb ;

	do {
		if (ESP_OK != gpio_config(&cfg))
			break ;

		if (ESP_OK != gpio_isr_handler_add(BUTTON, button_isr, NULL))
			break ;

		esito = true ;
	} while (false) ;

	return esito ;
}

void TST_end(void)
{
	gpio_intr_disable(BUTTON) ;
	gpio_isr_handler_remove(BUTTON) ;

	cbTst = tst_vuota ;
}

bool TST_is_pressed(void)
{
	return 0 == gpio_get_level(BUTTON) ? true : false ;
}
