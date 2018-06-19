#ifndef TASTO_H_
#define TASTO_H_

#include "bsp.h"

typedef void (* PF_TST)(bool) ;

bool TST_beg(PF_TST) ;
void TST_end(void) ;

bool TST_is_pressed(void) ;

#endif
