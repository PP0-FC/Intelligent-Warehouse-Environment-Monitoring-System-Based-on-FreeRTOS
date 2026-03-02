#include "app_oled.h"


void OLED_UI_Init(void)
{
		OLED_ShowChinese_F16X16(0, 2, 0);  // 第0行 第2列：显示“环”
    OLED_ShowChinese_F16X16(0, 3, 1);  // 第0行 第3列：显示“境”
    OLED_ShowChinese_F16X16(0, 4, 2);  // 第0行 第4列：显示“检”
    OLED_ShowChinese_F16X16(0, 5, 3);  // 第0行 第5列：显示“测”
	
	  /*---显示温度---*/
		OLED_ShowChinese_F16X16(1 ,0 ,4);
		OLED_ShowChinese_F16X16(1 ,1 ,5);
		OLED_ShowString_F8X16(1 , 4 , (uint8_t *)":");
	
		/*---显示湿度---*/
		OLED_ShowChinese_F16X16( 2 ,0 ,6 );
		OLED_ShowChinese_F16X16( 2 ,1 ,7 );
		OLED_ShowString_F8X16( 2 , 4 , (uint8_t *)":" );
}

void OLED_DATA_Update(DHT11_DATA_TYPEDEF *data)
{
	  //用字符串输出温度
		char temp_str[8];
		sprintf(temp_str ,"%2d.%d" , data->temp_int, data->temp_deci);
		OLED_ShowString_F8X16(1 ,5 ,(uint8_t *)temp_str);
	
	  //用字符串输出湿度
		char humi_str[8];
		sprintf(humi_str ,"%2d.%d" , data->humi_int, data->humi_deci);
		OLED_ShowString_F8X16(2 ,5 ,(uint8_t *)humi_str);
}
