// Example application using the driver library to cycle hex digits on a 7-seg
// display.
#include "pico/stdlib.h"
#include <driver/driver.hpp>
#include <pico/time.h>
#include <stdio.h>

int main() {
  stdio_init_all();

  /* Animation Data - HGFEDCBA Map */
  const uint8_t ANIMATION[11][4] = {
      {0x00, 0x00, 0x00, 0x00}, // Frame 0
      {0x02, 0x04, 0x02, 0x04}, // Frame 1
      {0x04, 0x02, 0x04, 0x02}, // Frame 2
      {0x80, 0x00, 0x00, 0x00}, // Frame 3
      {0x80, 0x80, 0x00, 0x00}, // Frame 4
      {0x80, 0x80, 0x80, 0x00}, // Frame 5
      {0x80, 0x80, 0x80, 0x80}, // Frame 6
      {0x80, 0x00, 0x00, 0x00}, // Frame 7
      {0x00, 0x80, 0x00, 0x00}, // Frame 8
      {0x00, 0x00, 0x80, 0x00}, // Frame 9
      {0x00, 0x00, 0x00, 0x80}  // Frame 10
  };

  driver::PinConfig pins = {
      .rclk = 9,
      .clk = 10,
      .mosi = 11,
  };
  driver::DisplayDriver display(pins);
  display.init();
  int frame = 0;
  while (true) {
    display.write(ANIMATION[frame]);
    frame = (frame + 1) % 11;
    sleep_ms(500);
  }
}
