#define STAMPA_DBG
#include "aggiorna.h"
#include "esp_ota_ops.h"

#include "mbedtls/aes.h"

static const char *TAG = "agg";

static uint8_t * nuovo = NULL ;
static uint32_t dimNuovo = 0 ;

static const uint8_t key[32] = {
	0x57, 0xF4, 0x0F, 0xF2, 0x91, 0xBF, 0xDC, 0x8E, 0x69, 0x12, 0x1C, 0xC4, 0xE3, 0x99, 0x05, 0x05,
	0xEA, 0xEA, 0x82, 0x3A, 0x15, 0x1A, 0x39, 0x6B, 0xA9, 0xFE, 0xE4, 0x68, 0x18, 0x75, 0xF4, 0x08
};

static bool decifra(void)
{
	bool esito = false ;
	mbedtls_aes_context aes = { 0 } ;
	uint8_t * iv = nuovo ;
	const uint8_t * ct = nuovo + 16 ;
	const uint32_t DIM = dimNuovo - 16 ;
	uint8_t * pt = NULL ;

	do {
		pt = os_malloc(dimNuovo) ;
		if (NULL == pt) {
			DBG_ERR ;
			break ;
		}

		mbedtls_aes_init(&aes) ;

		(void) mbedtls_aes_setkey_dec(&aes, key, 256) ;

		esito = 0 == mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, DIM, iv, ct, pt) ;

	} while (false) ;

	mbedtls_aes_free(&aes) ;

	if (esito) {
		os_free(nuovo) ;

		nuovo = pt ;
		dimNuovo = DIM ;
	}

	return esito ;
}

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

		if ( !decifra() ) {
			DBG_ERR ;
			break ;
		}

		const esp_partition_t * part = esp_ota_get_next_update_partition(NULL) ;
		if (NULL == part) {
			DBG_ERR ;
			break ;
		}
		ESP_LOGI(TAG, "partizione da %d byte", part->size) ;

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
