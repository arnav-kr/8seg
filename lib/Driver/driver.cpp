#include "driver/driver.hpp"

#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include <cstdint>

#define SPI_PORT spi1
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

  // 1000 / 250 = 4
  add_repeating_timer_us(-250, &DisplayDriver::timer_trampoline, (void *)this,
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
}

void DisplayDriver::output_digit(uint8_t d) const {
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

} // namespace driver
