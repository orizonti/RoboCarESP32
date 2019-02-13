#ifndef UART_TRANSFER_TASK
#define UART_TRANSFER_TASK

void tx_task();
void rx_task();
void init_uart();
void uart_event_task(void *pvParameters);

#endif UART_TRANSFER_TASK
