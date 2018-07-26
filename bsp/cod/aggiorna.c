#include "aggiorna.h"
#include "esp_ota_ops.h"

static const char *TAG = "agg";

static uint8_t * nuovo = NULL ;
static uint32_t dimNuovo = 0 ;

void AGG_beg(uint32_t dim)
{
	os_free(nuovo) ;

	nuovo = os_malloc(dim) ;
	dimNuovo = dim ;
}

bool AGG_dat(const void * v, uint32_t dim, uint32_t ofs)
{
	bool esito = false ;

	do {
		if (NULL == nuovo)
			break ;

		if (NULL == v)
			break ;

		if (0 == dim)
			break ;

		if (dim + ofs > dimNuovo)
			break ;

		memcpy(nuovo + ofs, v, dim) ;

		esito = true ;

	} while (false) ;

	return esito ;
}

bool AGG_end(void)
{
	bool esito = false ;

	do {
		if (NULL == nuovo)
			break ;

		const esp_partition_t * part = esp_ota_get_next_update_partition(NULL) ;
		if (NULL == part) {
			DBG_ERR ;
			break ;
		}

		if (part->size < dimNuovo) {
			DBG_ERR ;
			break ;
		}

		esp_ota_handle_t update_handle = 0 ;
		esp_err_t err = esp_ota_begin(part, OTA_SIZE_UNKNOWN, &update_handle) ;
	    if (err != ESP_OK) {
			DBG_ERR ;
			break ;
	    }

	    err = esp_ota_write(update_handle, (const void *) nuovo, dimNuovo) ;
	    if (err != ESP_OK) {
			DBG_ERR ;
			break ;
	    }

	    err = esp_ota_end(update_handle) ;
	    if (err != ESP_OK) {
			DBG_ERR ;
			break ;
	    }

	    err = esp_ota_set_boot_partition(part) ;
	    if (err != ESP_OK) {
			DBG_ERR ;
			break ;
	    }

	    os_free(nuovo) ;
	    nuovo = NULL ;
	    dimNuovo = 0 ;

	    esito = true ;
	    //esp_restart();

	} while (false) ;

	return esito ;
}
