#pragma once
extern "C" {
#include "pico/time.h"
}
#include <cassert>
#include <stddef.h>
#include <stdint.h>

namespace driver {
inline constexpr uint8_t DIGIT_KILOBIT = 0xFE;  // KILOBIT
inline constexpr uint8_t DIGIT_HUNDREDS = 0xFD; // HUNDREDS
inline constexpr uint8_t DIGIT_TENS = 0xFB;     // TENS
inline constexpr uint8_t DIGIT_UNITS = 0xF7;    // UNITS
inline constexpr uint8_t DigitMasks[4] = {DIGIT_KILOBIT, DIGIT_HUNDREDS,
                                          DIGIT_TENS, DIGIT_UNITS};
inline constexpr uint8_t SEG_DOT = 0x80; // Dot (decimal point)
inline constexpr uint8_t SegMap[16] = {
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111, // 9
    0b01110111, // A
    0b01111100, // b
    0b00111001, // C
    0b01011110, // d
    0b01111001, // E
    0b01110001  // F
};

struct PinConfig {
  uint8_t rclk;
  uint8_t clk;
  uint8_t mosi;
};

class DisplayDriver {
public:
  DisplayDriver(const PinConfig &pins);

  // Initialize SPI and GPIO
  void init() const;

  void write(uint8_t pos, uint8_t value) const;

  template <size_t N> void write(const uint8_t (&values)[N]) const {
    static_assert(N == 4, "DisplayDriver::write requires array length 4");
    write_fixed(values);
  }

  static uint8_t char_to_pattern(char c);

  void showString(const char *str) const;

  void showNumber(int value) const;

  void showNumberDec(int value, uint8_t dotsMask,
                     bool leadingZeros = false) const;

  void showNumberHex(int value, uint8_t dotsMask = 0,
                     bool leadingZeros = false) const;

  const uint8_t *readBuffer() const;

  void clear() const;

  void stop() const;

private:
  void write_fixed(const uint8_t values[]) const;

  void output_digit(uint8_t digit) const;

  void tick() const;

  static bool timer_trampoline(repeating_timer_t *rt);

  void formatDecimal(int value, uint8_t out[4], bool leadingZeros) const;

  void formatHex(int value, uint8_t out[4], bool leadingZeros) const;

  void applyDots(uint8_t out[4], uint8_t dotsMask) const;

  PinConfig pins_;

  mutable uint8_t patterns_[4] = {0, 0, 0, 0};

  mutable uint8_t current_digit_ = 0;

  mutable repeating_timer_t timer_{};
};

} // namespace driver
