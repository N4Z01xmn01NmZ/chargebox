#include <esp_system.h>
#include <esp_log.h>
#include <nvs.h>
#include <nvs_flash.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "CharlockWiFi.h"
#include "CharlockPN532.h"
#include "CharlockI2C.h"

/* FreeRTOS task tag for logging */
static const char *s_tag = "CHARLOCKE";

void app_main() {
    ESP_LOGI(s_tag, "[APP] Startup..");
    ESP_LOGI(s_tag, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(s_tag, "[APP] IDF version: %s", esp_get_idf_version());

    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("esp-tls", ESP_LOG_VERBOSE);
    esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE);
    esp_log_level_set("MQTT_EXAMPLE", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_BASE", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
    esp_log_level_set("OUTBOX", ESP_LOG_VERBOSE);

    /* Initialize non volatile storage */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    esp_err_t status = WIFI_FAILURE;
    /* Connect to WiFi access point */
    status = cl_wifi_connect();
    if (WIFI_CONNECT != status) {
        ESP_LOGI("WiFi", "Failed to associate to AP");
        return;
    }

    ESP_ERROR_CHECK(cl_i2c_master_driver_init());
    
    xTaskCreate(&cl_nfc_task, "nfc_task", 4096, NULL, 4, NULL);
}
