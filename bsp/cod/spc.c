#include "spc.h"
//#include "spc_priv.h"
#include "uspc.h"

//#define PORTA		SC609_SRV
//#define BAUD		115200
//

//static UART_Handle uart = NULL ;

#define SIG_DATA	(1 << 0)

static osThreadId tid = NULL ;

//static uint8_t brx[1 + MAX_DIM_PKS + 2 * 2 + 1] ;
//static uint8_t btx[1 + MAX_DIM_PKS + 2 * 2 + 1] ;
//
//static void cb_rx(uint8_t * u, int d)
//{
//	SPC_CMD cmd ;
//
//	memcpy(&cmd, u, sizeof(SPC_CMD)) ;
//	if (ERR_CMD == (cmd & ERR_OK)) {
//		d -= 2 ;
//
//		if (d) {
//		    // Qua devo passare i dati
//			SPC_msg(cmd, u + 2, d) ;
//		}
//		else {
//		    // Non ci sono dati: passo u per fornire memoria temporanea
//			SPC_msg(cmd, u, 0) ;
//		}
//	}
//}
//
//static PKNF_RX prx = {
//	.rx = brx,
//	.DIM_RX = sizeof(brx),
//	.pfMsg = cb_rx,
//	.CRC_I = 0x5609
//} ;
//
//static PKNF_TX ptx = {
//	.tx = btx,
//	.DIM_TX = sizeof(btx),
//	.CRC_I = 0x5609
//} ;
//
//static UART_Params uartSrvParams = {
//    .readEcho = UART_ECHO_OFF,
//
//    .baudRate = BAUD,
//    .dataLength = UART_LEN_8,
//    .stopBits = UART_STOP_ONE,
//    .parityType = UART_PAR_NONE,
//
//    .writeMode = UART_MODE_BLOCKING,
//    .writeCallback = NULL,
//    .writeDataMode = UART_DATA_BINARY,
//    .writeTimeout = UART_WAIT_FOREVER,
//
//    .readMode = UART_MODE_BLOCKING,
//    .readCallback = NULL,
//    .readDataMode = UART_DATA_TEXT,
//    .readReturnMode = UART_RETURN_NEWLINE,
//    .readTimeout = UART_WAIT_FOREVER
//} ;

static void dati(void)
{
#if 1
	CHECK_IT(osOK == osSignalSet(tid, SIG_DATA)) ;
#else
	static uint8_t tmp[100] ;

	ESP_LOGI("spc-cb", "dati") ;

	while (true) {
		const uint16_t LETTI = USPC_rx(tmp, sizeof(tmp)) ;
		if (LETTI)
			USPC_tx(tmp, LETTI) ;
		else
			break ;
	}
#endif
}

static void srv3d(void * v)
{
    UNUSED(v) ;

    CHECK_IT(USPC_open(115200, dati)) ;

    while (true) {
    	osEvent evn = osSignalWait(0, osWaitForever) ;

    	if (SIG_DATA & evn.value.signals) {
    		static uint8_t tmp[100] ;

    		ESP_LOGI("srv3d", "dati") ;

    		while (true) {
    			const uint16_t LETTI = USPC_rx(tmp, sizeof(tmp)) ;
    			if (LETTI)
    				USPC_tx(tmp, LETTI) ;
    			else
    				break ;
    		}
    	}

//    	// il buffer di trasmissione non puo' essere in uso:
//    	// vale come temporaneo
//        const int LETTI = UART_read(uart, btx, sizeof(btx)) ;
//        esamina(&prx, btx, LETTI) ;
    }
}

//static int rispondi(SPC_CMD cmd, const void * v, int d)
//{
//	ptx.dimTx = 0 ;
//	ptx.scritti = 0 ;
//
//	componi(&ptx, cmd, v, d) ;
//
//	UART_write(uart, btx, ptx.dimTx) ;
//
//	return ptx.scritti ;
//}

/******************** Interfaccia ********************/

bool SPC_begin(void)
{
    bool esito = false ;

    do {
    	if (NULL == tid) {
        	osThreadDef(srv3d, osPriorityNormal, 1, 2000) ;

        	tid = osThreadCreate(osThread(srv3d), NULL) ;
        	assert(tid) ;
        	if (NULL == tid)
        		break ;
    	}

    	//da_capo(&prx) ;

        esito = true ;
    } while (false) ;

    return esito ;
}

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
