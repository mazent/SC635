/*
 * Tutorial: https://os.mbed.com/handbook/CMSIS-RTOS
 * API: https://arm-software.github.io/CMSIS_5/RTOS/html/group__CMSIS__RTOS.html
 */

#include "bsp.h"

#include "cmsis_os.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static TickType_t ms_in_tick(uint32_t millisec)
{
	TickType_t ticks = 1 ;

	assert(portTICK_PERIOD_MS) ;

	if (millisec == osWaitForever)
	    ticks = portMAX_DELAY ;
	else if (0 == millisec) {
		// At least one tick
		return 1 ;
	}
	else {
		// Rounding
	    ticks = 1 + (millisec - 1) / portTICK_PERIOD_MS ;
	}

	return ticks ;
}

static unsigned portBASE_TYPE freeRtos_pri(osPriority priority)
{
	assert(configMAX_PRIORITIES >= 7) ;

	switch (priority) {
	case osPriorityIdle:		return tskIDLE_PRIORITY ;
	case osPriorityLow:			return tskIDLE_PRIORITY + 1 ;
	case osPriorityBelowNormal: return tskIDLE_PRIORITY + 2 ;
	case osPriorityNormal:      return tskIDLE_PRIORITY + 3 ;
	case osPriorityAboveNormal: return tskIDLE_PRIORITY + 4 ;
	case osPriorityHigh:        return tskIDLE_PRIORITY + 5 ;
	case osPriorityRealtime:    return tskIDLE_PRIORITY + 6 ;
	default:					return tskIDLE_PRIORITY ;
	}
}


//  ==== Kernel Control Functions ====

///// Initialize the RTOS Kernel for creating objects.
///// \return status code that indicates the execution status of the function.
///// \note MUST REMAIN UNCHANGED: \b osKernelInitialize shall be consistent in every CMSIS-RTOS.
//osStatus osKernelInitialize (void);
//
///// Start the RTOS Kernel.
///// \return status code that indicates the execution status of the function.
///// \note MUST REMAIN UNCHANGED: \b osKernelStart shall be consistent in every CMSIS-RTOS.
//osStatus osKernelStart (void);
//
///// Check if the RTOS kernel is already started.
///// \note MUST REMAIN UNCHANGED: \b osKernelRunning shall be consistent in every CMSIS-RTOS.
///// \return 0 RTOS is not started, 1 RTOS is started.
//int32_t osKernelRunning(void);
//
//#if (defined (osFeature_SysTick)  &&  (osFeature_SysTick != 0))     // System Timer available
//
///// Get the RTOS kernel system timer counter
///// \note MUST REMAIN UNCHANGED: \b osKernelSysTick shall be consistent in every CMSIS-RTOS.
///// \return RTOS kernel system timer as 32-bit value
//uint32_t osKernelSysTick (void);
//
///// The RTOS kernel system timer frequency in Hz
///// \note Reflects the system timer setting and is typically defined in a configuration file.
//#define osKernelSysTickFrequency 100000000
//
///// Convert a microseconds value to a RTOS kernel system timer value.
///// \param         microsec     time value in microseconds.
///// \return time value normalized to the \ref osKernelSysTickFrequency
//#define osKernelSysTickMicroSec(microsec) (((uint64_t)microsec * (osKernelSysTickFrequency)) / 1000000)
//
//#endif    // System Timer available

//  ==== Thread Management ====

osThreadId osThreadCreate(const osThreadDef_t * td, void * argument)
{
	TaskHandle_t handle ;

	if (xTaskCreate(td->pthread,
					td->nome,
	                (td->stacksize + sizeof(portSTACK_TYPE) - 1) / sizeof(portSTACK_TYPE),
	                argument,
	                freeRtos_pri(td->tpriority),
	                &handle) != pdPASS) {
	    return NULL ;
	}

	return handle ;
}

///// Return the thread ID of the current running thread.
///// \return thread ID for reference by other functions or NULL in case of error.
///// \note MUST REMAIN UNCHANGED: \b osThreadGetId shall be consistent in every CMSIS-RTOS.
//osThreadId osThreadGetId (void);

#if (INCLUDE_vTaskDelete == 1)
osStatus osThreadTerminate(osThreadId thread_id)
{
    vTaskDelete(thread_id) ;

    return osOK ;
}
#endif

///// Pass control to next thread that is in state \b READY.
///// \return status code that indicates the execution status of the function.
///// \note MUST REMAIN UNCHANGED: \b osThreadYield shall be consistent in every CMSIS-RTOS.
//osStatus osThreadYield (void);
//
///// Change priority of an active thread.
///// \param[in]     thread_id     thread ID obtained by \ref osThreadCreate or \ref osThreadGetId.
///// \param[in]     priority      new priority value for the thread function.
///// \return status code that indicates the execution status of the function.
///// \note MUST REMAIN UNCHANGED: \b osThreadSetPriority shall be consistent in every CMSIS-RTOS.
//osStatus osThreadSetPriority (osThreadId thread_id, osPriority priority);
//
///// Get current priority of an active thread.
///// \param[in]     thread_id     thread ID obtained by \ref osThreadCreate or \ref osThreadGetId.
///// \return current priority value of the thread function.
///// \note MUST REMAIN UNCHANGED: \b osThreadGetPriority shall be consistent in every CMSIS-RTOS.
//osPriority osThreadGetPriority (osThreadId thread_id);


//  ==== Generic Wait Functions ====

osStatus osDelay(uint32_t millisec)
{
	vTaskDelay( ms_in_tick(millisec) ) ;

	return osOK ;
}

#if (defined (osFeature_Wait)  &&  (osFeature_Wait != 0))     // Generic Wait available

///// Wait for Signal, Message, Mail, or Timeout.
///// \param[in] millisec          \ref CMSIS_RTOS_TimeOutValue or 0 in case of no time-out
///// \return event that contains signal, message, or mail information or error code.
///// \note MUST REMAIN UNCHANGED: \b osWait shall be consistent in every CMSIS-RTOS.
//osEvent osWait (uint32_t millisec);

#endif  // Generic Wait available


//  ==== Timer Management Functions ====

///// Create a timer.
///// \param[in]     timer_def     timer object referenced with \ref osTimer.
///// \param[in]     type          osTimerOnce for one-shot or osTimerPeriodic for periodic behavior.
///// \param[in]     argument      argument to the timer call back function.
///// \return timer ID for reference by other functions or NULL in case of error.
///// \note MUST REMAIN UNCHANGED: \b osTimerCreate shall be consistent in every CMSIS-RTOS.
//osTimerId osTimerCreate (const osTimerDef_t *timer_def, os_timer_type type, void *argument);
//
///// Start or restart a timer.
///// \param[in]     timer_id      timer ID obtained by \ref osTimerCreate.
///// \param[in]     millisec      \ref CMSIS_RTOS_TimeOutValue "time delay" value of the timer.
///// \return status code that indicates the execution status of the function.
///// \note MUST REMAIN UNCHANGED: \b osTimerStart shall be consistent in every CMSIS-RTOS.
//osStatus osTimerStart (osTimerId timer_id, uint32_t millisec);
//
///// Stop the timer.
///// \param[in]     timer_id      timer ID obtained by \ref osTimerCreate.
///// \return status code that indicates the execution status of the function.
///// \note MUST REMAIN UNCHANGED: \b osTimerStop shall be consistent in every CMSIS-RTOS.
//osStatus osTimerStop (osTimerId timer_id);
//
///// Delete a timer that was created by \ref osTimerCreate.
///// \param[in]     timer_id      timer ID obtained by \ref osTimerCreate.
///// \return status code that indicates the execution status of the function.
///// \note MUST REMAIN UNCHANGED: \b osTimerDelete shall be consistent in every CMSIS-RTOS.
//osStatus osTimerDelete (osTimerId timer_id);


//  ==== Signal Management ====

osStatus osSignalSet(osThreadId thread_id, int32_t signal)
{
	assert(signal >= 0) ;

#if 0
    if (inHandlerMode()) {
    	BaseType_t xHigherPriorityTaskWoken = pdFALSE ;

        if (xTaskNotifyFromISR( thread_id, (uint32_t)signal, eSetBits, &xHigherPriorityTaskWoken ) != pdPASS )
            return osErrorOS ;

        portYIELD_FROM_ISR( xHigherPriorityTaskWoken ) ;
    }
    else if (xTaskNotify( thread_id, (uint32_t)signal, eSetBits) != pdPASS )
        return osErrorOS ;
#else
	if ( xTaskNotify(thread_id, (uint32_t) signal, eSetBits) != pdPASS )
		return osErrorOS ;
#endif
    return osOK ;
}

///// Clear the specified Signal Flags of an active thread.
///// \param[in]     thread_id     thread ID obtained by \ref osThreadCreate or \ref osThreadGetId.
///// \param[in]     signals       specifies the signal flags of the thread that shall be cleared.
///// \return previous signal flags of the specified thread or 0x80000000 in case of incorrect parameters or call from ISR.
///// \note MUST REMAIN UNCHANGED: \b osSignalClear shall be consistent in every CMSIS-RTOS.
//int32_t osSignalClear (osThreadId thread_id, int32_t signals);

osEvent osSignalWait(int32_t signals, uint32_t millisec)
{
	osEvent ret ;
	TickType_t ticks = ms_in_tick(millisec) ;

	assert(signals >= 0) ;

	if (0 == signals)
		signals = NEGA(0x80000000) ;

	if (xTaskNotifyWait(
			0, // ulBitsToClearOnEntry: non cancello niente quando mi metto in attesa
			(uint32_t) signals,
			(uint32_t *) &ret.value.signals,
			ticks) != pdTRUE)
		ret.status = osEventTimeout ;
	else if (ret.value.signals < 0)
		ret.status = osErrorValue ;
	else
		ret.status = osEventSignal ;

	return ret ;
}


//  ==== Mutex Management ====

///// Create and Initialize a Mutex object.
///// \param[in]     mutex_def     mutex definition referenced with \ref osMutex.
///// \return mutex ID for reference by other functions or NULL in case of error.
///// \note MUST REMAIN UNCHANGED: \b osMutexCreate shall be consistent in every CMSIS-RTOS.
//osMutexId osMutexCreate (const osMutexDef_t *mutex_def);
//
///// Wait until a Mutex becomes available.
///// \param[in]     mutex_id      mutex ID obtained by \ref osMutexCreate.
///// \param[in]     millisec      \ref CMSIS_RTOS_TimeOutValue or 0 in case of no time-out.
///// \return status code that indicates the execution status of the function.
///// \note MUST REMAIN UNCHANGED: \b osMutexWait shall be consistent in every CMSIS-RTOS.
//osStatus osMutexWait (osMutexId mutex_id, uint32_t millisec);
//
///// Release a Mutex that was obtained by \ref osMutexWait.
///// \param[in]     mutex_id      mutex ID obtained by \ref osMutexCreate.
///// \return status code that indicates the execution status of the function.
///// \note MUST REMAIN UNCHANGED: \b osMutexRelease shall be consistent in every CMSIS-RTOS.
//osStatus osMutexRelease (osMutexId mutex_id);
//
///// Delete a Mutex that was created by \ref osMutexCreate.
///// \param[in]     mutex_id      mutex ID obtained by \ref osMutexCreate.
///// \return status code that indicates the execution status of the function.
///// \note MUST REMAIN UNCHANGED: \b osMutexDelete shall be consistent in every CMSIS-RTOS.
//osStatus osMutexDelete (osMutexId mutex_id);


//  ==== Semaphore Management Functions ====

#if (defined (osFeature_Semaphore)  &&  (osFeature_Semaphore != 0))     // Semaphore available

///// Create and Initialize a Semaphore object used for managing resources.
///// \param[in]     semaphore_def semaphore definition referenced with \ref osSemaphore.
///// \param[in]     count         number of available resources.
///// \return semaphore ID for reference by other functions or NULL in case of error.
///// \note MUST REMAIN UNCHANGED: \b osSemaphoreCreate shall be consistent in every CMSIS-RTOS.
//osSemaphoreId osSemaphoreCreate (const osSemaphoreDef_t *semaphore_def, int32_t count);
//
///// Wait until a Semaphore token becomes available.
///// \param[in]     semaphore_id  semaphore object referenced with \ref osSemaphoreCreate.
///// \param[in]     millisec      \ref CMSIS_RTOS_TimeOutValue or 0 in case of no time-out.
///// \return number of available tokens, or -1 in case of incorrect parameters.
///// \note MUST REMAIN UNCHANGED: \b osSemaphoreWait shall be consistent in every CMSIS-RTOS.
//int32_t osSemaphoreWait (osSemaphoreId semaphore_id, uint32_t millisec);
//
///// Release a Semaphore token.
///// \param[in]     semaphore_id  semaphore object referenced with \ref osSemaphoreCreate.
///// \return status code that indicates the execution status of the function.
///// \note MUST REMAIN UNCHANGED: \b osSemaphoreRelease shall be consistent in every CMSIS-RTOS.
//osStatus osSemaphoreRelease (osSemaphoreId semaphore_id);
//
///// Delete a Semaphore that was created by \ref osSemaphoreCreate.
///// \param[in]     semaphore_id  semaphore object referenced with \ref osSemaphoreCreate.
///// \return status code that indicates the execution status of the function.
///// \note MUST REMAIN UNCHANGED: \b osSemaphoreDelete shall be consistent in every CMSIS-RTOS.
//osStatus osSemaphoreDelete (osSemaphoreId semaphore_id);

#endif     // Semaphore available


//  ==== Memory Pool Management Functions ====

#if (defined (osFeature_Pool)  &&  (osFeature_Pool != 0))  // Memory Pool Management available

///// Create and Initialize a memory pool.
///// \param[in]     pool_def      memory pool definition referenced with \ref osPool.
///// \return memory pool ID for reference by other functions or NULL in case of error.
///// \note MUST REMAIN UNCHANGED: \b osPoolCreate shall be consistent in every CMSIS-RTOS.
//osPoolId osPoolCreate (const osPoolDef_t *pool_def);
//
///// Allocate a memory block from a memory pool.
///// \param[in]     pool_id       memory pool ID obtain referenced with \ref osPoolCreate.
///// \return address of the allocated memory block or NULL in case of no memory available.
///// \note MUST REMAIN UNCHANGED: \b osPoolAlloc shall be consistent in every CMSIS-RTOS.
//void *osPoolAlloc (osPoolId pool_id);
//
///// Allocate a memory block from a memory pool and set memory block to zero.
///// \param[in]     pool_id       memory pool ID obtain referenced with \ref osPoolCreate.
///// \return address of the allocated memory block or NULL in case of no memory available.
///// \note MUST REMAIN UNCHANGED: \b osPoolCAlloc shall be consistent in every CMSIS-RTOS.
//void *osPoolCAlloc (osPoolId pool_id);
//
///// Return an allocated memory block back to a specific memory pool.
///// \param[in]     pool_id       memory pool ID obtain referenced with \ref osPoolCreate.
///// \param[in]     block         address of the allocated memory block that is returned to the memory pool.
///// \return status code that indicates the execution status of the function.
///// \note MUST REMAIN UNCHANGED: \b osPoolFree shall be consistent in every CMSIS-RTOS.
//osStatus osPoolFree (osPoolId pool_id, void *block);

#endif   // Memory Pool Management available


//  ==== Message Queue Management Functions ====

#if (defined (osFeature_MessageQ)  &&  (osFeature_MessageQ != 0))     // Message Queues available

///// Create and Initialize a Message Queue.
///// \param[in]     queue_def     queue definition referenced with \ref osMessageQ.
///// \param[in]     thread_id     thread ID (obtained by \ref osThreadCreate or \ref osThreadGetId) or NULL.
///// \return message queue ID for reference by other functions or NULL in case of error.
///// \note MUST REMAIN UNCHANGED: \b osMessageCreate shall be consistent in every CMSIS-RTOS.
//osMessageQId osMessageCreate (const osMessageQDef_t *queue_def, osThreadId thread_id);
//
///// Put a Message to a Queue.
///// \param[in]     queue_id      message queue ID obtained with \ref osMessageCreate.
///// \param[in]     info          message information.
///// \param[in]     millisec      \ref CMSIS_RTOS_TimeOutValue or 0 in case of no time-out.
///// \return status code that indicates the execution status of the function.
///// \note MUST REMAIN UNCHANGED: \b osMessagePut shall be consistent in every CMSIS-RTOS.
//osStatus osMessagePut (osMessageQId queue_id, uint32_t info, uint32_t millisec);
//
///// Get a Message or Wait for a Message from a Queue.
///// \param[in]     queue_id      message queue ID obtained with \ref osMessageCreate.
///// \param[in]     millisec      \ref CMSIS_RTOS_TimeOutValue or 0 in case of no time-out.
///// \return event information that includes status code.
///// \note MUST REMAIN UNCHANGED: \b osMessageGet shall be consistent in every CMSIS-RTOS.
//osEvent osMessageGet (osMessageQId queue_id, uint32_t millisec);

#endif     // Message Queues available


//  ==== Mail Queue Management Functions ====

#if (defined (osFeature_MailQ)  &&  (osFeature_MailQ != 0))     // Mail Queues available

///// Create and Initialize mail queue.
///// \param[in]     queue_def     reference to the mail queue definition obtain with \ref osMailQ
///// \param[in]     thread_id     thread ID (obtained by \ref osThreadCreate or \ref osThreadGetId) or NULL.
///// \return mail queue ID for reference by other functions or NULL in case of error.
///// \note MUST REMAIN UNCHANGED: \b osMailCreate shall be consistent in every CMSIS-RTOS.
//osMailQId osMailCreate (const osMailQDef_t *queue_def, osThreadId thread_id);
//
///// Allocate a memory block from a mail.
///// \param[in]     queue_id      mail queue ID obtained with \ref osMailCreate.
///// \param[in]     millisec      \ref CMSIS_RTOS_TimeOutValue or 0 in case of no time-out
///// \return pointer to memory block that can be filled with mail or NULL in case of error.
///// \note MUST REMAIN UNCHANGED: \b osMailAlloc shall be consistent in every CMSIS-RTOS.
//void *osMailAlloc (osMailQId queue_id, uint32_t millisec);
//
///// Allocate a memory block from a mail and set memory block to zero.
///// \param[in]     queue_id      mail queue ID obtained with \ref osMailCreate.
///// \param[in]     millisec      \ref CMSIS_RTOS_TimeOutValue or 0 in case of no time-out
///// \return pointer to memory block that can be filled with mail or NULL in case of error.
///// \note MUST REMAIN UNCHANGED: \b osMailCAlloc shall be consistent in every CMSIS-RTOS.
//void *osMailCAlloc (osMailQId queue_id, uint32_t millisec);
//
///// Put a mail to a queue.
///// \param[in]     queue_id      mail queue ID obtained with \ref osMailCreate.
///// \param[in]     mail          memory block previously allocated with \ref osMailAlloc or \ref osMailCAlloc.
///// \return status code that indicates the execution status of the function.
///// \note MUST REMAIN UNCHANGED: \b osMailPut shall be consistent in every CMSIS-RTOS.
//osStatus osMailPut (osMailQId queue_id, void *mail);
//
///// Get a mail from a queue.
///// \param[in]     queue_id      mail queue ID obtained with \ref osMailCreate.
///// \param[in]     millisec      \ref CMSIS_RTOS_TimeOutValue or 0 in case of no time-out
///// \return event that contains mail information or error code.
///// \note MUST REMAIN UNCHANGED: \b osMailGet shall be consistent in every CMSIS-RTOS.
//osEvent osMailGet (osMailQId queue_id, uint32_t millisec);
//
///// Free a memory block from a mail.
///// \param[in]     queue_id      mail queue ID obtained with \ref osMailCreate.
///// \param[in]     mail          pointer to the memory block that was obtained with \ref osMailGet.
///// \return status code that indicates the execution status of the function.
///// \note MUST REMAIN UNCHANGED: \b osMailFree shall be consistent in every CMSIS-RTOS.
//osStatus osMailFree (osMailQId queue_id, void *mail);

#endif  // Mail Queues available

