#include "clock.hpp"
#include "driver.hpp"
#include "fs.hpp"
#include "wifi.hpp"
#include <cstdio>

extern "C" {
#include "pico/platform/common.h"
#include "pico/stdlib.h"
#include "pico/time.h"
}

int main() {
  stdio_init_all();

  driver::PinConfig pins = {
      .rclk = 9,
      .clk = 10,
      .mosi = 11,
  };
  driver::DisplayDriver display(pins);
  display.init();

  const uint8_t spinner_frames[][4] = {
      {0x01, 0x00, 0x00, 0x00}, {0x00, 0x01, 0x00, 0x00},
      {0x00, 0x00, 0x01, 0x00}, {0x00, 0x00, 0x00, 0x01},
      {0x00, 0x00, 0x00, 0x02}, {0x00, 0x00, 0x00, 0x04},
      {0x00, 0x00, 0x00, 0x08}, {0x00, 0x00, 0x08, 0x00},
      {0x00, 0x08, 0x00, 0x00}, {0x08, 0x00, 0x00, 0x00},
      {0x10, 0x00, 0x00, 0x00}, {0x20, 0x00, 0x00, 0x00},
  };

  int frame = 0;
  Config cfg;
  fs_load_config(cfg);
  int reconnect_delay = 0;
  wifi::init();
  wifi::start_connect(cfg.ssid, cfg.password);

  bool ntp_synced = false;
  datetime_t now_local;

  while (true) {
    if (!wifi::is_connected()) {
      if (reconnect_delay == 0) {
        wifi::start_connect(cfg.ssid, cfg.password);
        reconnect_delay = 50; // ~10 seconds (50 * 200ms)
      } else {
        reconnect_delay--;
      }
      display.write(spinner_frames[frame]);
      frame = (frame + 1) % 12;
      sleep_ms(200);
      continue;
    }

    if (!ntp_synced) {
      sync_internal_clock_from_ntp();
      display.write({0x39, 0x5c, 0x54, 0x54}); // Conn
      sleep_ms(2000);
      ntp_synced = true;
    }

    if (get_local_datetime(&now_local)) {
      display.showNumberDec(now_local.hour * 100 + now_local.min, 0b0101);
      sleep_ms(1000);
    } else {
      display.write({0x79, 0x50, 0x50, 0x00}); // Err
    }
  }
}
