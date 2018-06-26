#include "spc.h"
#include "prod.h"

#define CMD_ECO		((SPC_CMD) 0x0000)

#define CMD_CODP_L	((SPC_CMD) 0x0100)
#define CMD_CODP_S	((SPC_CMD) 0x0101)
#define CMD_CODS_L	((SPC_CMD) 0x0102)
#define CMD_CODS_S	((SPC_CMD) 0x0103)

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
			if ( PROD_write_product(dati) )
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
	case CMD_CODP_S:
		if (0 == dim)
			SPC_err(tx, cmd) ;
		else if (dim >= BOARD_SERIAL_NUMBER_DIM)
			SPC_err(tx, cmd) ;
		else {
			dati[dim] = 0 ;
			if ( PROD_write_board(dati) )
				SPC_resp(tx, cmd, NULL, 0) ;
			else
				SPC_err(tx, cmd) ;
		}
		break ;

	default:
		SPC_unk(tx, cmd) ;
		break ;
	}
}
