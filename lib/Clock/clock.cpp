#include "clock.hpp"
#include <string.h>
#include <time.h>
extern "C" {
#include "hardware/rtc.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "pico/util/datetime.h"

#include "lwip/dns.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
}

static void ntp_send_request(NTP_REQUEST_T *state) {
  cyw43_arch_lwip_begin();
  struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, NTP_MSG_LEN, PBUF_RAM);
  uint8_t *req = (uint8_t *)p->payload;
  memset(req, 0, NTP_MSG_LEN);
  req[0] = 0x1b; // LI, Version, Mode
  udp_sendto(state->ntp_pcb, p, &state->ntp_server_address, NTP_PORT);
  pbuf_free(p);
  cyw43_arch_lwip_end();
}

static void ntp_recv_callback(void *arg, struct udp_pcb *pcb, struct pbuf *p,
                              const ip_addr_t *addr, u16_t port) {
  NTP_REQUEST_T *state = (NTP_REQUEST_T *)arg;
  uint8_t mode = pbuf_get_at(p, 0) & 0x7;
  uint8_t stratum = pbuf_get_at(p, 1);

  if (ip_addr_cmp(addr, &state->ntp_server_address) && port == NTP_PORT &&
      p->tot_len == NTP_MSG_LEN && mode == 0x4 && stratum != 0) {
    uint8_t seconds_buf[4] = {0};
    pbuf_copy_partial(p, seconds_buf, sizeof(seconds_buf), 40);
    uint32_t seconds_since_1900 = seconds_buf[0] << 24 | seconds_buf[1] << 16 |
                                  seconds_buf[2] << 8 | seconds_buf[3];
    uint32_t seconds_since_1970 = seconds_since_1900 - NTP_DELTA;
    state->complete_time = seconds_since_1970;
  }
  pbuf_free(p);
  state->request_complete = true;
}

static void ntp_dns_found_callback(const char *hostname,
                                   const ip_addr_t *ipaddr, void *arg) {
  NTP_REQUEST_T *state = (NTP_REQUEST_T *)arg;
  if (ipaddr) {
    state->ntp_server_address = *ipaddr;
    ntp_send_request(state);
  } else {
    state->request_complete = true;
  }
}

bool sync_internal_clock_from_ntp() {
  rtc_init();

  NTP_REQUEST_T *state = (NTP_REQUEST_T *)calloc(1, sizeof(NTP_REQUEST_T));
  if (!state) {
    return false;
  }

  state->ntp_pcb = udp_new_ip_type(IPADDR_TYPE_ANY);
  if (!state->ntp_pcb) {
    free(state);
    return false;
  }
  udp_recv(state->ntp_pcb, ntp_recv_callback, state);

  cyw43_arch_lwip_begin();
  int err = dns_gethostbyname(NTP_SERVER, &state->ntp_server_address,
                              ntp_dns_found_callback, state);
  cyw43_arch_lwip_end();

  if (err == ERR_OK) {
    ntp_send_request(state);
  } else if (err == ERR_INPROGRESS) {
  } else {
    state->request_complete = true;
  }

  absolute_time_t timeout_time = make_timeout_time_ms(NTP_TIMEOUT_MS);
  while (!state->request_complete && !time_reached(timeout_time)) {
    sleep_ms(10);
  }

  bool success = state->request_complete && (state->complete_time > 0);
  if (success) {
    struct tm *utc = gmtime(&state->complete_time);
    datetime_t dt = {.year = (int16_t)(utc->tm_year + 1900),
                     .month = (int8_t)(utc->tm_mon + 1),
                     .day = (int8_t)utc->tm_mday,
                     .dotw = (int8_t)utc->tm_wday,
                     .hour = (int8_t)utc->tm_hour,
                     .min = (int8_t)utc->tm_min,
                     .sec = (int8_t)utc->tm_sec};
    rtc_set_datetime(&dt);
    printf("pico's RTC set successfully to UTC.\n");
  } else {
    printf("pico's NTP sync failed. No valid response received.\n");
  }

  udp_remove(state->ntp_pcb);
  free(state);
  return success;
}

static const int days_in_month[] = {0,  31, 28, 31, 30, 31, 30,
                                    31, 31, 30, 31, 30, 31};
static bool is_leap(int year) {
  return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

bool get_local_datetime(datetime_t *dt) {
  if (!rtc_get_datetime(dt)) {
    return false;
  }

  printf("[DEBUG] RTC datetime: %04d-%02d-%02d %02d:%02d:%02d\n", dt->year,
         dt->month, dt->day, dt->hour, dt->min, dt->sec);
  dt->min += TIMEZONE_OFFSET_MINUTES;
  while (dt->min >= 60) {
    dt->min -= 60;
    dt->hour += 1;
  }

  dt->hour += TIMEZONE_OFFSET_HOURS;
  while (dt->hour >= 24) {
    dt->hour -= 24;
    dt->day += 1;
    dt->dotw = (dt->dotw + 1) % 7;
  }

  int month_days = days_in_month[dt->month];
  if (dt->month == 2 && is_leap(dt->year)) {
    month_days = 29;
  }

  while (dt->day > month_days) {
    dt->day -= month_days;
    dt->month += 1;
    if (dt->month > 12) {
      dt->month = 1;
      dt->year += 1;
    }
    month_days = days_in_month[dt->month];
    if (dt->month == 2 && is_leap(dt->year)) {
      month_days = 29;
    }
  }

  return true;
}