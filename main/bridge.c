#include "bridge.h"
#include "mqtt.h"
#include "ble.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "Bridge";

void handle_ble_message(const char *message) {
    ESP_LOGI(TAG, "Received BLE Message: %s", message);

    // Publish the received message to MQTT
    if (mqtt_publish(message) == ESP_OK) {
        ESP_LOGI(TAG, "Message published to MQTT: %s", message);
    } else {
        ESP_LOGE(TAG, "Failed to publish message to MQTT");
    }
}

void bridge_task(void *pvParameters) {
    // This task will handle the bridging of BLE messages to MQTT
    while (1) {
        // Wait for a BLE message to be received
        char *received_message = ble_receive_message(); // This function should be implemented in ble.c

        if (received_message != NULL) {
            handle_ble_message(received_message);
            free(received_message); // Free the message after processing
        }

        vTaskDelay(pdMS_TO_TICKS(100)); // Delay to prevent busy-waiting
    }
}