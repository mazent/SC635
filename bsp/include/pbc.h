#ifndef PBC_H_
#define PBC_H_

// Pool di buffer per la comunicazione

#define DIM_BUFFER		2000
#define NUM_BUFFER		10

typedef struct {
	size_t dim ;
	uint8_t mem[DIM_BUFFER] ;
} UN_BUFFER ;


#endif
