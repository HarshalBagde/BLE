#include <stdio.h>
#include <string.h>
#include "ble.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include <nvs_flash.h>

#define GATTS_SERVICE_UUID_TEST   0x00FF
#define GATTS_CHAR_UUID_TEST      0xFF01
#define GATTS_NUM_HANDLE_TEST     4

static const char *TAG = "BLE";
static uint16_t gatt_handle_table[GATTS_NUM_HANDLE_TEST];
static char last_ble_message[128] = {0};

static esp_ble_adv_params_t adv_params = {
    .adv_int_min        = 0x20,
    .adv_int_max        = 0x40,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy  = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static void gatts_profile_event_handler(esp_gatts_cb_event_t event,
                                        esp_gatt_if_t gatts_if,
                                        esp_ble_gatts_cb_param_t *param);

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    if (event == ESP_GAP_BLE_ADV_START_COMPLETE_EVT) {
        if (param->adv_start_cmpl.status == ESP_BT_STATUS_SUCCESS) {
            ESP_LOGI(TAG, "BLE advertising started");
        } else {
            ESP_LOGE(TAG, "Failed to start advertising");
        }
    }
}

void ble_task(void *pvParameters) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }
    ESP_LOGI(TAG, "Initializing BLE...");
    ret = esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
    if (ret) ESP_LOGW(TAG, "BT Classic mem release failed: %s", esp_err_to_name(ret));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));
    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());

    ESP_ERROR_CHECK(esp_ble_gatts_register_callback(gatts_profile_event_handler));
    ESP_ERROR_CHECK(esp_ble_gap_register_callback(gap_event_handler));
    ESP_ERROR_CHECK(esp_ble_gatts_app_register(0));
    vTaskDelete(NULL);
}

static void gatts_profile_event_handler(esp_gatts_cb_event_t event,
                                        esp_gatt_if_t gatts_if,
                                        esp_ble_gatts_cb_param_t *param) {
    switch (event) {
    case ESP_GATTS_REG_EVT: {
        ESP_LOGI(TAG, "GATT REG_EVT");
        esp_ble_gap_set_device_name("ESP_BLE");
        esp_ble_adv_data_t adv_data = {
            .set_scan_rsp = false,
            .include_name = true,
            .include_txpower = false,
            .min_interval = 0x20,
            .max_interval = 0x40,
            .appearance = 0,
            .manufacturer_len = 0,
            .p_manufacturer_data = NULL,
            .service_data_len = 0,
            .p_service_data = NULL,
            .service_uuid_len = 0,
            .p_service_uuid = NULL,
            .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
        };
        esp_ble_gap_config_adv_data(&adv_data);

        esp_gatt_srvc_id_t service_id = {
            .is_primary = true,
            .id.inst_id = 0,
            .id.uuid.len = ESP_UUID_LEN_16,
            .id.uuid.uuid.uuid16 = GATTS_SERVICE_UUID_TEST,
        };
        esp_ble_gatts_create_service(gatts_if, &service_id, GATTS_NUM_HANDLE_TEST);
        break;
    }
    case ESP_GATTS_CREATE_EVT: {
        ESP_LOGI(TAG, "GATT CREATE_EVT");
        gatt_handle_table[0] = param->create.service_handle;
        esp_gatt_char_prop_t property = ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_READ;
        esp_attr_value_t char_val = {
            .attr_max_len = sizeof(last_ble_message),
            .attr_len = sizeof("Hello"),
            .attr_value = (uint8_t *)"Hello",
        };
        esp_bt_uuid_t char_uuid = {
            .len = ESP_UUID_LEN_16,
            .uuid = {.uuid16 = GATTS_CHAR_UUID_TEST},
        };
        esp_ble_gatts_add_char(gatt_handle_table[0], &char_uuid,
                               ESP_GATT_PERM_WRITE | ESP_GATT_PERM_READ,
                               property, &char_val, NULL);
        break;
    }
    case ESP_GATTS_ADD_CHAR_EVT: {
        ESP_LOGI(TAG, "GATT ADD_CHAR_EVT");
        gatt_handle_table[1] = param->add_char.attr_handle;
        esp_ble_gatts_start_service(gatt_handle_table[0]);
        esp_ble_gap_start_advertising(&adv_params);
        break;
    }
    case ESP_GATTS_WRITE_EVT: {
        if (!param->write.is_prep) {
            int len = param->write.len < sizeof(last_ble_message) - 1 ? param->write.len : sizeof(last_ble_message) - 1;
            memcpy(last_ble_message, param->write.value, len);
            last_ble_message[len] = '\0';
            ESP_LOGI(TAG, "Received data over BLE: %s", last_ble_message);
        }
        break;
    }
    default:
        break;
    }
}

char* ble_receive_message(void) {
    return last_ble_message;
}