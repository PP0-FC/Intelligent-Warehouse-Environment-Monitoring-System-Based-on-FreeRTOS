#include "app_dht11.h"
#include "bsp_dht11.h"
#include "bsp_oled.h"

static DHT11_DATA_TYPEDEF dht11_data = {0};

/**
  * @brief  读取DHT11传感器数据并打印结果
  * @param  无
  * @retval 读取成功返回1，失败返回0
  */
void Dht11_ReadAndPrint(void)
{
    if(DHT11_ReadData(&dht11_data) == HAL_OK)
    {
				printf("当前数据传输校验正确：");
			
        if(dht11_data.humi_deci & 0x80) // 湿度负数判断（一般DHT11无负湿度，保留）
        {
            printf("湿度为 -%d.%d %%RH，", dht11_data.humi_int, dht11_data.humi_deci);
        }
        else
        {
            printf("湿度为 %d.%d %%RH，", dht11_data.humi_int, dht11_data.humi_deci);
        }

        if(dht11_data.temp_deci & 0x80) // 温度负数判断
        {
            printf("温度为 -%d.%d ℃\r\n", dht11_data.temp_int, dht11_data.temp_deci);
        }
        else
        {
            printf("温度为 %d.%d ℃\r\n", dht11_data.temp_int, dht11_data.temp_deci);
        }
    }
    else
    {
        printf("读取DHT11数据错误！\r\n"); 
    }
}


/**
  * @brief  读取DHT11传感器数据并再OLED屏上面显示
  * @param  无
  * @retval 读取成功返回1，失败返回0
  */
void Dht11_ReadAndShow(void)
{
	  OLED_ShowChinese_F16X16(0, 2, 0);  // 第0行 第2列：显示“环”
    OLED_ShowChinese_F16X16(0, 3, 1);  // 第0行 第3列：显示“境”
    OLED_ShowChinese_F16X16(0, 4, 2);  // 第0行 第4列：显示“检”
    OLED_ShowChinese_F16X16(0, 5, 3);  // 第0行 第5列：显示“测”
	
	if(DHT11_ReadData(&dht11_data) == HAL_OK)
	{
		/*---显示温度---*/
		//“温度：”
		OLED_ShowChinese_F16X16(1 ,0 ,4);
		OLED_ShowChinese_F16X16(1 ,1 ,5);
		OLED_ShowString_F8X16(1 , 4 , (uint8_t *)":");
		
		//用字符串输出温度
		char temp_str[8];
		sprintf(temp_str ,"%2d.%d" , dht11_data.temp_int, dht11_data.temp_deci);
		OLED_ShowString_F8X16(1 ,5 ,(uint8_t *)temp_str);
		
		/*---显示湿度---*/
		//“湿度：”
		OLED_ShowChinese_F16X16( 2 ,0 ,6 );
		OLED_ShowChinese_F16X16( 2 ,1 ,7 );
		OLED_ShowString_F8X16( 2 , 4 , (uint8_t *)":" );
		
		//用字符串输出湿度
		char humi_str[8];
		sprintf(humi_str ,"%2d.%d" , dht11_data.humi_int, dht11_data.humi_deci);
		OLED_ShowString_F8X16(2 ,5 ,(uint8_t *)humi_str);
	}
	else
	{
		OLED_ShowString_F8X16(1 ,4 ,(uint8_t *)"DHT11");
		OLED_ShowString_F8X16(2 ,4 ,(uint8_t *)"ERROR");
	}
}


