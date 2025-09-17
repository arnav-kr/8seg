#pragma once
#include "fs.hpp"
#include <stdint.h>

namespace state {

enum DisplayMode { DISPLAY_TIME, DISPLAY_DEADLINE_COUNTDOWN };

struct DeadlineInfo {
  uint32_t timestamp;
  int days;
  int hours;
  int minutes;
};

class StateManager {
public:
  StateManager();

  void setConfig(const Config &cfg);
  DisplayMode getDisplayMode() const;
  bool getNextDeadline(DeadlineInfo &info, uint32_t current_time) const;
  void sortDeadlines();

private:
  Config config_;
  mutable uint32_t sorted_deadlines_[10];
  mutable int sorted_count_;
};

} // namespace state