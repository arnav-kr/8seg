#include "async_timer.hpp"
#include "clock.hpp"
#include "driver.hpp"
#include "fs.hpp"
#include "wifi.hpp"
#include <cstdio>

extern "C" {
#include "pico/stdlib.h"
#include "pico/time.h"
}

int main() {
  stdio_init_all();

  driver::PinConfig pins = {.rclk = 9, .clk = 10, .mosi = 11};
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

  Config cfg;
  fs_load_config(cfg);
  wifi::init();
  wifi::start_connect(cfg.ssid, cfg.password);

  // states
  int spinner_frame = 0;
  int reconnect_delay = 0;
  bool ntp_synced = false;
  bool initial_connection = true;
  datetime_t current_time;

  // timers
  async::Timer wifi_check_timer(200);
  async::Timer display_timer(1000);
  async::Timer spinner_timer(200);

  wifi_check_timer.start();
  display_timer.start();
  spinner_timer.start();

  printf("Starting 8seg display controller\n");

  while (true) {
    if (wifi_check_timer.ready()) {
      if (!wifi::is_connected()) {
        if (reconnect_delay == 0) {
          wifi::start_connect(cfg.ssid, cfg.password);
          reconnect_delay = 50; // ~10 seconds
        } else {
          reconnect_delay--;
        }
      } else {
        const char *ip = wifi::get_ip();
        if (ip && initial_connection) {
          printf("WiFi connected! IP: %s\n", ip);
        }

        if (!ntp_synced) {
          sync_internal_clock_from_ntp();
          ntp_synced = true;
          initial_connection = false;
        }
      }
    }

    if (!ntp_synced && spinner_timer.ready()) {
      display.write(spinner_frames[spinner_frame]);
      spinner_frame = (spinner_frame + 1) % 12;
    }

    if (ntp_synced && display_timer.ready()) {
      if (get_local_datetime(&current_time)) {
        uint8_t base_dots = 0b0100;
        uint8_t wifi_status_dot = 0b0000;
        if (wifi::is_connected())
          wifi_status_dot = 0b0001;
        else
          wifi_status_dot = 0b0000;

        uint8_t final_dots = base_dots | wifi_status_dot;

        display.showNumberDec(current_time.hour * 100 + current_time.min,
                              final_dots, true);
      } else {
        // Err
        display.write({0x79, 0x50, 0x50, 0x00});
      }
    }
    sleep_ms(1);
  }
}