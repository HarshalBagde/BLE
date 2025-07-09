#ifndef BRIDGE_H
#define BRIDGE_H

#include <stdint.h>

// Function to initialize the bridge between BLE and MQTT
void bridge_init(void);

// Function to handle received BLE messages and publish them to MQTT
void handle_ble_message(const char* message);

// FreeRTOS task to handle bridging BLE and MQTT
void bridge_task(void *pvParameters);

#endif // BRIDGE_H