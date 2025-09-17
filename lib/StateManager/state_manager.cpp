#include "state_manager.hpp"
#include "config.hpp"

#include <stdio.h>
#include <string.h>

namespace state {

StateManager::StateManager() : sorted_count_(0) {
  memset(sorted_deadlines_, 0, sizeof(sorted_deadlines_));
}

void StateManager::setConfig(const Config &cfg) {
  config_ = cfg;
  sortDeadlines();
}

DisplayMode StateManager::getDisplayMode() const {
  if (config_.mode == MODE_TIME_HHMM || config_.mode == MODE_TIME_MMSS) {
    return DISPLAY_TIME;
  } else {
    return DISPLAY_DEADLINE_COUNTDOWN;
  }
}

bool StateManager::getNextDeadline(DeadlineInfo &info,
                                   uint32_t current_time) const {
  if (sorted_count_ == 0)
    return false;
  for (int i = 0; i < sorted_count_; ++i) {
    if (sorted_deadlines_[i] > current_time) {
      info.timestamp = sorted_deadlines_[i];
      uint32_t diff = info.timestamp - current_time;
      info.days = diff / 86400;
      diff %= 86400;
      info.hours = diff / 3600;
      diff %= 3600;
      info.minutes = diff / 60;
      return true;
    }
  }
  return false; // no future deadlines
}

void StateManager::sortDeadlines() {
  sorted_count_ = config_.deadline_count;
  memcpy(sorted_deadlines_, config_.deadlines,
         sizeof(uint32_t) * sorted_count_);
  for (int i = 0; i < sorted_count_ - 1; ++i) {
    for (int j = 0; j < sorted_count_ - i - 1; ++j) {
      if (sorted_deadlines_[j] > sorted_deadlines_[j + 1]) {
        uint32_t temp = sorted_deadlines_[j];
        sorted_deadlines_[j] = sorted_deadlines_[j + 1];
        sorted_deadlines_[j + 1] = temp;
      }
    }
  }
}

} // namespace state