#ifndef RETE_H_
#define RETE_H_

#include "bsp.h"

#define MSG_BRIP		0x512E0461
extern osMessageQId comes ;

typedef struct {
    enum {
    	DA_ETH,
    	DA_WIFI
    } tipo ;

    void * eb ;

    uint16_t len ;

    uint8_t msg[1] ;
} UN_PKT ;

static inline UN_PKT * pkt_malloc(size_t x)
{
	size_t dim = x + sizeof(UN_PKT) - 1 ;

	return os_malloc(dim) ;
}

static inline void pkt_free(UN_PKT * p)
{
	os_free(p) ;
}

typedef struct {
	uint8_t dst[6] ;
	uint8_t srg[6] ;
	uint16_t type ;
} ETH_FRAME ;

static inline uint16_t gira(uint16_t val)
{
	union {
		uint16_t x ;
		uint8_t b[2] ;
	} u ;
	uint8_t tmp ;

	u.x = val ;
	tmp = u.b[0] ;
	u.b[0] = u.b[1] ;
	u.b[1] = tmp ;

	return u.x ;
}

void stampa_eth(const char * t, const uint8_t * p, int dim) ;


void ap_iniz(void) ;
void ap_attivo(bool) ;
extern bool ap_tx(void) ;

void eth_iniz(void) ;
extern bool eth_tx(void) ;

void br_iniz(void) ;
void br_fine(void) ;
void br_pkt(UN_PKT *) ;
void br_input(void *, uint16_t) ;
bool br_valido(void) ;

#endif
