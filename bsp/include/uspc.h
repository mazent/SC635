#ifndef USPC_H_
#define USPC_H_

#include "bsp.h"
#include "pbc.h"

typedef struct {
	osPoolId mp ;
	void (*msg)(UN_BUFFER *) ;
} S_USPC_CFG ;

bool USPC_open(S_USPC_CFG *) ;
void USPC_close(void) ;

bool USPC_tx(const void *, uint16_t) ;


//// When a packet is received, the callback will be called
//typedef void (*USPC_RX_CB)(void) ;
//
//bool USPC_open(uint32_t baud, USPC_RX_CB) ;
//void USPC_close(void) ;
//
//void USPC_tx(const void *, uint16_t) ;
//uint16_t USPC_rx(void *, uint16_t) ;

#endif
