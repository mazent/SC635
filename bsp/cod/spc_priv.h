#ifndef SPC_PRIV_H_
#define SPC_PRIV_H_

#include "spc.h"

// I due bit alti indicano:
	// Comando
#define ERR_CMD    (0 << 14)
	// Errore nell'esecuzione del comando
#define ERR_EXE    (1 << 14)
	// Errore: richiesta sconosciuta
#define ERR_SCO    (2 << 14)
	// Tutto bene / maschera
#define ERR_OK     (3 << 14)

typedef void (* PF_MSG)(uint8_t *, int) ;

typedef struct {
	// stato del protocollo
	bool nega ;

	// il pacchetto finisce qui
	uint8_t * rx ;
	const int DIM_RX ;
	int dimRx ;

	// callback ricezione
	PF_MSG pfMsg ;

} SPC_RX ;

void da_capo(SPC_RX *) ;
void esamina(SPC_RX *, const uint8_t *, const int) ;

typedef struct {
	// il pacchetto finisce qui
	uint8_t * tx ;
	const int DIM_TX ;
	int dimTx ;

	// quanti, dei byte da inviare, sono nel pacchetto
	int scritti ;
} SPC_TX ;

void componi(SPC_TX *, PKNF_CMD, const void *, int) ;

#endif
