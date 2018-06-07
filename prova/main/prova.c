/* Non-Volatile Storage (NVS) Read and Write a Blob - Example

   For other examples please check:
   https://github.com/espressif/esp-idf/tree/master/examples

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "prod.h"

void app_main()
{
	while (true) {
		PROD_BSN bsn = { 0 } ;

		(void) PROD_read_board(&bsn) ;

		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}

}
