#include "bsp.h"
#include "prod.h"

#include "esp_system.h"
#include "nvs.h"
#include "nvs_flash.h"

// cfr sc635.csv
#define PARTITION		"prod"

// cfr prod.csv
#define NAMESPACE		"prod"
#define KEY_BSN			"scheda"
#define KEY_PSN			"prodotto"

static bool inited = false ;

static bool init(void)
{
	do {
		if (inited)
			break ;

	    esp_err_t err = nvs_flash_init_partition(PARTITION) ;
	    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
#if 0
	    	// This is a read-only partition!
	    	break ;
#else
	        // partition was truncated and needs to be erased
	        err = nvs_flash_erase_partition(PARTITION) ;
	        if (err != ESP_OK)
	        	break ;

	        // Retry nvs_flash_init
	        err = nvs_flash_init_partition(PARTITION) ;
#endif
	    }
        inited = err == ESP_OK ;

	} while (false) ;

	return inited ;
}

bool PROD_read_board(PROD_BSN * p)
{
	bool esito = false ;
	nvs_handle h ;

	do {
		if (NULL == p)
			break ;

		if ( !init() )
			break ;

		esp_err_t err = nvs_open_from_partition(PARTITION, NAMESPACE, NVS_READONLY, &h) ;
		if (err != ESP_OK)
			break ;

		err = nvs_get_str(h, KEY_BSN, p->bsn, &p->len) ;
		esito = err == ESP_OK ;
	} while (false) ;

	nvs_close(h) ;

    return esito ;
}

bool PROD_read_product(PROD_PSN * p)
{
	bool esito = false ;
	nvs_handle h ;

	do {
		if (NULL == p)
			break ;

		if ( !init() )
			break ;

		esp_err_t err = nvs_open_from_partition(PARTITION, NAMESPACE, NVS_READONLY, &h) ;
		if (err != ESP_OK)
			break ;

		err = nvs_get_str(h, KEY_PSN, p->psn, &p->len) ;
		esito = err == ESP_OK ;
	} while (false) ;

	nvs_close(h) ;

    return esito ;
}