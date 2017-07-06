#include <c_types.h>
#include <user_interface.h>
#include <espconn.h>
#include <osapi.h>
#include <mem.h>
#include <time.h>

#include "ntp.h"
#include "user_config.h"
#include "driver/uart.h"

// list of major public servers http://tf.nist.gov/tf-cgi/servers.cgi

static os_timer_t ntp_timeout;
static struct espconn *pCon = 0;

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

static uint8 ICACHE_FLASH_ATTR calc_checksum(char *pdata, unsigned short len) {
	uint8 checksum = 0;
	int i;
	
	for (i = 0; i < len; i++) {
		checksum ^= pdata[i];
	}
	return checksum;
}

static void ICACHE_FLASH_ATTR ntp_udp_recv(void *arg, char *pdata, unsigned short len) {

	struct tm *dt;
	time_t timestamp;
	ntp_t *ntp;
	uint8 checksum;

	os_timer_disarm(&ntp_timeout);

	// extract ntp time
	ntp = (ntp_t*)pdata;
	timestamp = ntp->trans_time[0] << 24 | ntp->trans_time[1] << 16 |ntp->trans_time[2] << 8 | ntp->trans_time[3];
	// convert to unix time
	timestamp -= 2208988800UL;
	// create tm struct
	dt = gmtime(&timestamp);

	// do something with it, like setting an rtc
	//ds1307_setTime(dt);
	// or just print it out
	char gps_raw_data[31];
	char gps_formatted_data[37];
	os_sprintf(gps_raw_data, "GPRMC,%02d%02d%02d,A,,,,,,,%02d%02d%02d,,\0", dt->tm_hour, dt->tm_min, dt->tm_sec,
			dt->tm_mday, dt->tm_mon+1, dt->tm_year-100); //unix time starts in 1900
	checksum = calc_checksum(gps_raw_data, os_strlen(gps_raw_data));
	ets_uart_printf("\r\n\r\n$%s*%02x\r\n\r\n", gps_raw_data, checksum);
	//os_sprintf(gps_formatted_data, "$%s*%02x\r\n", gps_raw_data, checksum);
	//uart1_write_str(gps_formatted_data);

	// clean up connection
	if (pCon) {
		espconn_delete(pCon);
		os_free(pCon->proto.udp);
		os_free(pCon);
		pCon = 0;
	}
#ifdef PLATFORM_DEBUG
	ets_uart_printf("WiFi will now be disconnected\r\n");
#endif
//	wifi_disconnect();
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
