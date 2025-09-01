#pragma once
#include <stdint.h>

struct Config {
  char ssid[32];
  char password[32];
  int mode;
};

bool fs_load_config(Config &cfg);
bool fs_save_config(const Config &cfg);
