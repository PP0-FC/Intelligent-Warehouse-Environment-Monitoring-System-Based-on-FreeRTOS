#ifndef __BSP_DHT11_H
#define __BSP_DHT11_H

#include "main.h"

/* 定义 DHT11_DATA 连接的GPIO端口，用户只需要修改下面的代码即可改变控制的 DHT11_DATA 引脚 */

// DHT11_DATA
#define DHT11_DATA_Pin 				GPIO_PIN_12                   /* GPIO端口 */
#define DHT11_DATA_GPIO_Port 	GPIOB              						/* 对应引脚 */


/* DHT11数据结构体 */
typedef struct
{                            
   uint8_t humi_int;        // 湿度整数部分
   uint8_t humi_deci;       // 湿度小数部分
   uint8_t temp_int;        // 温度整数部分
   uint8_t temp_deci;       // 温度小数部分
   uint8_t check_sum;       // 校验和                              
} DHT11_DATA_TYPEDEF;


/* 函数声明 */
void DHT11_GPIO_Config(void);                                            //DHT11 DATA 引脚 IO 初始化
void DHT11_SetGPIOMode(uint32_t mode, uint32_t pull);                    //配置 DHT11 数据引脚的工作模式
uint8_t DHT11_ReadByte(void);                                            //读取DHT11一个字节数据
HAL_StatusTypeDef DHT11_ReadData(DHT11_DATA_TYPEDEF *dht11_data);        //读取DHT11传感器数据


#endif
