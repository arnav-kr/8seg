#pragma once

namespace wifi {

inline constexpr int WIFI_CONNECT_TIMEOUT_MS = 15000; // 15 seconds
inline constexpr int MAX_RETRIES = 5;

void init();
void start_connect(const char *ssid, const char *password);
bool is_connecting();
bool is_connected();
const char* get_ip();

} // namespace wifi
