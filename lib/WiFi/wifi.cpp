#include "wifi.hpp"
extern "C" {
#include "cyw43_country.h"
#include "pico/cyw43_arch.h"
#include "pico/platform/compiler.h"
}
#include <cstdio>

namespace wifi {
bool connect(const char *ssid, const char *password) {
  // initialize WiFi
  if (cyw43_arch_init_with_country(CYW43_COUNTRY_INDIA)) {
    printf("failed to initialise\n");
    return false;
  }
  printf("initialised\n");

  // enable station mode
  cyw43_arch_enable_sta_mode();

  for (int attempt = 1; attempt <= MAX_RETRIES; ++attempt) {
    printf("WiFi connect attempt %d/%d...\n", attempt, MAX_RETRIES);

    int result = cyw43_arch_wifi_connect_timeout_ms(
        ssid, password, CYW43_AUTH_WPA2_AES_PSK, WIFI_CONNECT_TIMEOUT_MS);

    if (result == 0) {
      // Wait for link up (DHCP IP assigned)
      int wait_secs = 0;
      while (wait_secs < 10) {
        int link_state = cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA);

        if (link_state == CYW43_LINK_UP) {
          printf("WiFi link is up\n");
          uint8_t *ip = (uint8_t *)&(cyw43_state.netif[0].ip_addr.addr);
          printf("IP address %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
          return true;
        }

        wait_secs++;

        if (wait_secs == 10) {
          printf("Can not get IP address\n");
          break;
        }
        sleep_ms(1000);
      }
      printf("WiFi connected but link did not come up\n");
    } else {
      printf("failed to connect (error %d)\n", result);
      sleep_ms(1000);
    }
  }
  printf("All WiFi connection attempts failed.\n");
  return false;
}

bool is_connected() {
  int link_state = cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA);
  return link_state == CYW43_LINK_UP;
}
} // namespace wifi
