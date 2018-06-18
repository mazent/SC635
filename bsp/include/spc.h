#ifndef SPC_H_
#define SPC_H_

#include "bsp.h"

/*
 * Semplice protocollo di comunicazione
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


#endif
