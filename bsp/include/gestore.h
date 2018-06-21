#ifndef GESTORE_H_
#define GESTORE_H_

#include "bsp.h"

typedef struct {
	void (*conn)(const char * ip, uint16_t porta) ;
	void (*msg)(void *, int) ;
	void (*scon)(void) ;
} S_GST_CB ;

bool GST_beg(S_GST_CB *) ;
void GST_end(void) ;

size_t GST_tx(const void *, size_t) ;

#endif
