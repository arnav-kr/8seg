#include "async_timer.hpp"

namespace async {

Timer::Timer(uint32_t interval_ms)
    : interval_ms_(interval_ms), enabled_(false) {}

void Timer::start() {
  next_run_ = make_timeout_time_ms(interval_ms_);
  enabled_ = true;
}

void Timer::start(uint32_t delay_ms) {
  interval_ms_ = delay_ms;
  start();
}

bool Timer::ready() {
  if (!enabled_ || !time_reached(next_run_)) {
    return false;
  }
  next_run_ = make_timeout_time_ms(interval_ms_);
  return true;
}

void Timer::stop() { enabled_ = false; }

bool Timer::is_running() const { return enabled_; }

} // namespace async