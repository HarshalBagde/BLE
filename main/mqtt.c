#include <stdio.h>
#include <string.h>
#include "mqtt.h"
#include "esp_log.h"
#include "mqtt_client.h"
#define MQTT_BROKER_URI "mqtt://10.156.240.192"

static const char *TAG = "MQTT";

static esp_mqtt_client_handle_t client = NULL;

static void mqtt_event_handler_cb(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            esp_mqtt_client_publish(client, "esp32/test", "Hello from ESP32!", 0, 1, 0);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA: Topic: %.*s Data: %.*s",
                     event->topic_len, event->topic,
                     event->data_len, event->data);
            break;
        default:
            break;
    }
}

void mqtt_app_start(void) {
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER_URI,
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler_cb, NULL);
    esp_mqtt_client_start(client);
}

esp_err_t mqtt_publish(const char *message) {
    if (client == NULL) {
        ESP_LOGE(TAG, "MQTT client not initialized");
        return ESP_FAIL;
    }
    int msg_id = esp_mqtt_client_publish(client, "esp32/test", message, 0, 1, 0);
    if (msg_id == -1) {
        ESP_LOGE(TAG, "Failed to publish message");
        return ESP_FAIL;
    }
    return ESP_OK;
}