#include "CharlockWiFi.h"

#include <stdio.h>
#include <stdlib.h>
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>

/* FreeRTOS event group signaling when we are connected */
static EventGroupHandle_t s_wifi_event_group;

/* FreeRTOS task tag for logging */
static const char *s_tag = "CB_WiFi_STA";

/* Counts the number of connection attempts made */
static int8_t s_retry_count = 0;

static void cl_wifi_event_handler(
    void *arg,
    esp_event_base_t event_base,
    int32_t event_id,
    void *event_data
) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(s_tag, "connecting to AP...");
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_count < CL_WIFI_MAX_RETRY) {
            ESP_LOGI(s_tag, "reconnecting to AP...");
            esp_wifi_connect();
            s_retry_count++;
        }
        else
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAILURE);
    }
}

static void cl_ip_event_handler(
    void* arg,
    esp_event_base_t event_base,
    int32_t event_id,
    void *event_data
) {
    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t*)event_data;
        ESP_LOGI(s_tag, "STA IP: %d.%d.%d.%d", IP2STR(&event->ip_info.ip));
        s_retry_count = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECT);
    }
}

esp_err_t cl_wifi_connect(void) {
    esp_err_t status = WIFI_FAILURE;

    s_wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&config));

    esp_event_handler_instance_t wifi_event_instance;
    esp_event_handler_instance_t ip_event_instance;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        &cl_wifi_event_handler,
        NULL,
        &wifi_event_instance
    ));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        &cl_ip_event_handler,
        NULL,
        &ip_event_instance
    ));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = CL_WIFI_SSID,
            .password = CL_WIFI_PASSWORD,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false
            }
        }
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(s_tag, "STA initialized");

    EventBits_t bits = xEventGroupWaitBits(
        s_wifi_event_group,
        WIFI_CONNECT | WIFI_FAILURE,
        pdFALSE,
        pdFALSE,
        portMAX_DELAY
    );

    if (bits & WIFI_CONNECT) {
        ESP_LOGI(
            s_tag,
            "connected to AP (SSID:%s, password:%s)",
            CL_WIFI_SSID,
            CL_WIFI_PASSWORD
            );
        status = WIFI_CONNECT;
    }
    else if (bits & WIFI_FAILURE) {
        ESP_LOGI(
            s_tag,
            "failed to connect to AP (SSID:%s, password:%s",
            CL_WIFI_SSID,
            CL_WIFI_PASSWORD
        );
        status = WIFI_FAILURE;
    }
    else {
        ESP_LOGE(s_tag, "UNEXPECTED EVENT");
        status = WIFI_FAILURE;
    }

    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(
        IP_EVENT, IP_EVENT_STA_GOT_IP, ip_event_instance
    ));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(
        WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_instance
    ));
    vEventGroupDelete(s_wifi_event_group);

    return status;
}
