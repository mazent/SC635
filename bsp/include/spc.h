#ifndef SPC_H_
#define SPC_H_

#include "bsp.h"
#include "pbc.h"

/*
 * Semplice protocollo di comunicazione su seriale
 *
 * Le funzioni extern dovete implementarvele
 */

// I 14 bit piu' bassi
typedef uint16_t SPC_CMD ;


// Inclusi i caratteri di fuga
// Se la risposta e' grande (comandi con offset), usare SPC_respMax
#define MAX_DIM_SPC     1024

bool SPC_begin(void) ;
void SPC_end(void) ;

// Invocata quando arriva un pacchetto
// Se non ci sono dati, la dimensione e' zero ma il buffer non
// e' NULL per riutilizzare la memoria
extern void SPC_msg(SPC_CMD, uint8_t *, int) ;

// Risposte
    // Bene
void SPC_resp(SPC_CMD, const void *, int) ;
int SPC_respMax(SPC_CMD, const void *, int) ;
    // Male
void SPC_unk(SPC_CMD) ;
void SPC_err(SPC_CMD) ;

// Alternativa (p.e. su porta tcp) ---------------------
typedef void (*PF_SPC_A_MSG)(void) ;
void SPC_a_begin(PF_SPC_A_MSG) ;
void SPC_a_raw(UN_BUFFER *) ;

typedef struct {
	SPC_CMD cmd ;
	uint8_t * dati ;
	int dim ;
} SPC_A_MSG ;

SPC_A_MSG * SPC_a_msg(void) ;

typedef struct {
	uint8_t * dati ;
	int dim ;
} SPC_A_RSP ;

void SPC_a_resp(SPC_A_RSP *, SPC_CMD, const void *, int) ;
void SPC_a_unk(SPC_A_RSP *, SPC_CMD) ;
void SPC_a_err(SPC_A_RSP *, SPC_CMD) ;



#endif
