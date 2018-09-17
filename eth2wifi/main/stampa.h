#ifndef STAMPA_H_
#define STAMPA_H_

#if STAMPA_REGISTRI > 0
extern void stampa_registri(void) ;
#else
static inline void stampa_registri(void) {}
#endif

#if STAMPA_WIFI > 0
extern void stampa_wifi(bool in, const void * p, int dim) ;
#else
#	define stampa_wifi(a, b, c)
#endif

#if STAMPA_ETH > 0
extern void stampa_eth(bool in, const void * p, int dim) ;
#else
#	define stampa_eth(a, b, c)
#endif

#if STAMPA_BR > 0
extern void stampa_br(bool in, const void * p, int dim) ;
#else
#	define stampa_br(a, b, c)
#endif




#endif
