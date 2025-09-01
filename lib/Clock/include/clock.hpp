#pragma once
extern "C" {
#include "lwip/ip_addr.h"
#include "pico/util/datetime.h"
}
#include <cassert>

// +5:30
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

static void ntp_send_request(NTP_REQUEST_T *state);
static void ntp_recv_callback(const char *response, size_t len);
static void ntp_dns_found_callback(const char *hostname,
                                   const ip_addr_t *ipaddr, void *arg);
bool sync_internal_clock_from_ntp();
bool get_local_datetime(datetime_t *dt);