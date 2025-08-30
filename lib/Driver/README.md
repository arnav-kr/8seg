simple driver for a 4‑digit 8‑segment (with dot) display, compatible with  TM1637TinyDisplay animation format <a href="https://jasonacox.github.io/TM1637TinyDisplay/examples/7-segment-animator.html"><sup>^0</sup></a>

# 8SEG Driver

Simple driver for a 4‑digit 7‑segment (with dot) display, compatible with TM1637TinyDisplay animation format <a href="https://jasonacox.github.io/TM1637TinyDisplay/examples/7-segment-animator.html"><sup>^0</sup></a>

## Features
- Automatic multiplexing (timer-driven)
- TM1637-style helpers: showNumber, showNumberDec, showNumberHex, showString
- Negative number and dot support
- Overflow marker ("----") for out-of-range values


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
```

## Patterns
Each digit pattern byte = `HGFEDCBA` (bit 7 = optional dot, bits 6..0 = segments)
Use the `SegMap` array to get the segment patterns for digits 0-9 and A-F (index 0-15).

## API
```cpp
void write(uint8_t pos, uint8_t pattern);   // one char
template<size_t N> void write(const uint8_t (&p)[N]); // write 4 segments
void showNumber(int value);                 // decimal, blanks leading zeros, supports negatives
void showNumberDec(int value, uint8_t dotsMask, bool leadingZeros=false); // decimal with custom dotMask
void showNumberHex(int value, uint8_t dotsMask=0, bool leadingZeros=false); // hexadecimal with custom dotMask
void showString(const char* str);           // up to 4 chars, dot attaches to previous
void clear(); // clear display
void stop(); // stop auto refresh
```

### Dot Mask
- For `showNumberDec`/`showNumberHex`, dotsMask bit0=units, bit1=tens, bit2=hundreds, bit3=thousands.
- Example: `dotsMask=0b0101` lights dots on units and hundreds.

### Negative and Overflow
- showNumber supports negative values (e.g. -42 shows " -42").
- If value is too large to fit (e.g. >9999 or <-999), display shows "----".
- Hex: negative values show "----"; only 0..0xFFFF supported.

driver keeps a 4‑byte shadow buffer. every 250 µs, the next digit is sent to the display.
## Implementation
Driver keeps a 4‑byte shadow buffer. Every 250 µs, the next digit is sent to the display (multiplexing).