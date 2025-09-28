<img src="https://github.com/user-attachments/assets/3af20405-2633-455e-9f07-bd3b32d4bb37" alt="8seg" width="600" align="center"/>

# 8seg: A 7-segment multipurpose display

## Features
- Time display
- WiFi Connectivity
- Network Time Sync (NTP)
- Configurable Countdown Deadlines
- WiFi Status Indicator

## Hardware
- RPI Pico W
- WaveShare 8-segment display HAT

## Software
- C++ (Pico SDK)
- CMake

## Future Improvements
- MQTT Support, Dashboard Control
- Realtime remote configuration updates

## Learning Objectives
- C++ for practical embedded systems
- Implementing & Exploring communication protocols (SPI, I2C) and libraries
- Working with Real Time Clocks (RTC)
- WiFi connectivity and network time synchronization (NTP)
- State management in embedded systems
- Configuration management and persistence (Flash memory)

## Setup Instructions
1. update the `config.hpp` file with your WiFi credentials and deadlines.
2. Follow the [Pico SDK setup instructions](https://datasheets.raspberrypi.com/picow/pico-w-datasheet.pdf) to set up your development environment.
3. Clone this repository and navigate to the project directory.
4. Build the project using CMake and make
5. Flash the compiled binary to your Pico W using `picotool` or drag-and-drop the UF2 file.
6. Power on the device and it should connect to your WiFi and start displaying the time and countdowns.

## Author
[Arnav Kumar](https://github.com/arnav-kr)

## License
This project is licensed under the AGPL-3.0 License - see the [LICENSE](LICENSE) file for details.

