#ifndef FSM_CFG_H_
#define FSM_CFG_H_

#define STAMPA_DBG
#include "esp_log.h"

#ifdef STAMPA_DBG
#	define fsm_printf(f, ...)			ESP_LOGI("fsm", f, ##__VA_ARGS__)
#else
#	define fsm_printf(f, ...)
#endif


// Niente storie
//MZ #define FSM_HISTORY   ( 64 )
#define FSM_HISTORY   ( 0 )

// Anche troppo
#define FSM_NAME_LEN     ( 32 )

// No, grazie
//#define FSM_PROLISSO


// Requisiti
	// memoria
extern void * fsm_malloc(size_t) ;
extern void fsm_free(void *) ;
	// restituisce un riferimento temporale (per stampe debug)
extern uint32_t fsm_adesso(void) ;


#endif
