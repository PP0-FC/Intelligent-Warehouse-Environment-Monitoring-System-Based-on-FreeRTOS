#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "bsp_dht11.h"
#include "app_oled.h"

osThreadId Task_sensorHandle;
osThreadId Task_oledHandle;
osThreadId Task_controlHandle;

osMessageQId Queue_sensorHandle;


void StartDefaultTask(void const * argument);
void StartTask02(void const * argument);
void StartTask03(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}


/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {

  /* Create the thread(s) */
  /* definition and creation of Task_sensor */
  osThreadDef(Task_sensor, StartDefaultTask, osPriorityNormal, 0, 128);
  Task_sensorHandle = osThreadCreate(osThread(Task_sensor), NULL);

  /* definition and creation of Task_oled */
  osThreadDef(Task_oled, StartTask02, osPriorityIdle, 0, 128);
  Task_oledHandle = osThreadCreate(osThread(Task_oled), NULL);

  /* definition and creation of Task_control */
  osThreadDef(Task_control, StartTask03, osPriorityIdle, 0, 128);
  Task_controlHandle = osThreadCreate(osThread(Task_control), NULL);
	
	osMessageQDef(Queue_sensorHandle ,2 ,DHT11_DATA_TYPEDEF);
	Queue_sensorHandle = osMessageCreate(osMessageQ(Queue_sensorHandle) ,NULL);

}

/**
  * @brief  Function implementing the Task_sensor thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  for(;;)
  {
		DHT11_DATA_TYPEDEF send_data;
		if(DHT11_ReadData(&send_data) == HAL_OK)
		{
			xQueueSend(Queue_sensorHandle ,&send_data ,0);
		}
		
    osDelay(1000);
  }
}

/**
* @brief Function implementing the Task_oled thread.
* @param argument: Not used
* @retval None
*/
void StartTask02(void const * argument)
{
	OLED_UI_Init();

  for(;;)
  {
		DHT11_DATA_TYPEDEF recv_data;
		if(xQueueReceive(Queue_sensorHandle ,&recv_data ,portMAX_DELAY) == pdTRUE)
		{
			OLED_DATA_Update(&recv_data);
		}
		
    osDelay(1);
  }
}

/**
* @brief Function implementing the Task_control thread.
* @param argument: Not used
* @retval None
*/
void StartTask03(void const * argument)
{
  for(;;)
  {
    osDelay(1);
  }
}

