#define STAMPA_DBG
#include "spc_priv.h"

#define INIZIO_TRAMA        0x8D
#define FINE_TRAMA          0x8E
#define CARATTERE_DI_FUGA   0x8F


extern uint16_t crc16_le(uint16_t, const uint8_t *, int) ;

static uint16_t CRC_I = 0x5635 ;

void da_capo(SPC_RX * prx)
{
    prx->dimRx = 0 ;
    prx->nega = false ;
}

#define DA_CAPO do {					\
	dimRx = 0 ;         				\
	nega = false ; } while (false)


void esamina(SPC_RX * prx, const uint8_t * dati, const int LETTI)
{
	bool nega = prx->nega ;
	int dimRx = prx->dimRx ;
	uint8_t * brx = prx->rx ;
	const int DIM_RX = prx->DIM_RX ;
	bool trovato = false ;

	for (int i=0 ; i<LETTI ; i++) {
	    uint8_t rx = dati[i] ;

	    if (nega) {
	        rx = NOT(rx) ;

	        switch (rx) {
	        case INIZIO_TRAMA:
	        case FINE_TRAMA:
	        case CARATTERE_DI_FUGA:
	            // Solo questi sono ammessi
				brx[dimRx++] = rx ;
				nega = false ;
	            break ;
	        default:
	            DA_CAPO ;
	            DBG_ERR ;
	            break ;
	        }
	    }
	    else if (INIZIO_TRAMA == rx)
	        DA_CAPO ;
	    else if (CARATTERE_DI_FUGA == rx)
	        nega = true ;
	    else if (FINE_TRAMA == rx) {
	        if (dimRx < sizeof(SPC_CMD) + 2) {
	            // Ci deve essere almeno il comando e il crc!
	            DBG_ERR ;
	        }
	        else {
	            // Controllo il crc
				uint16_t crc = crc16_le(CRC_I, brx, dimRx) ;
				if (0 == crc) {
					// Tolgo il crc
					dimRx -= 2 ;

	        		trovato = true ;
	        		break ;
				}
	            else {
	                DBG_ERR ;
	            }
	        }

	        // In ogni caso ricomincio
	        DA_CAPO ;
	    }
	    else if (DIM_RX == dimRx) {
	        // Non ci stanno
	        DA_CAPO ;

	        DBG_ERR ;
	    }
	    else {
	    	brx[dimRx++] = rx ;
	    }
	}

	prx->nega = nega ;
	prx->dimRx = dimRx ;
	if (trovato)
		prx->pfMsg(prx->rx, prx->dimRx) ;
}

static uint16_t aggiungi(uint8_t * btx, uint16_t dim, uint8_t x)
{
    if ( (INIZIO_TRAMA == x) ||
         (FINE_TRAMA == x) ||
         (CARATTERE_DI_FUGA == x)
       ) {
        btx[dim++] = CARATTERE_DI_FUGA ;
        x = NOT(x) ;
    }

    btx[dim++] = x ;

    return dim ;
}

void componi(SPC_TX * ptx, SPC_CMD cmd, const void * v, int d)
{
    union {
        SPC_CMD cmd ;
        uint8_t b[1] ;
        uint16_t crc ;
    } u ;
	uint8_t * btx = ptx->tx ;
	int scritti = 0 ;
	int dimTx = 0 ;

    btx[dimTx++] = INIZIO_TRAMA ;

    // comando
    u.cmd = cmd ;
    dimTx = aggiungi(btx, dimTx, u.b[0]) ;
    dimTx = aggiungi(btx, dimTx, u.b[1]) ;
    uint16_t crc = crc16_le(CRC_I, u.b, sizeof(cmd)) ;

    // Dati
    if (v) {
        const uint8_t * p = v ;
        // dim gia' inseriti + crc + fine trama
        const int DIM_TX = ptx->DIM_TX - dimTx - 2 * 2 - 1 ;

        for (scritti=0 ; (scritti < d) && (dimTx < DIM_TX) ; scritti++)
        	dimTx = aggiungi(btx, dimTx, p[scritti]) ;

        crc = crc16_le(crc, p, scritti) ;
    }

    // Checksum
    u.crc = crc ;
    dimTx = aggiungi(btx, dimTx, u.b[1]) ;
    dimTx = aggiungi(btx, dimTx, u.b[0]) ;

    btx[dimTx++] = FINE_TRAMA ;

    ptx->dimTx = dimTx ;
    ptx->scritti = scritti ;
}
