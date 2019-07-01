#include "wifi_tcp_server.h"
#include "transfer_data_type.h"

#define PORT 2323

HEADER_STRUCT* HEADER;
/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

const int IPV4_GOTIP_BIT = BIT0;
extern QueueHandle_t DCMotorStateQueue;
extern QueueHandle_t DCMotorStateQueue;
extern QueueHandle_t AccelStateQueue;
extern QueueHandle_t StepMotorStateQueue;
extern QueueHandle_t RangeStateQueue;
extern QueueHandle_t BatteryStateQueue;
const char *TAG = "example";

uint8_t WIFI_BUFFER[100];
//INITIALIZATOIN WIFI STATE PROCESS
esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id) 
    {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START");
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, IPV4_GOTIP_BIT);
        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, IPV4_GOTIP_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}
void initialise_wifi(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    //wifi_sta_config_t config = {"DLINK123","4997488286"};
    wifi_sta_config_t config = {"mgts340","4997488286"};
    wifi_config_t wifi_config;
    wifi_config.sta = config;
    
    //{
    //    .sta = 
    //    {
    //        .ssid = "mgts340",
    //        //.ssid = "DLINK123",
    //        .password = "4997488286",
    //    },
    //};
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}
void wait_for_ip()
{
    uint32_t bits = IPV4_GOTIP_BIT;
    ESP_LOGI(TAG, "Waiting for AP connection...");
    xEventGroupWaitBits(wifi_event_group, bits, false, true, portMAX_DELAY);
    ESP_LOGI(TAG, "Connected to AP");
    
}

void tcp_server_task(void *pvParameters)
{
    //char rx_buffer[128];
    char addr_str[128];
    int addr_family;
    int ip_protocol;

//DC_MotorControlStruct* DC_MotorControlStructCommand2 = (DC_MotorControlStruct*)malloc(sizeof(DC_MotorControlStruct));
                        //    DC_MotorControlStructCommand2->HEADER.HEADER1 = 0xF1;
                        //    DC_MotorControlStructCommand2->HEADER.HEADER2 = 0xD1;
                        //    DC_MotorControlStructCommand2->HEADER.SIZE_UNIT = 14;
                    
                        //    DC_MotorControlStructCommand2->Speed1 = 2020;
                        //    DC_MotorControlStructCommand2->Speed2 = 2030;
                        //    DC_MotorControlStructCommand2->Speed3 = 2040;
                        //    DC_MotorControlStructCommand2->Speed4 = 2050;

    DC_MotorControlStruct* DC_MotorControlData = (DC_MotorControlStruct*)malloc(sizeof(DC_MotorControlStruct));
    StepMotorControlStruct* Step_MotorControlData = (StepMotorControlStruct*)malloc(sizeof(StepMotorControlStruct));
    AccelerometerStruct* AccelerometerData = (AccelerometerStruct*)malloc(sizeof(AccelerometerStruct));

    RangeControlStruct* RangeData = (RangeControlStruct*)malloc(sizeof(RangeControlStruct));
    BatterControlStruct* BatterData = (BatterControlStruct*)malloc(sizeof(BatterControlStruct));

while (1) 
{

struct sockaddr_in destAddr;
destAddr.sin_addr.s_addr = htonl(INADDR_ANY);
destAddr.sin_family = AF_INET;
destAddr.sin_port = htons(PORT);
addr_family = AF_INET;
ip_protocol = IPPROTO_IP;


inet_ntoa_r(destAddr.sin_addr, addr_str, sizeof(addr_str) - 1);
//CREATE SOCKET
int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
    if (listen_sock < 0) 
    {
	ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
	break;
    }
    ESP_LOGI(TAG, "Socket created");

//BIND SOCKET TO ADDRESS
int err = bind(listen_sock, (struct sockaddr *)&destAddr, sizeof(destAddr));
    if (err != 0) 
    {
	ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
	break;
    }
    ESP_LOGI(TAG, "Socket binded");

//SOCKET LISTEN CONNECTION
    err = listen(listen_sock, 1);
    if (err != 0) 
    {
	ESP_LOGE(TAG, "Error occured during listen: errno %d", errno);
	break;
    }
    ESP_LOGI(TAG, "Socket listening");

//ACCEPT NEW CONNECTION
struct sockaddr_in6 sourceAddr; // Large enough for both IPv4 or IPv6
uint addrLen = sizeof(sourceAddr);
int sock = accept(listen_sock, (struct sockaddr *)&sourceAddr, &addrLen);
    if (sock < 0) 
    {
	ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
	break;
    }
    ESP_LOGI(TAG, "Socket accepted");


//COMMUNICATION PROCESS VIA SOCKET
	while(1)
	{

	    //DC_MOTOR
	    if(xQueueReceive(DCMotorStateQueue,DC_MotorControlData,(TickType_t)0))
	    {
		    err = send(sock, DC_MotorControlData, sizeof(DC_MotorControlStruct), 0);
	    }
	    //STEP MOTOR
	    if(xQueueReceive(StepMotorStateQueue,Step_MotorControlData,(TickType_t)0))
	    {
		    err = send(sock, Step_MotorControlData, sizeof(StepMotorControlStruct), 0);
	    }

	    //ACCELEROMTER
	    if(xQueueReceive(AccelStateQueue,AccelerometerData,(TickType_t)30))
	    {
	        ESP_LOGE(TAG, "HEADER %02X %02X",AccelerometerData->HEADER.HEADER1,AccelerometerData->HEADER.HEADER2);
	        ESP_LOGE(TAG, "ACCEL %d %d %d",AccelerometerData->AccelX,AccelerometerData->AccelY,AccelerometerData->AccelZ);
		    err = send(sock, AccelerometerData, sizeof(AccelerometerStruct), 0);
	    }

	    //RANGER
	    if(xQueueReceive(RangeStateQueue,RangeData,(TickType_t)0))
	    {
		    err = send(sock, RangeData, sizeof(RangeControlStruct), 0);
	    }

	    //BATTERY
	    if(xQueueReceive(BatteryStateQueue,BatterData,(TickType_t)0))
	    {
		    err = send(sock, BatterData, sizeof(BatterControlStruct), 0);
	    }

	    if(DCMotorStateQueue == 0)
	    {
	    ESP_LOGE(TAG, "DC QUEUE ERRROR !!!");
	    vTaskDelay(2000);
	    }

	    int nbytes = recv(sock, WIFI_BUFFER, 20, MSG_DONTWAIT);

		if(nbytes > 0)
		{
		    HEADER = (HEADER_STRUCT*)WIFI_BUFFER;

		    if(HEADER->HEADER1 == 0xF1 && HEADER->HEADER2 == 0xC1)
		    {
		        ESP_LOGE(TAG, "REC CONNECT CHECK REQUEST");
			ConnectCheckRequest* Req = (ConnectCheckRequest*)WIFI_BUFFER;
					     Req->Connect = 1;
			err = send(sock, Req, sizeof(ConnectCheckRequest), 0);
		    }

		}
	}

    if (sock != -1) 
    {
	ESP_LOGE(TAG, "Shutting down socket and restarting...");
	shutdown(sock, 0);
	close(sock);
    }
}
    free(DC_MotorControlData);
    vTaskDelete(NULL);
}
