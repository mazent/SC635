#include "cmsis_os.h"

#include "prod.h"

void app_main()
{
	while (true) {
		PROD_BSN bsn = { 0 } ;

		(void) PROD_read_board(&bsn) ;

		PROD_PSN psn = { 0 } ;
		(void) PROD_read_product(&psn) ;

		(void) osDelay(1000) ;
	}
}
