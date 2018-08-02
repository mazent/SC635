#include "spc.h"
#include "prod.h"
#include "cavo.h"
#include "mobd.h"
#include "led.h"
#include "rid.h"
#include "phy.h"
#include "aggiorna.h"

extern int cntTst ;
extern const uint32_t VERSIONE ;
extern const char * DATA ;


#define CMD_ECO		((SPC_CMD) 0x0000)

#define CMD_CODP_L	((SPC_CMD) 0x0100)
#define CMD_CODP_S	((SPC_CMD) 0x0101)
#define CMD_CODS_L	((SPC_CMD) 0x0102)
#define CMD_CODS_S	((SPC_CMD) 0x0103)

#define CMD_TST_Z 	((SPC_CMD) 0x0200)
#define CMD_TST_L 	((SPC_CMD) 0x0201)
#define CMD_CRJ_I 	((SPC_CMD) 0x0202)
#define CMD_MOBD  	((SPC_CMD) 0x0203)
#define CMD_ETH   	((SPC_CMD) 0x0204)
#define CMD_LED   	((SPC_CMD) 0x0205)
#define CMD_RID_I 	((SPC_CMD) 0x0206)
#define CMD_RID_T 	((SPC_CMD) 0x0207)
#define CMD_RID_E 	((SPC_CMD) 0x0208)
#define CMD_PHYRST	((SPC_CMD) 0x0209)

#define CMD_AGG_I	((SPC_CMD) 0x0300)
#define CMD_AGG_D	((SPC_CMD) 0x0301)
#define CMD_AGG_F	((SPC_CMD) 0x0302)
#define CMD_AGG_V	((SPC_CMD) 0x0303)
#define CMD_AGG_DC	((SPC_CMD) 0x0304)

// Sala di lettura
static union {
	PROD_PSN psn ;
	PROD_BSN bsn ;
} sdl ;


void esegui(RX_SPC * rx, TX_SPC * tx)
{
	SPC_CMD cmd ;
	uint8_t * dati = rx->rx + sizeof(SPC_CMD) ;
	int dim = rx->dimRx - sizeof(SPC_CMD) ;

	memcpy(&cmd, rx->rx, sizeof(SPC_CMD)) ;

	switch (cmd) {
	case CMD_ECO:
		SPC_resp(tx, cmd, dati, dim) ;
		break ;

	case CMD_CODP_L:
		if (0 == dim) {
			if ( PROD_read_product(&sdl.psn) )
				SPC_resp(tx, cmd, sdl.psn.psn, sdl.psn.len) ;
			else
				SPC_err(tx, cmd) ;
		}
		else
			SPC_err(tx, cmd) ;
		break ;
	case CMD_CODP_S:
		if (0 == dim)
			SPC_err(tx, cmd) ;
		else if (dim >= PRODUCT_SERIAL_NUMBER_DIM)
			SPC_err(tx, cmd) ;
		else {
			dati[dim] = 0 ;
			if ( PROD_write_product((char *) dati) )
				SPC_resp(tx, cmd, NULL, 0) ;
			else
				SPC_err(tx, cmd) ;
		}
		break ;

	case CMD_CODS_L:
		if (0 == dim) {
			if ( PROD_read_board(&sdl.bsn) )
				SPC_resp(tx, cmd, sdl.bsn.bsn, sdl.bsn.len) ;
			else
				SPC_err(tx, cmd) ;
		}
		else
			SPC_err(tx, cmd) ;
		break ;
	case CMD_CODS_S:
		if (0 == dim)
			SPC_err(tx, cmd) ;
		else if (dim >= BOARD_SERIAL_NUMBER_DIM)
			SPC_err(tx, cmd) ;
		else {
			dati[dim] = 0 ;
			if ( PROD_write_board((char *) dati) )
				SPC_resp(tx, cmd, NULL, 0) ;
			else
				SPC_err(tx, cmd) ;
		}
		break ;

	case CMD_TST_Z:
		if (0 == dim) {
			cntTst = 0 ;
			SPC_resp(tx, cmd, NULL, 0) ;
		}
		else
			SPC_err(tx, cmd) ;
		break ;
	case CMD_TST_L:
		if (0 == dim)
			SPC_resp(tx, cmd, &cntTst, 1) ;
		else
			SPC_err(tx, cmd) ;
		break ;

	case CMD_CRJ_I:
		if (0 == dim) {
			bool x = CRJ_in() ;
			SPC_resp(tx, cmd, &x, 1) ;
		}
		else
			SPC_err(tx, cmd) ;
		break ;

	case CMD_MOBD:
		if (1 == dim) {
			MOBD_mobd_eth(0 != dati[0]) ;
			SPC_resp(tx, cmd, NULL, 0) ;
		}
		else
			SPC_err(tx, cmd) ;
		break ;
	case CMD_ETH:
		if (1 == dim) {
			MOBD_eth_esp32(0 != dati[0]) ;
			SPC_resp(tx, cmd, NULL, 0) ;
		}
		else
			SPC_err(tx, cmd) ;
		break ;

	case CMD_LED:
		if (1 == dim) {
			LED_rosso(0 != dati[0]) ;
			SPC_resp(tx, cmd, NULL, 0) ;
		}
		else
			SPC_err(tx, cmd) ;
		break ;

	case CMD_RID_I:
		if (0 == dim) {
			if ( RID_start() )
				SPC_resp(tx, cmd, NULL, 0) ;
			else
				SPC_err(tx, cmd) ;
		}
		else
			SPC_err(tx, cmd) ;
		break ;
	case CMD_RID_T:
		if (0 == dim) {
			RID_stop() ;
			SPC_resp(tx, cmd, NULL, 0) ;
		}
		else
			SPC_err(tx, cmd) ;
		break ;
	case CMD_RID_E:
		if (0 == dim) {
			bool doip ;
			if ( RID_doip(&doip) )
				SPC_resp(tx, cmd, &doip, 1) ;
			else
				SPC_err(tx, cmd) ;
		}
		else
			SPC_err(tx, cmd) ;
		break ;

	case CMD_PHYRST:
		if (1 == dim) {
			SPC_resp(tx, cmd, NULL, 0) ;

			PHY_reset(dati[0]) ;
		}
		else
			SPC_err(tx, cmd) ;
		break ;

	case CMD_AGG_I:
		if (sizeof(uint32_t) == dim) {
			uint32_t bdim ;

			SPC_resp(tx, cmd, NULL, 0) ;

			memcpy(&bdim, dati, dim) ;
			AGG_beg(bdim) ;
		}
		else
			SPC_err(tx, cmd) ;
		break ;
	case CMD_AGG_D:
		if (dim > sizeof(uint32_t)) {
			uint32_t ofs ;

			memcpy(&ofs, dati, sizeof(uint32_t)) ;
			if ( AGG_dat(dati + sizeof(uint32_t), dim - sizeof(uint32_t), ofs) )
				SPC_resp(tx, cmd, NULL, 0) ;
			else
				SPC_err(tx, cmd) ;
		}
		else
			SPC_err(tx, cmd) ;
		break ;
	case CMD_AGG_F:
		if (0 == dim) {
			if ( AGG_end() )
				SPC_resp(tx, cmd, NULL, 0) ;
			else
				SPC_err(tx, cmd) ;
		}
		else
			SPC_err(tx, cmd) ;
		break ;

	case CMD_AGG_V:
		if (0 == dim)
			SPC_resp(tx, cmd, &VERSIONE, sizeof(VERSIONE)) ;
		else
			SPC_err(tx, cmd) ;
		break ;
	case CMD_AGG_DC:
		if (0 == dim) {
			int d = strlen(DATA) ;

			SPC_resp(tx, cmd, DATA, d) ;
		}
		else
			SPC_err(tx, cmd) ;
		break ;

	default:
		SPC_unk(tx, cmd) ;
		break ;
	}
}
