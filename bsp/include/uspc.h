#ifndef USPC_H_
#define USPC_H_

// When a packet is received, the callback will be called
typedef void (*USPC_RX_CB)(void) ;

bool USPC_beg(uint32_t baud, USPC_RX_CB) ;
void USPC_end(void) ;

void USPC_tx(const void *, uint16_t) ;
uint16_t USPC_rx(void *, uint16_t) ;

#endif
