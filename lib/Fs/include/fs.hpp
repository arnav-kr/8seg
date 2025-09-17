#pragma once
#include <stdint.h>

struct Config {
  char ssid[32];
  char password[32];
  int mode;
  uint32_t deadlines[10];
  int deadline_count;
};

bool fs_load_config(Config &cfg);
bool fs_save_config(const Config &cfg);
