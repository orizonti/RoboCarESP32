#ifndef WIFI_TCP_SERVER
#define WIFI_TCP_SERVER

#define WIFI_READ_BUFF_SIZE 100
#define LWIP_IPV4 1
#define LWIP_SOCKET 1
#define LWIP_COMPAT_SOCKETS 1
#define WITH_POSIX 
#define IDF_VER "3.30200.190418"
#define PLATFORMIO 3060
#define ARDUINO_ESP32_DEV 1
#define ESP32 
#define ESP_PLATFORM 
#define HAVE_CONFIG_H 
#define MBEDTLS_CONFIG_FILE "mbedtls/esp_config.h"
#define GCC_NOT_5_2_0 0
#define example_wifi_ssid config_wifi_ssid
#define example_wifi_pass config_wifi_password
#define port config_example_port
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "lwip/sockets.h"
#include <string.h>
#include <sys/param.h>
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "transfer_data_type.h"

#define example_wifi_ssid config_wifi_ssid
#define example_wifi_pass config_wifi_password
#define port config_example_port

esp_err_t event_handler(void *ctx, system_event_t *event);
void initialise_wifi(void);
void wait_for_ip();
void tcp_server_task(void *pvparameters);

//class MCDataQueueBuffer
//{
//	public:
//		void AppedDataFromSocket();
//	            int bytes_received = recv(sock,WIFI_BUFFER ,WIFI_READ_BUFF_SIZE, 0);
//
//        uint8_t* WIFI_BUFFER;
//
//
//
//}


#endif "WIFI_TCP_SERVER"
