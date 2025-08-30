# 8SEG Driver

simple driver for a 4‑digit 8‑segment (with dot) display, compatible with  TM1637TinyDisplay animation format <a href="https://jasonacox.github.io/TM1637TinyDisplay/examples/7-segment-animator.html"><sup>^0</sup></a>

## Pins
- rclk (latch)
- clk  (SPI SCK)
- mosi (SPI MOSI)

## Basic Use
CMake:
```
add_subdirectory(lib/Driver)
target_link_libraries(8seg PRIVATE driver)
```
Code:
```cpp
#include <driver/driver.hpp>
#include "pico/stdlib.h"

int main() {
    stdio_init_all();
    driver::PinConfig pins{ .rclk=9, .clk=10, .mosi=11 }; // adjust
    driver::DisplayDriver disp(pins);
    disp.init();

    uint8_t hello[4] = {
        driver::SegMap[1], // "1"
        driver::SegMap[2], // "2"
        driver::SegMap[3], // "3"
        driver::SegMap[4]  // "4"
    };
    disp.write(hello);
    while (true) tight_loop_contents();
}
```

## Patterns
each digit pattern byte = `HGFEDCBA`

(bit 7 = optional dot, bits 6..0 = segments)

use the `SegMap` array to get the segment patterns for digits 0-9 and A-F. (index 0-15)
## API
```cpp
void write(uint8_t pos, uint8_t pattern);   // one digit (0..3)
template<size_t N> void write(const uint8_t (&p)[N]); // write 4 segments
void clear(); // clear display
void stop(); // stop auto refresh
```

## Implementation
driver keeps a 4‑byte shadow buffer. every 250 µs, the next digit is sent to the display.