#ifndef TASTO_H_
#define TASTO_H_

#include "bsp.h"

typedef void (* PF_CRJ)(void) ;

bool CRJ_beg(PF_CRJ) ;
void CRJ_end(void) ;

// True if a cable is inserted
bool CRJ_in(void) ;

#endif
