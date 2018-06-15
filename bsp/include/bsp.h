#ifndef BSP_H_
#define BSP_H_

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
// Per le PRIu32 ecc
#include <inttypes.h>

// Sistema operativo
#include "cmsis_os.h"

// Log
#include "esp_log.h"


// Questi mancano
// ==========================================

#define MIN(a, b)			((a) < (b) ? (a) : (b))
#define MAX(a, b)			((a) > (b) ? (a) : (b))

#define UNUSED(x)           (void)(sizeof(x))
#define NOT(x)              (~(unsigned int) (x))
#define ABS(x)				(x < 0 ? -(x) : x)

#define DIM_VECT(a)         sizeof(a) / sizeof(a[0])

// Debug
// ==========================================

#ifdef NDEBUG
#   define CHECK_IT(a)      (void)(a)

#   define BPOINT
#else
#   define CHECK_IT(a)      assert(a)

#   define BPOINT			__asm__("break 0,0")
#endif


// Print
// ==========================================

//#ifdef NDEBUG
//#	define PRINTF(f, ...)
//#	define PUTS(a)
//#else
//#	define PRINTF(f, ...)		bsp_printf(f, ##__VA_ARGS__)
//#	define PUTS(a)				bsp_puts(a)
//#endif
//
//// Every file can enable its DBG_xxx
//#ifdef STAMPA_DBG
//#	ifndef NDEBUG
//#		define DBG_ABIL					1
//#	endif
//#	define DBG_FUN						PUTS(__FUNCTION__)
//#	define DBG_ERR						PRINTF("ERR %s %d\n", __FILE__, __LINE__)
//#	define DBG_PRINTF(f, ...)			PRINTF(f, ##__VA_ARGS__)
//#	define DBG_PUTS(a)					PUTS(a)
//#else
//#	define DBG_FUN
//#	define DBG_ERR
//#	define DBG_PRINTF(f, ...)
//#	define DBG_PUTS(a)
//#endif

#define DBG_ERR
#define DBG_PRINTF(f, ...)
#define DBG_PUTS(a)


#endif
