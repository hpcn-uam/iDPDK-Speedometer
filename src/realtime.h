#ifndef __REALTIME__H__
#define __REALTIME__H__

#include <sys/time.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#define APROXCICLES (1024*16)
#define EXTRALEN 2.625

uint64_t realtime_timeofday_old;
uint64_t realtime_cicles_old;
uint64_t realtime_hpet_hz;

#define PRECCISION  10000000ull // = 100ns
//2^64 = 	18.446.744.073.709.551.616
//       	36.450.791.397.000.000.000
//			xx.xxx.xxx.xxx.x00.000.000
//			18.446.744.073.7 ciclos ~ 76.8s @ 2.4hz
//			 3.647.989.712.700.000.000
#define PRECCISIONu 10ull

/**
 * Initialice the module
 **/
inline void realtime_init(void);

/**
 * Synchronize the virtual clock with the system clock
 **/
inline void realtime_sync_real(void);

/**
 * Synchronize virtual clock in a virtual way
 **/
inline void realtime_sync(void);

/**
 * Gets the time
 **/
inline uint64_t realtime_get(void); 

/**
 * Gets an approximation of time depending on packet size.
 * For each "APROXCICLES" possible clock skews are fixed.
 **/
inline uint64_t realtime_getAprox(unsigned tam);


/*** PRIVATE ***/
inline uint64_t realtime_get_slow(void);
#endif
