#include "app_esp8266.h"
#include "bsp_esp8266.h"
#include "bsp_dht11.h"
#include "freertos_user.h"


/**
  * @brief  ESP8266 任务函数
  * @note   完成 ESP8266 上电初始化、模式设置、Wi-Fi 连接、TCP 连接及透传模式配置
  * @retval 无
  */
void ESP8266_Task(void)
{
    // 软件复位 ESP8266
    printf(">>> ESP8266 软件复位中...\r\n");
    if(ESP8266_Soft_RST())       // 软件复位
        printf(">>> ESP8266 复位完成\r\n");
    else
    {
        printf(">>> ESP8266 复位超时!\r\n");
        return;                              // 复位失败则退出任务
    }
	
    // 测试 ESP8266 模块是否可用
    if(ESP8266_Test())
        printf(">>> ESP8266 模块测试通过\r\n");
    else
    {
        printf(">>> ESP8266 模块不可用!\r\n");
        return;                                       // 模块不可用则退出任务
    }

    // 设置工作模式为 STA
    if(ESP8266_NetMode(STA))
        printf(">>> ESP8266 已设置为 STA 模式\r\n");
    else
    {
        printf(">>> 设置工作模式失败!\r\n");
        return;                                       // 设置失败则退出任务
    }

    // 设置单连接模式
    if(ESP8266_ConnMode(SINGLE_CONN))
        printf(">>> ESP8266 单连接模式已设置\r\n");
    else
    {
        printf(">>> 设置连接模式失败!\r\n");
        return;                                       // 设置失败则退出任务
    }

    // 连接指定 Wi-Fi
    if(ESP8266_ConnectWiFi(macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd) == 0)
		{
        printf(">>> Wi-Fi 已连接\r\n");
				HAL_Delay(2000);
		}
    else
		{
        printf(">>> Wi-Fi 连接失败! \r\n");
				return; 
		}
			
    // 连接 TCP 服务器
    if(ESP8266_ConnectTCPServer(macUser_ESP8266_TcpServer_IP, macUser_ESP8266_TcpServer_Port, 0))
        printf(">>> 已连接到 TCP 服务器\r\n");
    else
    {
        printf(">>> 连接 TCP 服务器失败!\r\n");
        return;                                       // 连接失败则退出任务
    }
		
    // 设置透传模式
    if(ESP8266_SetTransparentMode(TRANSPARENT_ON))
        printf(">>> ESP8266 已开启透传模式\r\n");
    else
		{
        printf(">>> 设置透传模式失败!\r\n");
				return; 
		}
		
		ESP8266_Cmd("AT+CIPSEND"); 
    HAL_Delay(500); // 稍微等半秒钟，让它回复 '>'
    printf(">>> 开始实时发数据\r\n");
}
/**
  * @brief  ESP8266 通过TCP协议连接巴法云平台
  * @param  无 
  * @retval 无
  */
void ESP8266_Bemfa(void)
{
    // 构造订阅字符串
	char sub_str[128];
	sprintf(sub_str , "cmd=1&uid=BEMFA_UID&topic=BEMFA_TOPIC\r\n");
	
	xSemaphoreTake(TxSemaphore, 0);     // 先清空信号量，确保后续等待时能正确获取
	
	ESP8266_SendString(sub_str);        // 发送订阅请求
	
    // 等待订阅结果，最长等待1秒
	if (xSemaphoreTake(TxSemaphore, pdMS_TO_TICKS(1000)) == pdTRUE)     // 成功获取信号量，说明订阅成功
	{
		printf(">>>订阅成功\r\n");
	}
	else
	{
		printf(">>>订阅失败\r\n");
	}
}
