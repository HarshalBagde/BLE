# ESP32 BLE to MQTT Bridge

This project implements an ESP32 application that acts as a bridge between BLE (Bluetooth Low Energy) and MQTT (Message Queuing Telemetry Transport). It receives messages via BLE from compatible applications and publishes them to an MQTT broker.

## Project Structure

```
esp32-espidf-app
├── main
│   ├── main.c         # Entry point of the application
│   ├── ble.c          # BLE functionalities implementation
│   ├── ble.h          # BLE module header file
│   ├── wifi.c         # WiFi connection logic implementation
│   ├── wifi.h         # WiFi module header file
│   ├── mqtt.c         # MQTT functionalities implementation
│   ├── mqtt.h         # MQTT module header file
│   ├── bridge.c       # Logic for bridging BLE messages to MQTT
│   └── bridge.h       # Bridge module header file
├── CMakeLists.txt     # Build configuration file
└── sdkconfig          # Configuration settings for the ESP-IDF project
```

## Setup Instructions

1. **Clone the Repository**: Clone this repository to your local machine.

2. **Install ESP-IDF**: Ensure you have the ESP-IDF framework installed. Follow the official installation guide for your operating system.

3. **Configure the Project**:
   - Navigate to the project directory.
   - Run `idf.py menuconfig` to configure the project settings, including WiFi credentials and MQTT broker details.

4. **Build the Project**:
   - Run `idf.py build` to compile the project.

5. **Flash the Application**:
   - Connect your ESP32 device to your computer.
   - Run `idf.py -p (PORT) flash` to upload the firmware to the ESP32.

6. **Monitor Output**:
   - Use `idf.py -p (PORT) monitor` to view the serial output from the ESP32.

## Usage

- The ESP32 will start advertising as a BLE device.
- Connect to the BLE device using a compatible app (e.g., LightBlue or nRF Connect).
- Send messages to the BLE characteristic, which will be published to the specified MQTT topic.

## Testing

- Ensure that the MQTT broker is running and accessible from the ESP32.
- Use an MQTT client to subscribe to the topic specified in the code to verify that messages are being published correctly.

## Notes

- Modify the WiFi and MQTT configurations in the `wifi.c` and `mqtt.c` files as needed to match your network settings.
- Ensure that the necessary libraries for BLE and MQTT are included in your ESP-IDF environment.

## License

This project is licensed under the MIT License. See the LICENSE file for more details.