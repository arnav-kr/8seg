#pragma once

namespace wifi {

inline constexpr int WIFI_CONNECT_TIMEOUT_MS = 15000; // 15 seconds
inline constexpr int MAX_RETRIES = 5;

bool connect(const char *ssid, const char *password);

bool is_connected();

} // namespace wifi
