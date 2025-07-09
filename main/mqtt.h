#ifndef MQTT_H
#define MQTT_H

#include "esp_err.h"

void mqtt_app_start(void);
esp_err_t mqtt_publish(const char *message);

#endif // MQTT_H