// Example application using the driver library to cycle hex digits on a 7-seg
// display.
#include "pico/stdlib.h"
#include <driver/driver.hpp>
#include <pico/time.h>
#include <stdio.h>

int main() {
  stdio_init_all();

  driver::PinConfig pins = {
      .rclk = 9,
      .clk = 10,
      .mosi = 11,
  };
  driver::DisplayDriver display(pins);
  display.init();
  //we'll test the functionality one by one
  display.showNumber(1234);
  sleep_ms(1000);
  display.showNumberDec(5678, 0b0001);
  sleep_ms(1000);
  display.showNumberHex(0xABCD, 0b0010);
  sleep_ms(1000);
  display.showString("EF53");
  sleep_ms(1000);
  display.clear();
}
