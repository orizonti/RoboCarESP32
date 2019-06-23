#ifndef WIFI_TCP_SERVER
#define WIFI_TCP_SERVER


#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#define example_wifi_ssid config_wifi_ssid
#define example_wifi_pass config_wifi_password
#define port config_example_port

esp_err_t event_handler(void *ctx, system_event_t *event);
void initialise_wifi(void);
void wait_for_ip();
void tcp_server_task(void *pvparameters);

#endif WIFI_TCP_SERVER