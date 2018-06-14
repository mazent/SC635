#include <stdio.h>
#include "uspc.h"

static osThreadId tid = NULL ;

#define SIG_QUIT		(1 << 0)
#define SIG_DATA		(1 << 1)

static void uspc_cb(void)
{
	CHECK_IT( osOK == osSignalSet(tid, SIG_DATA) ) ;
}

static void spcThd(void * v)
{
	static uint8_t rx[100] ;

	UNUSED(v) ;

	(void) USPC_beg(115200, uspc_cb) ;

	while (true) {
#if 0
		osEvent evn = osSignalWait(0, osWaitForever) ;
		assert(osEventSignal == evn.status) ;
#else
		osEvent evn = osSignalWait(0, 1000) ;
		if (osEventSignal != evn.status) {
			static int conta = 0 ;
			sprintf((char *) rx, "ciao %d\n", ++conta) ;
			USPC_tx(rx, strlen((char *) rx)) ;
			continue ;
		}
#endif

		if (SIG_QUIT == evn.value.signals)
			break ;

		if (SIG_DATA == evn.value.signals) {
			while (true) {
				const uint16_t LETTI = USPC_rx(rx, sizeof(rx)) ;
				if (0 == LETTI)
					break ;
				else
					USPC_tx(rx, LETTI) ;
			}
		}
	}

	osThreadTerminate(NULL) ;
	tid = NULL ;
}

#define STACK_SIZE			1500
osThreadDef(spcThd, osPriorityNormal, 0, STACK_SIZE) ;


void app_main(void)
{
	tid = osThreadCreate(osThread(spcThd), NULL) ;
	assert(tid) ;
}
