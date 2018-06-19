#include "spc_priv.h"
#include "uspc.h"

#define BAUD		921600

#define SPC_STACK		2000

#define SIG_DATA	(1 << 0)
#define SIG_QUIT	(1 << 1)

static osThreadId tid = NULL ;

static uint8_t brx[1 + MAX_DIM_SPC + 2 * 2 + 1] ;
static uint8_t btx[1 + MAX_DIM_SPC + 2 * 2 + 1] ;

static void cb_rx(uint8_t * v, int dim)
{
	SPC_CMD cmd ;

	memcpy(&cmd, v, sizeof(SPC_CMD)) ;
	if (ERR_CMD == (cmd & ERR_OK)) {
		dim -= 2 ;

		if (dim) {
		    // Qua devo passare i dati
			SPC_msg(cmd, v + 2, dim) ;
		}
		else {
		    // Non ci sono dati: passo v per fornire memoria temporanea
			SPC_msg(cmd, v, 0) ;
		}
	}
}

static SPC_RX prx = {
	.rx = brx,
	.DIM_RX = sizeof(brx),
	.pfMsg = cb_rx,

} ;

static SPC_TX ptx = {
	.tx = btx,
	.DIM_TX = sizeof(btx),
} ;

static void dati(void)
{
	CHECK_IT(osOK == osSignalSet(tid, SIG_DATA)) ;
}

static void spcThd(void * v)
{
    UNUSED(v) ;

    CHECK_IT(USPC_open(BAUD, dati)) ;

    while (true) {
    	osEvent evn = osSignalWait(0, osWaitForever) ;

    	if (SIG_QUIT & evn.value.signals)
    		break ;

    	if (SIG_DATA & evn.value.signals) {
    		while (true) {
    	    	// il buffer di trasmissione non puo' essere in uso:
    	    	// vale come temporaneo
    			const uint16_t LETTI = USPC_rx(btx, sizeof(btx)) ;
    			if (LETTI)
    				esamina(&prx, btx, LETTI) ;
    			else
    				break ;
    		}
    	}
    }

    USPC_close() ;
	(void) osThreadTerminate(NULL) ;
	tid = NULL ;
}

static int rispondi(SPC_CMD cmd, const void * v, int d)
{
	ptx.dimTx = 0 ;
	ptx.scritti = 0 ;

	componi(&ptx, cmd, v, d) ;

	USPC_tx(btx, ptx.dimTx) ;

	return ptx.scritti ;
}

/******************** Interfaccia ********************/

bool SPC_begin(void)
{
    bool esito = false ;

    do {
    	if (NULL == tid) {
        	osThreadDef(spcThd, osPriorityNormal, 1, SPC_STACK) ;

        	tid = osThreadCreate(osThread(spcThd), NULL) ;
        	assert(tid) ;
        	if (NULL == tid)
        		break ;
    	}

    	da_capo(&prx) ;

        esito = true ;
    } while (false) ;

    return esito ;
}

void SPC_end(void)
{
	assert(tid) ;

	if (tid) {
		CHECK_IT(osOK == osSignalSet(tid, SIG_QUIT)) ;
		while (tid)
			osDelay(10) ;
	}
}

void SPC_resp(SPC_CMD cmd, const void * v, int d)
{
	assert(tid) ;

    cmd |= ERR_OK ;

    (void) rispondi(cmd, v, d) ;
}

int SPC_resp_max(SPC_CMD cmd, const void * v, int d)
{
	assert(tid) ;

    cmd |= ERR_OK ;

    return rispondi(cmd, v, d) ;
}

void SPC_unk(SPC_CMD cmd)
{
	assert(tid) ;

    cmd |= ERR_SCO ;

    (void) rispondi(cmd, NULL, 0) ;
}

void SPC_err(SPC_CMD cmd)
{
	assert(tid) ;

    cmd |= ERR_EXE ;

    (void) rispondi(cmd, NULL, 0) ;
}
