#pragma once

extern "C" {
#include "pico/time.h"
}

namespace async {

class Timer {
private:
  absolute_time_t next_run_;
  uint32_t interval_ms_;
  bool enabled_;

public:
  Timer(uint32_t interval_ms = 1000);

  void start();
  void start(uint32_t delay_ms);
  bool ready();
  void stop();
  bool is_running() const;
};

} // namespace async