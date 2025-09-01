#include "wifi.hpp"
extern "C" {
#include "cyw43_country.h"
#include "pico/cyw43_arch.h"
#include "pico/platform/compiler.h"
}
#include <cstdio>

namespace wifi {

static bool connecting = false;
static const char *current_ssid = nullptr;
static const char *current_password = nullptr;

void init() {
  static bool initialized = false;
  if (!initialized) {
    if (cyw43_arch_init_with_country(CYW43_COUNTRY_INDIA)) {
      printf("failed to initialise\n");
      return;
    }
    printf("initialised\n");
    cyw43_arch_enable_sta_mode();
    initialized = true;
  }
}

void start_connect(const char *ssid, const char *password) {
  current_ssid = ssid;
  current_password = password;
  connecting = true;
  cyw43_arch_wifi_connect_async(ssid, password, CYW43_AUTH_WPA2_AES_PSK);
}

bool is_connecting() {
  if (!connecting)
    return false;
  int link_state = cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA);
  if (link_state == CYW43_LINK_UP || link_state == CYW43_LINK_DOWN) {
    connecting = false;
  }
  return connecting;
}

bool is_connected() {
  int link_state = cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA);
  return link_state == CYW43_LINK_UP;
}

} // namespace wifi
