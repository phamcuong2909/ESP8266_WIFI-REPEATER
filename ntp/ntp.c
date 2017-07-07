#include <c_types.h>
#include <user_interface.h>
#include <espconn.h>
#include <osapi.h>
#include <mem.h>
#include <time.h>
#include <sys/time.h>

#include "ntp.h"
#include "user_config.h"
#include "driver/uart.h"

#define OFFSET 2208988800ULL

static os_timer_t ntp_timeout;
static struct espconn *pCon = 0;


void ICACHE_FLASH_ATTR ntp_to_tv(uint8_t ntp[8], struct timeval *tv)
{
uint64_t aux = 0;

	tv->tv_sec = ntohl(*(uint32_t*)ntp) - OFFSET;

	aux = ntohl(*(uint32_t*)&ntp[4]);

	// aux is the NTP fraction (0..2^32-1)
	aux *= 1000000; // multiply by 1e6 
	aux >>= 32;     // and divide by 2^32
	tv->tv_usec = (uint32_t)aux;
}


static void ICACHE_FLASH_ATTR ntp_udp_timeout(void *arg) {
	
	os_timer_disarm(&ntp_timeout);
	os_printf("ntp timout\r\n", 12);

	// clean up connection
	if (pCon) {
		espconn_delete(pCon);
		os_free(pCon->proto.udp);
		os_free(pCon);
		pCon = 0;
	}
}


static void ICACHE_FLASH_ATTR ntp_udp_recv(void *arg, char *pdata, unsigned short len) {
	ntp_t *ntp;
	struct timeval tv;

	os_timer_disarm(&ntp_timeout);

	// extract ntp time
	ntp = (ntp_t*)pdata;

	ntp_to_tv(ntp->trans_time, &tv);

	os_printf("s: %d, us: %d\r\n", tv.tv_sec, tv.tv_usec);

	// clean up connection
	if (pCon) {
		espconn_delete(pCon);
		os_free(pCon->proto.udp);
		os_free(pCon);
		pCon = 0;
	}
}

void ICACHE_FLASH_ATTR ntp_get_time(ip_addr_t *ntp_server) {

	ntp_t ntp;

	// set up the udp "connection"
	pCon = (struct espconn*)os_zalloc(sizeof(struct espconn));
	pCon->type = ESPCONN_UDP;
	pCon->state = ESPCONN_NONE;
	pCon->proto.udp = (esp_udp*)os_zalloc(sizeof(esp_udp));
	pCon->proto.udp->local_port = espconn_port();
	pCon->proto.udp->remote_port = 123;
	os_memcpy(pCon->proto.udp->remote_ip, &(ntp_server->addr), 4);

	// create a really simple ntp request packet
	os_memset(&ntp, 0, sizeof(ntp_t));
	ntp.options = 0b00011011; // leap = 0, version = 3, mode = 3 (client)

	// set timeout timer
	os_timer_disarm(&ntp_timeout);
	os_timer_setfn(&ntp_timeout, (os_timer_func_t*)ntp_udp_timeout, pCon);
	os_timer_arm(&ntp_timeout, NTP_TIMEOUT_MS, 0);

	// send the ntp request
	espconn_create(pCon);
	espconn_regist_recvcb(pCon, ntp_udp_recv);
	espconn_sent(pCon, (uint8*)&ntp, sizeof(ntp_t));
}
