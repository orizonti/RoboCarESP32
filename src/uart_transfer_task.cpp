#include "uart_transfer_task.h"
#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "soc/uart_struct.h"
#include "string.h"
#include "transfer_data_type.h"

static const char *TAG = "uart_events";

static const int RX_BUF_SIZE = 100;

//#define TXD_PIN (GPIO_NUM_1)
//#define RXD_PIN (GPIO_NUM_3)

#define TXD_PIN (GPIO_NUM_16)
#define RXD_PIN (GPIO_NUM_17)

#define EX_UART_NUM UART_NUM_2
#define PATTERN_CHR_NUM    (3)         /*!< Set the number of consecutive and identical characters received by receiver which defines a UART pattern*/

#define BUF_SIZE (100)
#define RD_BUF_SIZE (BUF_SIZE)
static QueueHandle_t uart0_queue;

QueueHandle_t DCMotorStateQueue;
QueueHandle_t AccelStateQueue;
QueueHandle_t StepMotorStateQueue;
QueueHandle_t RangeStateQueue;
QueueHandle_t BatteryStateQueue;


DC_MotorControlStruct* DC_MotorControlData;
AccelerometerStruct* AccelData;
StepMotorControlStruct* StepMotorData;
RangeControlStruct* RangeData;
void uart_event_task(void *pvParameters)
{
    uart_event_t event;
    size_t buffered_size;
    uint8_t* dtmp = (uint8_t*) malloc(RD_BUF_SIZE);

    DCMotorStateQueue = xQueueCreate(5,sizeof(DC_MotorControlStruct));
    AccelStateQueue = xQueueCreate(5,sizeof(AccelerometerStruct));
    StepMotorStateQueue = xQueueCreate(5,sizeof(StepMotorControlStruct));
    RangeStateQueue = xQueueCreate(5,sizeof(RangeControlStruct));
    BatteryStateQueue = xQueueCreate(5,sizeof(BatterControlStruct));

    HEADER_STRUCT* HEADER;


    vTaskDelay(200);
    for(;;) 
    {
        //Waiting for UART event.
        if(xQueueReceive(uart0_queue, (void * )&event, (portTickType)portMAX_DELAY)) 
        {
		
            bzero(dtmp, RD_BUF_SIZE);
            //ESP_LOGI(TAG, "uart[%d] event:", UART_NUM_0);
            switch(event.type) 
            {
                case UART_DATA:
                {
                    uart_read_bytes(UART_NUM_2, dtmp, event.size, portMAX_DELAY);

                    HEADER = (HEADER_STRUCT*)dtmp;

                    //ESP_LOGI(TAG, "UART REC - %s",dtmp+3);
                    //ESP_LOGI(TAG, "UART REC HEADER1 - %02X HEADER2 - %02X",*(uint16_t*)dtmp,*(uint16_t*)(dtmp+2));
                    if(HEADER->HEADER1 == 0xF1)
                    {
                        switch(HEADER->HEADER2)
                        {
                        case 0xD1:
                             ESP_LOGI(TAG, "DC MOTOR DATA REC");
                             DC_MotorControlData = (DC_MotorControlStruct*)dtmp;
                            xQueueSend(DCMotorStateQueue,(void*)DC_MotorControlData,(TickType_t)0);
                        break;
                        case 0xD2:
                            AccelData = (AccelerometerStruct*)dtmp;
	                        ESP_LOGI(TAG, "UART - ACCEL -%d %d %d",AccelData->AccelX,AccelData->AccelY,AccelData->AccelZ);
                            xQueueSend(AccelStateQueue,(void*)AccelData,(TickType_t)0);
                        break;
                        case 0xD3:
                             ESP_LOGI(TAG, "STEP MOTOR DATA REC");
                            StepMotorData = (StepMotorControlStruct*)dtmp;
                            xQueueSend(StepMotorStateQueue,(void*)StepMotorData,(TickType_t)0);
                        break;
                        case 0xD4:
                            ESP_LOGI(TAG, "BATTERY DATA REC");
                            //BatteryData = (BatterControlStruct*)dtmp;
                            //xQueueSend(BatteryStateQueue,(void*)BatteryData,(TickType_t)0);
                        break;
                        case 0xD5:
                            ESP_LOGI(TAG, "RANGE DATA REC");
                            RangeData = (RangeControlStruct*)dtmp;
                            xQueueSend(RangeStateQueue,(void*)RangeData,(TickType_t)0);
                        break;
                        }
                    }



                    //uart_write_bytes(UART_NUM_2, (const char*) dtmp, event.size);
                    break;
                }
                case UART_FIFO_OVF://Event of HW FIFO overflow detected
                        ESP_LOGI(TAG, "hw fifo overflow");
                    // If fifo overflow happened, you should consider adding flow control for your application.
                    uart_flush_input(UART_NUM_2);
                    xQueueReset(uart0_queue);
                    break;
                case UART_BUFFER_FULL://Event of UART ring buffer full
                    ESP_LOGI(TAG, "ring buffer full");
                    // If buffer full happened, you should consider encreasing your buffer size
                    uart_flush_input(UART_NUM_2);
                    xQueueReset(uart0_queue);
                    break;
                case UART_BREAK://Event of UART RX break detected
                    ESP_LOGI(TAG, "uart rx break");
                    break;
                
                case UART_PARITY_ERR://Event of UART parity check error
                    ESP_LOGI(TAG, "uart parity error");
                    break;
                case UART_FRAME_ERR://Event of UART frame error
                    ESP_LOGI(TAG, "uart frame error");
                    break;
                
                case UART_PATTERN_DET://UART_PATTERN_DET
                {
                    uart_get_buffered_data_len(UART_NUM_2, &buffered_size);
                    int pos = uart_pattern_pop_pos(UART_NUM_2);
                    ESP_LOGI(TAG, "[UART PATTERN DETECTED] pos: %d, buffered size: %d", pos, buffered_size);
                            if (pos == -1) 
                            {
                                // There used to be a UART_PATTERN_DET event, but the pattern position queue is full so that it can not
                                // record the position. We should set a larger queue size.
                                uart_flush_input(UART_NUM_2);
                            } 
                            else 
                            {
                                uart_read_bytes(UART_NUM_2, dtmp, pos, 100 / portTICK_PERIOD_MS);
                                uint8_t pat[PATTERN_CHR_NUM + 1];
                                memset(pat, 0, sizeof(pat));
                                uart_read_bytes(UART_NUM_2, pat, PATTERN_CHR_NUM, 100 / portTICK_PERIOD_MS);
                                ESP_LOGI(TAG, "read data: %s", dtmp);
                                ESP_LOGI(TAG, "read pat : %s", pat);
                            }
                    break;
                }
                default:
                {
                    ESP_LOGI(TAG, "uart event type: %d", event.type);
                    break;
                }
            }
        }
    }
    free(dtmp);
    dtmp = NULL;
    vTaskDelete(NULL);
}


void init_uart() 
{
    ESP_LOGI(TAG, "START INIT TASK");
    const uart_config_t uart_config = 
    {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_2, &uart_config);
        //Set UART pins (using UART0 default pins ie no changes.)
        //uart_set_pin(EX_UART_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_set_pin(UART_NUM_2, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    // We won't use a buffer for sending data.
    //uart_driver_install(EX_UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 20, &uart0_queue, 0);
    //uart_driver_install(UART_NUM_2, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_driver_install(UART_NUM_2, RX_BUF_SIZE * 2, BUF_SIZE * 2, 20, &uart0_queue, 0);

    esp_log_level_set(TAG, ESP_LOG_INFO);
    //Set uart pattern detect function.
    uart_enable_pattern_det_intr(UART_NUM_2, '+', PATTERN_CHR_NUM, 10000, 10, 10);
    //Reset the pattern queue length to record at most 20 pattern positions.
    uart_pattern_queue_reset(UART_NUM_2, 20);

    ESP_LOGI(TAG, "END INIT TASK");
}

int sendData(const char* logName, const char* data)
{
    const int len = strlen(data);
    const int txBytes = uart_write_bytes(UART_NUM_2, data, len);
    ESP_LOGI(logName, "Wrote %d bytes", txBytes);
    return txBytes;
}

void tx_task(void *pvparameters)
{
    static const char *TX_TASK_TAG = "TX_TASK";
    esp_log_level_set(TX_TASK_TAG, ESP_LOG_INFO);
    while (1) 
    {
        sendData(TX_TASK_TAG, "Hello world");
        vTaskDelay(8000 / portTICK_PERIOD_MS);
    }
}

void rx_task(void *pvparameters)
{
    static const char *RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    uint8_t* data = (uint8_t*) malloc(RX_BUF_SIZE+1);
    while (1) 
    {
        const int rxBytes = uart_read_bytes(UART_NUM_2, data, RX_BUF_SIZE, 1000 / portTICK_RATE_MS);
        if (rxBytes > 0) 
        {
            data[rxBytes] = 0;
            ESP_LOGI(RX_TASK_TAG, "Read %d bytes: '%s'", rxBytes, data);
            ESP_LOG_BUFFER_HEXDUMP(RX_TASK_TAG, data, rxBytes, ESP_LOG_INFO);
        }
    }
    free(data);
}
