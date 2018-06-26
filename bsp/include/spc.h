#ifndef SPC_H_
#define SPC_H_

#include "bsp.h"
#include "pbc.h"

// I 14 bit piu' bassi
typedef uint16_t SPC_CMD ;


typedef struct {
	// stato del protocollo
	bool nega ;

	// il pacchetto finisce qui
	const int DIM_RX ;
	uint8_t * rx ;
	int dimRx ;
} RX_SPC ;

typedef bool (*PF_TX)(const void *, uint16_t) ;

typedef struct {
	// il pacchetto finisce qui
	const int DIM_TX ;
	uint8_t * tx ;
	int dimTx ;

	// quanti, dei byte da inviare, sono nel pacchetto
	int scritti ;

	PF_TX ftx ;
} TX_SPC ;

bool SPC_ini_rx(RX_SPC *) ;
bool SPC_ini_tx(TX_SPC *) ;

bool SPC_esamina(RX_SPC *, UN_BUFFER *) ;

// Risposte
    // Bene
bool SPC_resp(TX_SPC *, SPC_CMD, const void *, int) ;
    // Male
bool SPC_unk(TX_SPC *, SPC_CMD) ;
bool SPC_err(TX_SPC *, SPC_CMD) ;


/*
 * Semplice protocollo di comunicazione su seriale
 *
 * Le funzioni extern dovete implementarvele
 */



//bool SPC_begin(void) ;
//void SPC_end(void) ;
//
//// Invocata quando arriva un pacchetto
//// Se non ci sono dati, la dimensione e' zero ma il buffer non
//// e' NULL per riutilizzare la memoria
//extern void SPC_msg(SPC_CMD, uint8_t *, int) ;
//
//// Risposte
//    // Bene
//void SPC_resp(SPC_CMD, const void *, int) ;
//int SPC_respMax(SPC_CMD, const void *, int) ;
//    // Male
//void SPC_unk(SPC_CMD) ;
//void SPC_err(SPC_CMD) ;
//
//// Alternativa (p.e. su porta tcp) ---------------------
//typedef void (*PF_SPC_A_MSG)(void) ;
//void SPC_a_begin(PF_SPC_A_MSG) ;
//void SPC_a_raw(UN_BUFFER *) ;
//
//typedef struct {
//	SPC_CMD cmd ;
//	uint8_t * dati ;
//	int dim ;
//} SPC_A_MSG ;
//
//SPC_A_MSG * SPC_a_msg(void) ;
//
//typedef struct {
//	uint8_t * dati ;
//	int dim ;
//} SPC_A_RSP ;
//
//void SPC_a_resp(SPC_A_RSP *, SPC_CMD, const void *, int) ;
//void SPC_a_unk(SPC_A_RSP *, SPC_CMD) ;
//void SPC_a_err(SPC_A_RSP *, SPC_CMD) ;



#endif
