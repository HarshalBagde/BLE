#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include "wifi.h"
#include "mqtt.h"
#include "ble.h"

void app_main(void) {
    wifi_init();
    mqtt_app_start();
    xTaskCreate(ble_task, "ble_task", 4096, NULL, 5, NULL);
}