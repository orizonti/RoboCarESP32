#include "wifi_tcp_server.h"
#include "uart_transfer_task.h"

void app_main()
{
    ESP_ERROR_CHECK( nvs_flash_init() );
    initialise_wifi();
    wait_for_ip();

    xTaskCreate(tcp_server_task, "tcp_server", 4096, NULL, 5, NULL);

    init_uart();
    xTaskCreate(uart_event_task, "uart_event_task", 2048, NULL, 12, NULL);
    //xTaskCreate(tx_task, "uart_tx_task", 1024*2, NULL, configMAX_PRIORITIES-1, NULL);
    //xTaskCreate(rx_task, "uart_rx_task", 1024*2, NULL, configMAX_PRIORITIES, NULL);
}


 
 

