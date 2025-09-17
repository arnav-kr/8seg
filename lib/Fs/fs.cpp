#include "include/fs.hpp"
extern "C" {
#include "hardware/flash.h"
#include "hardware/sync.h"
}
#include "config.hpp"
#include <stdio.h>
#include <string.h>

#define FLASH_TARGET_OFFSET (1024 * 1024 - 4096) // Last 4KB of 2MB flash
#define CONFIG_MAGIC 0x42C0FFEE

struct FlashConfig {
  uint32_t magic;
  Config config;
};

static FlashConfig cached_config;

bool fs_load_config(Config &cfg) {
  const FlashConfig *flash_cfg =
      (const FlashConfig *)(XIP_BASE + FLASH_TARGET_OFFSET);
  if (flash_cfg->magic != CONFIG_MAGIC) {
    cfg = DEFAULT_CONFIG;
    return false;
  }
  memcpy(&cfg, &flash_cfg->config, sizeof(Config));
  return true;
}

bool fs_save_config(const Config &cfg) {
  FlashConfig to_write = {CONFIG_MAGIC, {0}};
  memcpy(&to_write.config, &cfg, sizeof(Config));
  uint32_t ints = save_and_disable_interrupts();
  flash_range_erase(FLASH_TARGET_OFFSET, 4096);
  flash_range_program(FLASH_TARGET_OFFSET, (const uint8_t *)&to_write,
                      sizeof(FlashConfig));
  restore_interrupts(ints);
  const FlashConfig *flash_cfg =
      (const FlashConfig *)(XIP_BASE + FLASH_TARGET_OFFSET);
  if (memcmp(&to_write, flash_cfg, sizeof(FlashConfig)) != 0) {
    return false;
  }
  memcpy(&cached_config, &to_write, sizeof(FlashConfig));
  return true;
}