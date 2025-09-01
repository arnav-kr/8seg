#include "clock.hpp"
#include "driver.hpp"
#include "wifi.hpp"
#include <cstdio>

extern "C" {
#include "pico/stdlib.h"
#include "pico/time.h"
#include "pico/platform/common.h"
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
      {0x01, 0x02, 0x04, 0x08},
      {0x02, 0x04, 0x08, 0x01},
      {0x04, 0x08, 0x01, 0x02},
      {0x08, 0x01, 0x02, 0x04}};

  int frame = 0;
  while (!wifi::connect("Arnav", "ThereIsNoPassword")) {
    display.write(spinner_frames[frame]);
    frame = (frame + 1) % 4;
    sleep_ms(200);
  }

  sync_internal_clock_from_ntp();
  display.write({0x39, 0x5c, 0x54, 0x54}); // Conn
  sleep_ms(2000);

  datetime_t now_local;

  while (true) {
    if (get_local_datetime(&now_local)) {
      display.showNumberDec(now_local.hour * 100 + now_local.min, 0b0100);
      sleep_ms(1000);
    } else {
      display.write({0x79, 0x50, 0x50, 0x00}); // Err
    }
  }
}
