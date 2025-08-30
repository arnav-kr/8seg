# Driver Library

This directory contains a reusable 7-segment display driver for the Raspberry Pi Pico.

## Files
- `CMakeLists.txt` : Builds a static library target named `driver`.
- `include/driver/driver.hpp` : Public header declaring the API.
- `driver.cpp` : Implementation.

## Usage
1. In the top-level `CMakeLists.txt`, add:
```
add_subdirectory(lib/Driver)
target_link_libraries(8seg PRIVATE driver)
```
2. Include the header in your application source:
```
#include <driver/driver.hpp>
```
3. Instantiate and use:
```
driver::PinConfig pins{ .segment_pins = { 2,3,4,5,6,7,8,9 } }; // adjust
driver::SevenSeg disp(pins, { .on_level = true }); // true for common cathode
disp.init();
for (uint8_t v = 0; v < 16; ++v) {
    disp.show_hex(v);
    sleep_ms(500);
}
```

## Notes
- Adjust GPIO numbers to your wiring.
- For common anode, pass `{ .on_level = false }` and the logic will invert.
- Extend later for multiplexed multi-digit displays (add digit control pins and a refresh timer/IRQ).
