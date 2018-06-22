#ifndef GESTORE_H_
#define GESTORE_H_

#include "bsp.h"
#include "pbc.h"

typedef struct {
	osPoolId mp ;
	void (*conn)(const char * ip, uint16_t porta) ;
	void (*msg)(UN_BUFFER *) ;
	void (*scon)(void) ;
} S_GST_CFG ;

bool GST_beg(S_GST_CFG *) ;
void GST_end(void) ;

size_t GST_tx(const void *, size_t) ;

#endif
