#include "spc.h"
//#include "uspc.h"
//#include "pbc.h"
//
//#define BAUD		115200
//
//#define SPC_STACK		2000
//
//#define SIG_DATA	(1 << 0)
//#define SIG_QUIT	(1 << 1)
//
//static osThreadId tid = NULL ;
//
//static uint8_t * brx = NULL ;
//static uint8_t * btx = NULL ;
//
//static void cb_rx(uint8_t * v, int dim)
//{
//	SPC_CMD cmd ;
//
//	memcpy(&cmd, v, sizeof(SPC_CMD)) ;
//	if (ERR_CMD == (cmd & ERR_OK)) {
//		dim -= sizeof(SPC_CMD) ;
//
//		if (dim) {
//		    // Qua devo passare i dati
//			SPC_msg(cmd, v + sizeof(SPC_CMD), dim) ;
//		}
//		else {
//		    // Non ci sono dati: passo v per fornire memoria temporanea
//			SPC_msg(cmd, v, 0) ;
//		}
//	}
//}
//
//static SPC_RX prx = {
//	.DIM_RX = DIM_BUFFER,
//	.pfMsg = cb_rx
//} ;
//
//static SPC_TX ptx = {
//	.tx = btx,
//	.DIM_TX = DIM_BUFFER
//} ;
//
//static void dati(void)
//{
//	CHECK_IT(osOK == osSignalSet(tid, SIG_DATA)) ;
//}
//
//static void spcThd(void * v)
//{
//    UNUSED(v) ;
//
//    CHECK_IT(USPC_open(BAUD, dati)) ;
//
//    while (true) {
//    	osEvent evn = osSignalWait(0, osWaitForever) ;
//
//    	if (SIG_QUIT & evn.value.signals)
//    		break ;
//
//    	if (SIG_DATA & evn.value.signals) {
//    		while (true) {
//    	    	// il buffer di trasmissione non puo' essere in uso:
//    	    	// vale come temporaneo
//    			const uint16_t LETTI = USPC_rx(btx, sizeof(btx)) ;
//    			if (LETTI)
//    				esamina(&prx, btx, LETTI) ;
//    			else
//    				break ;
//    		}
//    	}
//    }
//
//    USPC_close() ;
//	(void) osThreadTerminate(NULL) ;
//	tid = NULL ;
//}
//
//static int rispondi(SPC_CMD cmd, const void * v, int d)
//{
//	ptx.dimTx = 0 ;
//	ptx.scritti = 0 ;
//
//	componi(&ptx, cmd, v, d) ;
//
//	USPC_tx(btx, ptx.dimTx) ;
//
//	return ptx.scritti ;
//}
//
///******************** Interfaccia ********************/
//
//bool SPC_begin(void)
//{
//    bool esito = false ;
//
//    do {
//    	if (NULL == brx) {
//    		brx = malloc(DIM_BUFFER) ;
//    		assert(brx) ;
//    		if (NULL == brx)
//    			break ;
//    		prx.rx = brx ;
//    	}
//
//    	if (NULL == btx) {
//    		btx = malloc(DIM_BUFFER) ;
//    		assert(btx) ;
//    		if (NULL == btx)
//    			break ;
//    		ptx.tx = btx ;
//    	}
//
//    	if (NULL == tid) {
//        	osThreadDef(spcThd, osPriorityNormal, 1, SPC_STACK) ;
//
//        	tid = osThreadCreate(osThread(spcThd), NULL) ;
//        	assert(tid) ;
//        	if (NULL == tid)
//        		break ;
//    	}
//
//    	da_capo(&prx) ;
//
//        esito = true ;
//    } while (false) ;
//
//    return esito ;
//}
//
//void SPC_end(void)
//{
//	assert(tid) ;
//
//	if (tid) {
//		CHECK_IT(osOK == osSignalSet(tid, SIG_QUIT)) ;
//		while (tid)
//			osDelay(10) ;
//	}
//}
//
//void SPC_resp(SPC_CMD cmd, const void * v, int d)
//{
//	assert(tid) ;
//
//    cmd |= ERR_OK ;
//
//    (void) rispondi(cmd, v, d) ;
//}
//
//int SPC_resp_max(SPC_CMD cmd, const void * v, int d)
//{
//	assert(tid) ;
//
//    cmd |= ERR_OK ;
//
//    return rispondi(cmd, v, d) ;
//}
//
//void SPC_unk(SPC_CMD cmd)
//{
//	assert(tid) ;
//
//    cmd |= ERR_SCO ;
//
//    (void) rispondi(cmd, NULL, 0) ;
//}
//
//void SPC_err(SPC_CMD cmd)
//{
//	assert(tid) ;
//
//    cmd |= ERR_EXE ;
//
//    (void) rispondi(cmd, NULL, 0) ;
//}
//
//// Alternativa ==================
//
//static PF_SPC_A_MSG pfAmsg = NULL ;
//
//static SPC_A_MSG aMsg = { 0 } ;
//
//static void acb_rx(uint8_t * v, int dim)
//{
//	SPC_CMD cmd ;
//
//	memcpy(&cmd, v, sizeof(SPC_CMD)) ;
//	if (ERR_CMD == (cmd & ERR_OK)) {
//		dim -= sizeof(SPC_CMD) ;
//
//		aMsg.cmd = cmd ;
//		aMsg.dim = dim ;
//		if (dim)
//			aMsg.dati = v + sizeof(SPC_CMD) ;
//		else
//			aMsg.dati = NULL ;
//
//		if (pfAmsg)
//			pfAmsg() ;
//	}
//}
//
//static SPC_RX arx = {
//	.pfMsg = acb_rx,
//	.DIM_RX = DIM_BUFFER
//} ;
//
//static SPC_TX atx = {
//		.DIM_TX = DIM_BUFFER
//} ;
//
//void SPC_a_begin(PF_SPC_A_MSG cb)
//{
//	pfAmsg = cb ;
//
//	if (NULL == arx.rx) {
//		arx.rx = malloc(DIM_BUFFER) ;
//		assert(arx.rx) ;
//	}
//
//	if (NULL == atx.tx)  {
//		atx.tx = malloc(DIM_BUFFER) ;
//		assert(atx.tx) ;
//	}
//}
//
//void SPC_a_raw(UN_BUFFER * ub)
//{
//	esamina(&arx, ub->mem, ub->dim) ;
//}
//
//SPC_A_MSG * SPC_a_msg(void)
//{
//	return & aMsg ;
//}
//
//void SPC_a_resp(SPC_A_RSP * r, SPC_CMD cmd, const void * v, int d)
//{
//	assert(r) ;
//
//	if (r) {
//	    cmd |= ERR_OK ;
//
//		atx.dimTx = 0 ;
//		atx.scritti = 0 ;
//
//		componi(&atx, cmd, v, d) ;
//
//		r->dati = atx.tx ;
//		r->dim = atx.dimTx ;
//	}
//}
//
//void SPC_a_unk(SPC_A_RSP * r, SPC_CMD cmd)
//{
//	assert(r) ;
//
//	if (r) {
//		cmd |= ERR_SCO ;
//
//		atx.dimTx = 0 ;
//		atx.scritti = 0 ;
//
//		componi(&atx, cmd, NULL, 0) ;
//
//		r->dati = atx.tx ;
//		r->dim = atx.dimTx ;
//	}
//}
//
//void SPC_a_err(SPC_A_RSP * r, SPC_CMD cmd)
//{
//	assert(r) ;
//
//	if (r) {
//		cmd |= ERR_EXE ;
//
//		atx.dimTx = 0 ;
//		atx.scritti = 0 ;
//
//		componi(&atx, cmd, NULL, 0) ;
//
//		r->dati = atx.tx ;
//		r->dim = atx.dimTx ;
//	}
//}

// I due bit alti indicano:
	// Comando
#define ERR_CMD    (0 << 14)
	// Errore nell'esecuzione del comando
#define ERR_EXE    (1 << 14)
	// Errore: richiesta sconosciuta
#define ERR_SCO    (2 << 14)
	// Tutto bene / maschera
#define ERR_OK     (3 << 14)


#define INIZIO_TRAMA        0x8D
#define FINE_TRAMA          0x8E
#define CARATTERE_DI_FUGA   0x8F


extern uint16_t crc1021V(uint16_t, const uint8_t *, int) ;

static uint16_t CRC_I = 0x5635 ;

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

static void componi(TX_SPC * ptx, SPC_CMD cmd, const void * v, int d)
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
    uint16_t crc = crc1021V(CRC_I, u.b, sizeof(cmd)) ;

    // Dati
    if (v) {
        const uint8_t * p = v ;
        // dim gia' inseriti + crc + fine trama
        const int DIM_TX = ptx->DIM_TX - dimTx - 2 * 2 - 1 ;

        for (scritti=0 ; (scritti < d) && (dimTx < DIM_TX) ; scritti++)
        	dimTx = aggiungi(btx, dimTx, p[scritti]) ;

        crc = crc1021V(crc, p, scritti) ;
    }

    // Checksum
    u.crc = crc ;
    dimTx = aggiungi(btx, dimTx, u.b[1]) ;
    dimTx = aggiungi(btx, dimTx, u.b[0]) ;

    btx[dimTx++] = FINE_TRAMA ;

    ptx->dimTx = dimTx ;
    ptx->scritti = scritti ;
}

static bool rispondi(TX_SPC * ptx, SPC_CMD cmd, const void * v, int d)
{
	ptx->dimTx = 0 ;
	ptx->scritti = 0 ;

	componi(ptx, cmd, v, d) ;

	return ptx->ftx(ptx->tx, ptx->dimTx) ;
}


/********************************************************/

bool SPC_ini_rx(RX_SPC * p)
{
	bool esito = false ;

	p->nega = false ;

	if (NULL == p->rx) {
		p->rx = malloc(p->DIM_RX) ;
		esito = p->rx != NULL ;
	}
	else
		esito = true ;

	return esito ;
}

bool SPC_ini_tx(TX_SPC * p)
{
	bool esito = false ;

	if (NULL == p->tx) {
		p->tx = malloc(p->DIM_TX) ;
		esito = p->tx != NULL ;
	}
	else
		esito = true ;

	return esito ;
}

#define DA_CAPO do {					\
	dimRx = 0 ;         				\
	nega = false ; } while (false)


bool SPC_esamina(RX_SPC * prx, UN_BUFFER * ub)
{
	bool nega = prx->nega ;
	int dimRx = prx->dimRx ;
	uint8_t * brx = prx->rx ;
	const int DIM_RX = prx->DIM_RX ;
	const int LETTI = ub->dim ;
	const uint8_t * dati = ub->mem ;
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
				uint16_t crc = crc1021V(CRC_I, brx, dimRx) ;
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

	return trovato ;
}

bool SPC_resp(TX_SPC * ptx, SPC_CMD cmd, const void * v, int d)
{
	cmd |= ERR_OK ;

	return rispondi(ptx, cmd, v, d) ;
}

bool SPC_unk(TX_SPC * ptx, SPC_CMD cmd)
{
    cmd |= ERR_SCO ;

    return rispondi(ptx, cmd, NULL, 0) ;
}

bool SPC_err(TX_SPC * ptx, SPC_CMD cmd)
{
    cmd |= ERR_EXE ;

    return rispondi(ptx, cmd, NULL, 0) ;
}
