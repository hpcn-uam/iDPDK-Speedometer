#include <rte_mbuf.h>
#include <rte_malloc.h>
#include <rte_memcpy.h>
#include <rte_spinlock.h>
#include <rte_launch.h>

#include <getopt.h>
#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "external.h"
#include "realtime.h"
#include "pcap_jose.h"
#include "main.h"

#ifndef APP_STATS
#define APP_STATS                    1000000
#endif

#define FILEOPTIONS   (O_CREAT | O_WRONLY /*| O_NONBLOCK | O_NDELAY*/ /*| O_TRUNC*/ | O_DIRECT | O_SYNC)

/*Common*/
struct app_lcore_params * tmp_lp;
uint32_t tmp_bsz_wr;
uint32_t tmp_bsz_rd;

/*Multicore*/
#define NBUFS (4)
rte_spinlock_t locks_R [NBUFS];
rte_spinlock_t locks_W [NBUFS];
int WSlaveid = -1;
unsigned curBuf=0;

/*Disco*/
char outputFolfer [150]    ={0};
uint64_t maxbytes     = 1;
uint64_t fileWrote    = 0; 

/*PCAP*/
pcap_hdr_tJZ defaultPcapHeader;

/*Buffers*/
#define BUFFERKIND uint8_t
BUFFERKIND* diskBuffer [NBUFS];
char changeFile [NBUFS];
unsigned bytesLeft [NBUFS];
uint64_t FileStamp [NBUFS];
BUFFERKIND* diskBufferPTR = NULL;
//BUFFERKIND* diskBufferPTRdw = NULL;
//BUFFERKIND* diskBufferPTRLimit = NULL;
uint64_t diskBufferWrote=0; //32M
uint64_t diskBufferSize=8*1024*1024*1024ul; //1G
uint64_t diskBurst=(1024*1024*1024);//(1024*1024*1024); //4K

/**
 * Obtiene los argumentos (incluidos los de la base) para configurarse
 * NOTA: ya están usados: rx, tx, w, rsz, bsz, pos-lb
 * RET < 0 significa error
 **/
inline int external_config(int argc, char **argv)
{
	if(argc==0 || argv==NULL)
		return -1;
	return 0;
}

/**
 * Realiza las reservas de memoria y demás requerimientos necesarios para el uso de la aplicación externa
 **/
inline void external_init(void)
{
	realtime_init();
}

struct timeval start_ewr, end_ewr;
uint64_t pktsrecv = 0;
uint64_t pkttotalsize = 0;

inline void external_first_exec(struct app_lcore_params *lp, uint32_t bsz_rd, uint32_t bsz_wr)
{
		tmp_lp 		= lp ;
		tmp_bsz_wr 	= bsz_wr;
		tmp_bsz_rd 	= bsz_rd;
		
		realtime_sync_real();
		gettimeofday(&start_ewr, NULL);
}

/**
 * recoge un paquete y lo procesa
 **/
inline void external_work(struct rte_mbuf * pkt)
{
			pktsrecv++;
			
			//realtime_getAprox(pkt->pkt.pkt_len);
			pkttotalsize += pkt->pkt.pkt_len;
	
			if(pkttotalsize>=diskBufferSize)
			{
				gettimeofday(&end_ewr, NULL);
				
				printf("%.2lf GiB @ %lf Gb/s (%.1lf pkt/s)\n", 
			    	diskBufferSize/(1024.*1024.*1024.),
				(((pkttotalsize)+pktsrecv*(4/*crc*/+8/*prelud*/+12/*ifg*/))/(((end_ewr.tv_sec * 1000000. + end_ewr.tv_usec) - (start_ewr.tv_sec * 1000000. + start_ewr.tv_usec))/1000000.))/(1000*1000*1000./8.),
				pktsrecv/(((end_ewr.tv_sec * 1000000. + end_ewr.tv_usec) - (start_ewr.tv_sec * 1000000. + start_ewr.tv_usec)) /1000000.));
				pktsrecv=0;
				pkttotalsize=0;

				gettimeofday(&start_ewr, NULL);
			}
			
			rte_pktmbuf_free(pkt); //liberamos el pkt
		
			
			return;
			
}


/**********************************************************************/
/**
 * Encola un paquete y lo envia
 **/
inline void external_send(struct app_lcore_params_worker *lp, uint32_t bsz_wr, struct rte_mbuf * pkt, uint8_t port)
{
			uint32_t pos;
			int ret;
	
			pos = lp->mbuf_out[port].n_mbufs;

			lp->mbuf_out[port].array[pos ++] = pkt;
			if (likely(pos < bsz_wr)) {
				lp->mbuf_out[port].n_mbufs = pos;
				return;
			}

			/*Se incia el proceso de encolado al nucleo de envio...*/
			ret = rte_ring_sp_enqueue_bulk(
				lp->rings_out[port],
				(void **) lp->mbuf_out[port].array,
				bsz_wr);

		#if APP_STATS
			lp->rings_out_iters[port] ++;
			if (ret == 0) {
				lp->rings_out_count[port] += 1;
			}
			if (lp->rings_out_iters[port] == APP_STATS){
				printf("\t\tWorker %u out (NIC port %u): enq success rate = %.2f (%u/%u)\n",
					lp->worker_id,
					(uint32_t) port,
					((double) lp->rings_out_count[port]) / ((double) lp->rings_out_iters[port]),
					(uint32_t)lp->rings_out_count[port],
					(uint32_t)lp->rings_out_iters[port]);
				lp->rings_out_iters[port] = 0;
				lp->rings_out_count[port] = 0;
			}
		#endif

			if (unlikely(ret == -ENOBUFS)) {
				uint32_t k;
				for (k = 0; k < bsz_wr; k ++) {
					struct rte_mbuf *pkt_to_free = lp->mbuf_out[port].array[k];
					rte_pktmbuf_free(pkt_to_free);
				}
			}

			lp->mbuf_out[port].n_mbufs = 0;
			lp->mbuf_out_flush[port] = 0;
}

