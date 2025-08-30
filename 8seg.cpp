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

  // Show "1234"
  display.showNumber(1234);
  sleep_ms(1000);

  // Show negative, overflow, and hex
  display.showNumber(-42); // shows " -42"
  sleep_ms(1000);

  display.showNumber(12345); // shows "----" (overflow)
  sleep_ms(1000);

  display.showNumberHex(0x1AF); // shows " 1AF"
  sleep_ms(1000);

  // Show number with dots (bit0=units, bit3=thousands)
  display.showNumberDec(1234, 0b0101); // dots on units and hundreds
  sleep_ms(1000);

  // Show string (first 4 chars, dot attaches to previous digit)
  display.showString("Ab.f");
  sleep_ms(1000);

  // Blank display
  display.clear();
}
