#include "freertos_user.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "bsp_dht11.h"
#include "app_oled.h"
#include "bsp_esp8266.h"
#include "app_esp8266.h"


void Task_Sensor(void * pvParameters);    //DHT11温湿度传感器的任务逻辑
void Task_Oled(void * pvParameters);      //OLED的任务逻辑
void Task_Wifi(void * pvParameters);      //ESP8266的任务逻辑

QueueHandle_t xQueue1;             //队列1：实现DHT11与OLED间的数据传输
QueueHandle_t xQueue2;             //队列2：实现DHT11与ESP8266间的数据传输
SemaphoreHandle_t TxSemaphore = NULL;

void FreeRTOS_APP_Init(void)
{
	//创建任务
	xTaskCreate(Task_Sensor ,"Sensor" ,128 ,NULL ,1 ,NULL);
	xTaskCreate(Task_Oled ,"Oled" ,512 ,NULL ,1 ,NULL);
	xTaskCreate(Task_Wifi ,"Wifi" ,1024 ,NULL ,1 ,NULL);
	
	//创建队列
	xQueue1 = xQueueCreate(10 ,sizeof(DHT11_DATA_TYPEDEF));
	xQueue2 = xQueueCreate(10 ,sizeof(DHT11_DATA_TYPEDEF));
	
	//任务开始
	vTaskStartScheduler();
}

void Task_Sensor(void * pvParameters)
{
	for( ; ; )
	{
		DHT11_DATA_TYPEDEF send_data;
		if(DHT11_ReadData(&send_data) == HAL_OK)
		{
			xQueueSend(xQueue1 ,&send_data ,0);
			xQueueSend(xQueue2 ,&send_data ,0);
		}
		
		vTaskDelay(pdMS_TO_TICKS(1500));
	}
	
}

void Task_Oled(void * pvParameters)
{
	OLED_UI_Init();
	for( ; ; )
	{
		DHT11_DATA_TYPEDEF recv_data;
		if(xQueueReceive(xQueue1 ,&recv_data ,portMAX_DELAY) == pdTRUE)
		{
			OLED_DATA_Update(&recv_data);
		}
		
		vTaskDelay(pdMS_TO_TICKS(100));
	}
	
}

void Task_Wifi(void * pvParameters)
{
	//创建一个二值信号量
	if (TxSemaphore == NULL) 
    {
        TxSemaphore = xSemaphoreCreateBinary(); 
    }
	
	ESP8266_Init();
		
	ESP8266_Task();
		
	ESP8266_Bemfa();
	
	for( ; ; )
	{
		DHT11_DATA_TYPEDEF recv_data;
		if(xQueueReceive(xQueue2 ,&recv_data ,portMAX_DELAY) == pdTRUE)
		{
			char upload_str[256]; 
			snprintf(upload_str, sizeof(upload_str), 
							 "cmd=2&uid=db0e980dc50b4420ae62c2bd10b15648&topic=DHT11&msg=%2d.%d %2d.%d\r\n", 
							 recv_data.temp_int, recv_data.temp_deci,
							 recv_data.humi_int, recv_data.humi_deci    );
            
			xSemaphoreTake(TxSemaphore, 0); 
			ESP8266_SendString(upload_str);
			if (xSemaphoreTake(TxSemaphore, pdMS_TO_TICKS(1000)) == pdTRUE)
			{
				printf(">>>温度：%2d.%d。湿度：%2d.%d。\r\n" , 
								recv_data.temp_int, 
								recv_data.temp_deci,
								recv_data.humi_int, 
								recv_data.humi_deci           );
			}
			else
			{
				printf(">>>传输失败。");
			}
		}
		
		vTaskDelay(pdMS_TO_TICKS(100));
	}
	
}

