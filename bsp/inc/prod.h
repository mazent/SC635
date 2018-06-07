#ifndef PROD_H_
#define PROD_H_

/*
 * Accesso ai parametri della produzione
 */

#define BOARD_SERIAL_NUMBER_DIM			(11 + 1)
#define PRODUCT_SERIAL_NUMBER_DIM		(12 + 1)

typedef struct {
	size_t len ;
	char bsn[BOARD_SERIAL_NUMBER_DIM] ;
} PROD_BSN ;

typedef struct {
	size_t len ;
	char psn[PRODUCT_SERIAL_NUMBER_DIM] ;
} PROD_PSN ;


bool PROD_read_board(PROD_BSN *) ;
bool PROD_read_product(PROD_PSN *) ;

#endif
