#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT    0xC635
#define STACK	2000

#include "gestore.h"

static const char *TAG = "gst";

static osThreadId tid = NULL ;
#define DIM_MSG 		1600
static uint8_t * msg = NULL ;

static S_GST_CB cb = {
	.conn = NULL,
	.msg = NULL,
	.scon = NULL
} ;

static int cln = -1 ;

#define CMD_ESCI		((uint32_t)	0xCD3F6568)

static void riusabile(int sockfd)
{
	int optval = 1 ;
	/* setsockopt: Handy debugging trick that lets
	 * us rerun the server immediately after we kill it;
	 * otherwise we have to wait about 20 secs.
	 * Eliminates "ERROR on binding: Address already in use" error.
	 */
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
			(const void *)&optval , sizeof(int));
}

static bool invia(uint32_t cmd)
{
	bool esito = false ;
	int soc = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

	do {
		if (soc < 0)
			break ;

		struct sockaddr_in server = { 0 } ;
		server.sin_family = AF_INET;
		server.sin_port = htons(PORT);
		server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

		if (sendto(soc, &cmd, sizeof(cmd), 0,
		                 (struct sockaddr *)&server, sizeof(server)) < 0)
			break ;

		uint32_t rsp ;
		int n = recvfrom(soc, &rsp, sizeof(rsp), 0, NULL, 0) ;
		if (n != sizeof(rsp))
			break ;

		esito = rsp == cmd ;

	} while (false) ;

	if (soc >= 0)
	   close(soc);

	return esito ;
}

static void gstSRV(void * v)
{
	int srvE, srvI ;
	fd_set active_fd_set, read_fd_set;
	int i;
	struct sockaddr_in clientname;
	size_t size;

	UNUSED(v) ;

	do {
		FD_ZERO(&active_fd_set) ;

		struct sockaddr_in name = { 0 } ;
		name.sin_family = AF_INET;
		name.sin_port = htons(PORT);

		// socket interno udp
		srvI = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
		if (srvI < 0)
			break ;

		name.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

		riusabile(srvI) ;

		if ( bind(srvI, (struct sockaddr *) &name, sizeof (name)) < 0)
			break ;

		FD_SET(srvI, &active_fd_set);

		/* Create the socket and set it up to accept connections. */
		srvE = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
		if (srvE < 0)
			break ;

		name.sin_addr.s_addr = htonl(INADDR_ANY);

		if ( bind(srvE, (struct sockaddr *) &name, sizeof (name)) < 0)
			break ;

		if ( listen(srvE, 1) < 0 )
			break ;

		// DA FARE: registrarsi con mDNS

		FD_SET(srvE, &active_fd_set);

		bool continua = true ;
		while (continua) {
			/* Block until input arrives on one or more active sockets. */
			read_fd_set = active_fd_set;
			if ( select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0 )
				break ;

			/* Service all the sockets with input pending. */
			for (i = 0; i < FD_SETSIZE; ++i) {
				if (FD_ISSET (i, &read_fd_set))	{
					if (i == srvI) {
						// Comando
						uint32_t cmd ;
						int nbytes = read(srvI, &cmd, sizeof(cmd));
						if (nbytes == sizeof(cmd)) {
							switch (cmd) {
							case CMD_ESCI:
								if (cln >= 0)
									close(cln) ;
								close(srvE) ;

								(void) sendto(srvI, &cmd, sizeof(cmd), 0, NULL, 0) ;

								continua = false ;
								break ;
							}
						}
					}
					else if (i == srvE) {
						/* Connection request on original socket. */
						size = sizeof (clientname);
						cln = accept (srvE,
								(struct sockaddr *) &clientname,
								&size);
						if (cln >= 0) {
							const char * ip = inet_ntoa (clientname.sin_addr) ;
							uint16_t porta = ntohs (clientname.sin_port) ;

							ESP_LOGI(TAG, "connesso %s:%hd", ip, porta) ;

							FD_SET(cln, &active_fd_set);

							cb.conn(ip, porta) ;
						}
					}
					else {
						/* Data arriving on an already-connected socket. */
						int nbytes = read (i, msg, DIM_MSG);
						if (nbytes <= 0) {
							close (i);
							FD_CLR (i, &active_fd_set);
							ESP_LOGI(TAG, "sconnesso") ;

							cb.scon() ;
						}
						else {
							/* Data read. */
							cb.msg(msg, nbytes) ;
						}
					}
				}
			}
		}

	} while (false) ;

	CHECK_IT( osOK == osThreadTerminate(NULL) ) ;
	tid = NULL ;
}

bool GST_beg(S_GST_CB * pCB)
{
	bool esito = false ;

	do {
		if (NULL == pCB)
			break ;

		if (NULL == pCB->conn)
			break ;
		if (NULL == pCB->scon)
			break ;
		if (NULL == pCB->msg)
			break ;

		if (NULL == msg) {
			msg = malloc(DIM_MSG) ;
			assert(msg) ;
			if (NULL == msg)
				break ;
		}

		osThreadDef(gstSRV, osPriorityNormal, 1, STACK) ;
		tid = osThreadCreate(osThread(gstSRV), NULL) ;
		assert(tid) ;
		if (NULL == tid)
			break ;

		cb = *pCB ;

		esito = true ;

	} while (false) ;

	return esito ;
}

void GST_end(void)
{
	while (tid) {
		(void) invia(CMD_ESCI) ;

		osDelay(100) ;
	}
}

size_t GST_tx(const void * buf, size_t count)
{
	if (cln < 0)
		return 0 ;
	else {
		int s = write(cln, buf, count) ;
		if (s < 0)
			return 0 ;
		else
			return s ;
	}
}
