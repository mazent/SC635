#include "bsp.h"

#include "driver/uart.h"

#define USPC_TXD  	(GPIO_NUM_1)
#define USPC_RXD  	(GPIO_NUM_3)
#define USPC_RTS  	(UART_PIN_NO_CHANGE)
#define USPC_CTS  	(UART_PIN_NO_CHANGE)
#define USPC_UART	UART_NUM_0

#define BUF_SIZE 	1024

#define NUM_EVN		10
static QueueHandle_t uart0_queue;

static void uart_event_task(void *pvParameters)
{
//    uart_event_t event;
//    size_t buffered_size;
//    uint8_t* dtmp = (uint8_t*) malloc(RD_BUF_SIZE);
//    for(;;) {
//        //Waiting for UART event.
//        if(xQueueReceive(uart0_queue, (void * )&event, (portTickType)portMAX_DELAY)) {
//            bzero(dtmp, RD_BUF_SIZE);
//            ESP_LOGI(TAG, "uart[%d] event:", EX_UART_NUM);
//            switch(event.type) {
//                //Event of UART receving data
//                /*We'd better handler data event fast, there would be much more data events than
//                other types of events. If we take too much time on data event, the queue might
//                be full.*/
//                case UART_DATA:
//                    ESP_LOGI(TAG, "[UART DATA]: %d", event.size);
//                    uart_read_bytes(EX_UART_NUM, dtmp, event.size, portMAX_DELAY);
//                    ESP_LOGI(TAG, "[DATA EVT]:");
//                    uart_write_bytes(EX_UART_NUM, (const char*) dtmp, event.size);
//                    break;
//                //Event of HW FIFO overflow detected
//                case UART_FIFO_OVF:
//                    ESP_LOGI(TAG, "hw fifo overflow");
//                    // If fifo overflow happened, you should consider adding flow control for your application.
//                    // The ISR has already reset the rx FIFO,
//                    // As an example, we directly flush the rx buffer here in order to read more data.
//                    uart_flush_input(EX_UART_NUM);
//                    xQueueReset(uart0_queue);
//                    break;
//                //Event of UART ring buffer full
//                case UART_BUFFER_FULL:
//                    ESP_LOGI(TAG, "ring buffer full");
//                    // If buffer full happened, you should consider encreasing your buffer size
//                    // As an example, we directly flush the rx buffer here in order to read more data.
//                    uart_flush_input(EX_UART_NUM);
//                    xQueueReset(uart0_queue);
//                    break;
//                //Event of UART RX break detected
//                case UART_BREAK:
//                    ESP_LOGI(TAG, "uart rx break");
//                    break;
//                //Event of UART parity check error
//                case UART_PARITY_ERR:
//                    ESP_LOGI(TAG, "uart parity error");
//                    break;
//                //Event of UART frame error
//                case UART_FRAME_ERR:
//                    ESP_LOGI(TAG, "uart frame error");
//                    break;
//                //UART_PATTERN_DET
//                case UART_PATTERN_DET:
//                    uart_get_buffered_data_len(EX_UART_NUM, &buffered_size);
//                    int pos = uart_pattern_pop_pos(EX_UART_NUM);
//                    ESP_LOGI(TAG, "[UART PATTERN DETECTED] pos: %d, buffered size: %d", pos, buffered_size);
//                    if (pos == -1) {
//                        // There used to be a UART_PATTERN_DET event, but the pattern position queue is full so that it can not
//                        // record the position. We should set a larger queue size.
//                        // As an example, we directly flush the rx buffer here.
//                        uart_flush_input(EX_UART_NUM);
//                    } else {
//                        uart_read_bytes(EX_UART_NUM, dtmp, pos, 100 / portTICK_PERIOD_MS);
//                        uint8_t pat[PATTERN_CHR_NUM + 1];
//                        memset(pat, 0, sizeof(pat));
//                        uart_read_bytes(EX_UART_NUM, pat, PATTERN_CHR_NUM, 100 / portTICK_PERIOD_MS);
//                        ESP_LOGI(TAG, "read data: %s", dtmp);
//                        ESP_LOGI(TAG, "read pat : %s", pat);
//                    }
//                    break;
//                //Others
//                default:
//                    ESP_LOGI(TAG, "uart event type: %d", event.type);
//                    break;
//            }
//        }
//    }
//    free(dtmp);
//    dtmp = NULL;
//    vTaskDelete(NULL);
}


bool USPC_beg(uint32_t baud) 
{
	bool esito = false ;
	uart_config_t uart_config = {
			.baud_rate = baud,
			.data_bits = UART_DATA_8_BITS,
			.parity    = UART_PARITY_DISABLE,
			.stop_bits = UART_STOP_BITS_1,
			.flow_ctrl = UART_HW_FLOWCTRL_DISABLE
	};
	do {
		esp_err_t err = uart_param_config(USPC_UART, &uart_config);
		if (err != ESP_OK)
			break ;
		
		err = uart_set_pin(USPC_UART, USPC_TXD, USPC_RXD, USPC_RTS, USPC_CTS);
		if (err != ESP_OK)
			break ;
		
		err = uart_driver_install(USPC_UART, BUF_SIZE, 0, NUM_EVN, &uart0_queue, 0);
		esito = ESP_OK == err ;
		
		xTaskCreate(uart_event_task, "uart_event_task", 2048, NULL, 12, NULL);
		
	} while (false) ;
	
	return esito ;
}