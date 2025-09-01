#include "driver.hpp"
extern "C" {
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"
#include "pico/time.h"
}
#include <cstdint>

#define SPI_PORT spi1
constexpr int DISPLAY_TIMER_INTERVAL_US = -250;

namespace driver {
DisplayDriver::DisplayDriver(const PinConfig &pins) : pins_(pins) {}

void DisplayDriver::init() const {

  // initialize SPI
  spi_init(SPI_PORT, 1'000'000);
  gpio_set_function(pins_.clk, GPIO_FUNC_SPI);
  gpio_set_function(pins_.mosi, GPIO_FUNC_SPI);

  // latch RCLK as GPIO output, starting low for rising edge latch.
  gpio_init(pins_.rclk);
  gpio_set_dir(pins_.rclk, GPIO_OUT);
  gpio_put(pins_.rclk, 0);

  // refresh display at 4ms period (250us per digit)
  add_repeating_timer_us(DISPLAY_TIMER_INTERVAL_US,
                         &DisplayDriver::timer_trampoline, (void *)this,
                         &timer_);
}

void DisplayDriver::write(uint8_t pos, uint8_t value) const {
  if (pos > 3)
    return;
  patterns_[pos] = value;
}

void DisplayDriver::write_fixed(const uint8_t values[]) const {
  if (!values)
    return;
  for (uint8_t i = 0; i < 4; ++i)
    patterns_[i] = values[i];
}

void DisplayDriver::clear() const {
  for (uint8_t &p : patterns_)
    p = 0;
  sleep_ms(2);
}

void DisplayDriver::output_digit(uint8_t d) const {
  if (d >= 4)
    return;
  uint8_t buf[2] = {DigitMasks[d], patterns_[d]};
  gpio_put(pins_.rclk, 0);
  spi_write_blocking(SPI_PORT, buf, 2);
  gpio_put(pins_.rclk, 1);
}

void DisplayDriver::tick() const {
  output_digit(current_digit_);
  current_digit_ = (current_digit_ + 1) % 4;
}

bool DisplayDriver::timer_trampoline(repeating_timer_t *rt) {
  auto *self = static_cast<DisplayDriver *>(rt->user_data);
  self->tick();
  return true;
}

void DisplayDriver::stop() const { cancel_repeating_timer(&timer_); }

uint8_t DisplayDriver::char_to_pattern(char c) {
  if (c >= '0' && c <= '9')
    return SegMap[c - '0'];
  if (c >= 'A' && c <= 'F')
    return SegMap[10 + (c - 'A')];
  if (c >= 'a' && c <= 'f')
    return SegMap[10 + (c - 'a')];
  if (c == ' ')
    return 0x00;
  if (c == '-')
    return 0x40;
  return 0x00;
}

void DisplayDriver::showString(const char *str) const {
  if (!str)
    return;
  uint8_t out[4]{0, 0, 0, 0};
  int pos = 0;
  for (size_t i = 0; str[i] && pos < 4; ++i) {
    char c = str[i];
    if (c == '.') {
      if (pos > 0)
        out[pos - 1] |= SEG_DOT;
      continue;
    }
    out[pos++] = char_to_pattern(c);
  }
  write(out);
}

void DisplayDriver::formatDecimal(int value, uint8_t out[4],
                                  bool leadingZeros) const {
  bool neg = value < 0;
  int64_t v64 = value;
  uint32_t mag = (uint32_t)(neg ? -v64 : v64);
  if ((!neg && mag > 9999) || (neg && mag > 999)) {
    for (int i = 0; i < 4; ++i)
      out[i] = 0x40; // '-'
    return;
  }
  for (int i = 0; i < 4; ++i)
    out[i] = 0;
  int pos = 3;
  if (mag == 0) {
    out[3] = SegMap[0];
    if (leadingZeros)
      for (int i = 0; i < 3; ++i)
        out[i] = SegMap[0];
  } else {
    while (mag && pos >= 0) {
      out[pos] = SegMap[mag % 10];
      mag /= 10;
      --pos;
    }
    if (leadingZeros)
      while (pos >= 0)
        out[pos--] = SegMap[0];
  }
  if (neg) {
    int first = 0;
    while (first < 4 && out[first] == 0)
      ++first;
    if (first == 0) {
      for (int i = 0; i < 4; ++i)
        out[i] = 0x40;
    } else {
      out[first - 1] = 0x40;
    }
  }
}

void DisplayDriver::formatHex(int value, uint8_t out[4],
                              bool leadingZeros) const {
  if (value < 0) {
    for (int i = 0; i < 4; ++i)
      out[i] = 0x40;
    return;
  }
  uint32_t v = (uint32_t)value & 0xFFFFu;
  for (int i = 0; i < 4; ++i)
    out[i] = 0;
  for (int i = 3; i >= 0; --i) {
    out[i] = SegMap[v & 0xF];
    v >>= 4;
  }
  if (!leadingZeros) {
    int i = 0;
    bool any = false;
    for (int k = 0; k < 4; ++k)
      if (out[k] != SegMap[0]) {
        any = true;
        break;
      }
    if (any)
      while (i < 4 && out[i] == SegMap[0]) {
        out[i] = 0;
        ++i;
      }
  }
}

void DisplayDriver::applyDots(uint8_t out[4], uint8_t dotsMask) const {
  for (int i = 0; i < 4; ++i)
    if (dotsMask & (1u << i))
      out[3 - i] |= SEG_DOT;
}

void DisplayDriver::showNumber(int value) const {
  uint8_t out[4];
  formatDecimal(value, out, false);
  write(out);
}

void DisplayDriver::showNumberDec(int value, uint8_t dotsMask,
                                  bool leadingZeros) const {
  uint8_t out[4];
  formatDecimal(value, out, leadingZeros);
  applyDots(out, dotsMask);
  write(out);
}

void DisplayDriver::showNumberHex(int value, uint8_t dotsMask,
                                  bool leadingZeros) const {
  uint8_t out[4];
  formatHex(value, out, leadingZeros);
  applyDots(out, dotsMask);
  write(out);
}
const uint8_t *DisplayDriver::readBuffer() const { return patterns_; }

} // namespace driver
