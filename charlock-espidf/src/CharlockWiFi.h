#ifndef CL_WIFI_H
#define CL_WIFI_H

#pragma once

#include <esp_err.h>

/*  This header use WiFi configuration that you can set by overwriting the entries
    below to strings with the config you want - i.e.
    #define CB_WIFI_SSID "egwifissid"
 */
#define CL_WIFI_SSID        "Julian"    // CB_WIFI_CONFIG_SSID
#define CL_WIFI_PASSWORD    "18192021"  // CB_WIFI_CONFIG_PASSWORD
#define CL_WIFI_MAX_RETRY   10

#define CL_BIT(x)           (1 << x)

/* Multiple bits associated with the event group for each event:
 * - connected to an AP with an IP
 * - failed to connect after exceeding the maximum amount of retries
 */
#define WIFI_CONNECT        CL_BIT(0)
#define WIFI_FAILURE        CL_BIT(1)

#define TCP_CONNECT         CL_BIT(0)
#define TCP_FAILURE         CL_BIT(1)

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t cl_wifi_connect(void);

#ifdef __cplusplus
}
#endif

#endif // CL_WIFI_H