#ifndef USPC_H_
#define USPC_H_

#include "bsp.h"

// When a packet is received, the callback will be called
typedef void (*USPC_RX_CB)(void) ;

bool USPC_open(uint32_t baud, USPC_RX_CB) ;
void USPC_close(void) ;

void USPC_tx(const void *, uint16_t) ;
uint16_t USPC_rx(void *, uint16_t) ;

#endif
