#ifndef BSP_H_
#define BSP_H_

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
// Per le PRIu32 ecc
#include <inttypes.h>

// Questi mancano
// ==========================================

#define MIN(a, b)			((a) < (b) ? (a) : (b))
#define MAX(a, b)			((a) > (b) ? (a) : (b))

#define INUTILE(x)          (void)(sizeof(x))
#define NEGA(x)             (~(unsigned int) (x))
#define ABS(x)				(x < 0 ? -(x) : x)

#define DIM_VETT(a)         sizeof(a) / sizeof(a[0])


#endif
