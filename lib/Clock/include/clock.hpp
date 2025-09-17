#pragma once
extern "C" {
#include "lwip/ip_addr.h"
#include "pico/util/datetime.h"
}

// +5:30 IST timezone offset
#define TIMEZONE_OFFSET_HOURS 5
#define TIMEZONE_OFFSET_MINUTES 30

#define NTP_SERVER "pool.ntp.org"
#define NTP_MSG_LEN 48
#define NTP_PORT 123
#define NTP_DELTA 2208988800U // Seconds between 1 Jan 1900 and 1 Jan 1970
#define NTP_TIMEOUT_MS 10000U // 10-second timeout

typedef struct NTP_REQUEST_T_ {
  ip_addr_t ntp_server_address;
  struct udp_pcb *ntp_pcb;
  time_t complete_time; // unix timestamp
  bool request_complete;
} NTP_REQUEST_T;

// Simple blocking NTP sync API
bool sync_internal_clock_from_ntp();
bool get_local_datetime(datetime_t *dt);

// Internal helper functions
static void ntp_send_request(NTP_REQUEST_T *state);
static void ntp_recv_callback(void *arg, struct udp_pcb *pcb, struct pbuf *p,
                              const ip_addr_t *addr, u16_t port);
static void ntp_dns_found_callback(const char *hostname,
                                   const ip_addr_t *ipaddr, void *arg);