#include "freertos_user.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "bsp_dht11.h"
#include "app_oled.h"
#include "bsp_esp8266.h"
#include "app_esp8266.h"


void Task_Sensor(void * pvParameters);    //DHT11的任务逻辑
void Task_Oled(void * pvParameters);      //OLED的任务逻辑
void Task_Wifi(void * pvParameters);      //ESP8266的任务逻辑

QueueHandle_t xQueue1;             //队列1：用于DHT11与OLED的数据传输
QueueHandle_t xQueue2;             //队列2：用于DHT11与ESP8266的数据传输
SemaphoreHandle_t TxSemaphore = NULL;

/**
 * @brief 	FreeRTOS应用程序初始化函数
 * @note	在main函数中调用此函数来创建任务、队列和信号量，并启动调度器。
 * @retval	None
 */
void FreeRTOS_APP_Init(void)
{
	//	创建任务
	xTaskCreate(Task_Sensor ,"Sensor" ,128 ,NULL ,1 ,NULL);
	xTaskCreate(Task_Oled ,"Oled" ,512 ,NULL ,1 ,NULL);
	xTaskCreate(Task_Wifi ,"Wifi" ,1024 ,NULL ,1 ,NULL);
	
	//	创建队列
	xQueue1 = xQueueCreate(10 ,sizeof(DHT11_DATA_TYPEDEF));
	xQueue2 = xQueueCreate(10 ,sizeof(DHT11_DATA_TYPEDEF));
	
	//	创建信号量
	vTaskStartScheduler();
}

/**
 * @brief 	DHT11传感器任务函数
 * @note	此任务负责读取DHT11传感器的数据并将其发送到队列中。
 * @retval	None
 */
void Task_Sensor(void * pvParameters)
{
	for( ; ; )
	{
		DHT11_DATA_TYPEDEF send_data;

		//	读取DHT11传感器数据并发送到队列
		if(DHT11_ReadData(&send_data) == HAL_OK)
		{
			xQueueSend(xQueue1 ,&send_data ,0);
			xQueueSend(xQueue2 ,&send_data ,0);
		}
		
		vTaskDelay(pdMS_TO_TICKS(1500));
	}
	
}

/**
 * @brief 	OLED显示任务函数
 * @note	此任务负责从队列中接收数据并更新OLED显示。
 * @retval	None
 */
void Task_Oled(void * pvParameters)
{
	OLED_UI_Init();
	for( ; ; )
	{
		DHT11_DATA_TYPEDEF recv_data;

		//	从队列中接收数据并更新OLED显示
		if(xQueueReceive(xQueue1 ,&recv_data ,portMAX_DELAY) == pdTRUE)
		{
			OLED_DATA_Update(&recv_data);
		}
		
		vTaskDelay(pdMS_TO_TICKS(100));
	}
	
}

/**
 * @brief 	WiFi任务函数
 * @note	此任务负责处理WiFi相关的操作。
 * @retval	None
 */
void Task_Wifi(void * pvParameters)
{
	//	确保信号量已创建
	if (TxSemaphore == NULL) 
    {
		//创建二值信号量
        TxSemaphore = xSemaphoreCreateBinary(); 
    }
	
	ESP8266_Init();
		
	ESP8266_Task();
		
	ESP8266_Bemfa();
	
	for( ; ; )
	{
		//	从队列中接收数据并通过ESP8266发送到BEMFA平台
		DHT11_DATA_TYPEDEF recv_data;
		if(xQueueReceive(xQueue2 ,&recv_data ,portMAX_DELAY) == pdTRUE)
		{
			//	构建上传字符串
			char upload_str[256]; 
			snprintf(upload_str, sizeof(upload_str), 
							 "cmd=2&uid=BEMFA_UID&topic=BEMFA_TOPIC&msg=%2d.%d %2d.%d\r\n", 
							 recv_data.temp_int, recv_data.temp_deci,
							 recv_data.humi_int, recv_data.humi_deci    );

			//	发送数据前先获取信号量，确保上一次发送已完成
			xSemaphoreTake(TxSemaphore, 0); 

			ESP8266_SendString(upload_str);
			if (xSemaphoreTake(TxSemaphore, pdMS_TO_TICKS(1000)) == pdTRUE)
			{
				printf(">>>温度：%2d.%d湿度：%2d.%d??\r\n" , 
								recv_data.temp_int, 
								recv_data.temp_deci,
								recv_data.humi_int, 
								recv_data.humi_deci           );
			}
			else
			{
				printf(">>>数据传输失败\r\n");
			}
		}
		
		vTaskDelay(pdMS_TO_TICKS(100));
	}
	
}

