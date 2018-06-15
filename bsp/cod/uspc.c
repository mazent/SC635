#include "uspc.h"
#include "esp_log.h"

#include "driver/uart.h"
#include "cirbu.h"

#define TAG		"uspc"

#define USPC_TXD  	(GPIO_NUM_1)
#define USPC_RXD  	(GPIO_NUM_3)
#define USPC_RTS  	(UART_PIN_NO_CHANGE)
#define USPC_CTS  	(UART_PIN_NO_CHANGE)
#define USPC_UART	UART_NUM_0

#define BUF_SIZE 	1024

#define NUM_EVN		20
static QueueHandle_t evnQ = NULL ;

static osThreadId tid = NULL ;

#define MAX_BUFF	(2 * BUF_SIZE)
static union {
	S_CIRBU c ;
	uint8_t b[sizeof(S_CIRBU) - 1 + MAX_BUFF] ;
} u ;

static void nocb(void)
{
}

static USPC_RX_CB cbRx = nocb ;


static const uart_event_t quit = {
	.type = UART_EVENT_MAX
} ;

static void uspcThd(void * v)
{
	static uint8_t dtmp[BUF_SIZE] ;
	bool cont = true ;
    uart_event_t event;
//    size_t buffered_size;
    while (cont) {
        //Waiting for UART event.
        if (xQueueReceive(evnQ, (void * )&event, (portTickType)portMAX_DELAY)) {
            ESP_LOGI(TAG, "uart[%d] event:", USPC_UART);
            switch (event.type) {
                //Event of UART receving data
                /*We'd better handler data event fast, there would be much more data events than
                other types of events. If we take too much time on data event, the queue might
                be full.*/
                case UART_DATA:
                    ESP_LOGI(TAG, "[UART DATA]: %d", event.size);
                    uart_read_bytes(USPC_UART, dtmp, event.size, portMAX_DELAY);
                    ESP_LOGI(TAG, "[DATA EVT]:");
                    CIRBU_ins(&u.c, dtmp, event.size) ;
                    uart_write_bytes(USPC_UART, (char *) dtmp, event.size);
                    break;
				//Event of UART RX break detected
				case UART_BREAK:
					ESP_LOGI(TAG, "uart rx break");
					break;
				//Event of UART ring buffer full
				case UART_BUFFER_FULL:
					ESP_LOGI(TAG, "ring buffer full");
					// If buffer full happened, you should consider encreasing your buffer size
					// As an example, we directly flush the rx buffer here in order to read more data.
					uart_flush_input(USPC_UART);
					xQueueReset(evnQ);
					break;
                //Event of HW FIFO overflow detected
                case UART_FIFO_OVF:
                    ESP_LOGI(TAG, "hw fifo overflow");
                    // If fifo overflow happened, you should consider adding flow control for your application.
                    // The ISR has already reset the rx FIFO,
                    // As an example, we directly flush the rx buffer here in order to read more data.
                    uart_flush_input(USPC_UART);
                    xQueueReset(evnQ);
                    break;
				//Event of UART frame error
				case UART_FRAME_ERR:
					ESP_LOGI(TAG, "uart frame error");
					break;
                //Event of UART parity check error
                case UART_PARITY_ERR:
                    ESP_LOGI(TAG, "uart parity error");
                    break;
                case UART_DATA_BREAK:
                	break ;
                //UART_PATTERN_DET
                case UART_PATTERN_DET:
//                    uart_get_buffered_data_len(USPC_UART, &buffered_size);
//                    int pos = uart_pattern_pop_pos(USPC_UART);
//                    ESP_LOGI(TAG, "[UART PATTERN DETECTED] pos: %d, buffered size: %d", pos, buffered_size);
//                    if (pos == -1) {
//                        // There used to be a UART_PATTERN_DET event, but the pattern position queue is full so that it can not
//                        // record the position. We should set a larger queue size.
//                        // As an example, we directly flush the rx buffer here.
//                        uart_flush_input(USPC_UART);
//                    } else {
//                        uart_read_bytes(USPC_UART, dtmp, pos, 100 / portTICK_PERIOD_MS);
//                        uint8_t pat[PATTERN_CHR_NUM + 1];
//                        memset(pat, 0, sizeof(pat));
//                        uart_read_bytes(USPC_UART, pat, PATTERN_CHR_NUM, 100 / portTICK_PERIOD_MS);
//                        ESP_LOGI(TAG, "read data: %s", dtmp);
//                        ESP_LOGI(TAG, "read pat : %s", pat);
//                    }
                    break;
                case UART_EVENT_MAX:
                	cont = false ;
                	break ;
                //Others
                default:
                    ESP_LOGI(TAG, "uart event type: %d", event.type);
                    break;
            }
        }
    }

	(void) osThreadTerminate(NULL) ;
	tid = NULL ;
}


#define STACK_SIZE			1500
osThreadDef(uspcThd, osPriorityNormal, 0, STACK_SIZE) ;

bool USPC_beg(uint32_t baud, USPC_RX_CB cb)
{
	bool esito = false ;
	uart_config_t uart_config = {
			.baud_rate = baud,
			.data_bits = UART_DATA_8_BITS,
			.parity    = UART_PARITY_DISABLE,
			.stop_bits = UART_STOP_BITS_1,
			.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
	};

	do {
		if (tid)
			break ;

		CIRBU_begin(&u.c, MAX_BUFF) ;

		esp_err_t err = uart_param_config(USPC_UART, &uart_config);
		if (err != ESP_OK)
			break ;
		
		err = uart_set_pin(USPC_UART, USPC_TXD, USPC_RXD, USPC_RTS, USPC_CTS);
		if (err != ESP_OK)
			break ;
		
		err = uart_driver_install(USPC_UART, BUF_SIZE, 0, NUM_EVN, &evnQ, 0);
		if (err != ESP_OK)
			break ;

		tid = osThreadCreate(osThread(uspcThd), NULL) ;
		if (NULL == tid)
			break ;

		if (cb)
			cbRx = cb ;

		esito = true ;
		
	} while (false) ;
	
	return esito ;
}

void USPC_end(void)
{
	if (tid) {
		CHECK_IT(pdTRUE == xQueueSend(evnQ, &quit, portMAX_DELAY)) ;
	}
	(void) uart_driver_delete(USPC_UART) ;
	evnQ = NULL ;
	cbRx = nocb ;
}

void USPC_tx(const void * v, uint16_t dim)
{
	if (NULL == v) {
	}
	else if (0 == dim) {
	}
	else
		uart_write_bytes(USPC_UART, (const char *) v, dim) ;
}

uint16_t USPC_rx(void * v, uint16_t dim)
{
	return CIRBU_ext(&u.c, (uint8_t *) v, dim) ;
}
