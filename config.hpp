// config.hpp: Project-wide default configuration
#pragma once
#include "fs.hpp"

// modes: time hh:mm, time2: mm:ss, countdown_days, countdown_hours,
// countdown_minutes, animation, etc.
enum Mode {
  MODE_TIME_HHMM,
  MODE_TIME_MMSS,
  MODE_COUNTDOWN_DAYS,
  MODE_COUNTDOWN_HOURS,
  MODE_COUNTDOWN_MINUTES,
  MODE_ANIMATION
};

constexpr Config DEFAULT_CONFIG = {
    .ssid = "Arnav",
    .password = "ThereIsNoPassword",
    .mode = MODE_TIME_HHMM,
    .deadlines = {1758220200, 1759150600, 0, 0, 0, 0, 0, 0, 0, 0},
    .deadline_count = 2};
