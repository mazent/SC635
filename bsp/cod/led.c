#include "bsp.h"
#include "led.h"

#include "driver/gpio.h"

#define LED_R_Vneg		GPIO_NUM_4

static osThreadId tid = NULL ;
static int lev = 0 ;

#define RIPOSO_MS		1000

#define SIG_QUIT		(1 << 0)

static void led(const void * v)
{
	INUTILE(v) ;

	while (true) {
		osEvent evn = osSignalWait(0, RIPOSO_MS) ;

		if (osEventTimeout == evn.status) {
			int curlev = gpio_get_level(LED_R_Vneg) ;
			lev = 0 == lev ? 1 : 0 ;
			gpio_set_level(LED_R_Vneg, lev) ;
		}
		else if (SIG_QUIT & evn.value.signals)
			break ;
	}

	// So I'm dead
	tid = NULL ;
	(void) osThreadTerminate(NULL) ;
}

void LED_begin(void)
{
	if (NULL == tid) {
	    gpio_pad_select_gpio(LED_R_Vneg) ;

	    gpio_set_direction(LED_R_Vneg, GPIO_MODE_OUTPUT);
	    gpio_set_level(LED_R_Vneg, lev) ;

	    osThreadDef(led, osPriorityNormal, 1, 1000) ;
	    tid = osThreadCreate(osThread(led), NULL) ;
	}
}

void LED_end(void)
{
	if (tid)
		(void) osSignalSet(tid, SIG_QUIT) ;
}
