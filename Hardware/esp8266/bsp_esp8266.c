#include "bsp_esp8266.h"
#include "bsp_usart.h"
#include "freertos_user.h"

uint8_t Debug_RxBuff[UART_RX_Buff_LEN];    //PC串口接收缓冲区
uint8_t ESP8266_RxBuff[UART_RX_Buff_LEN];  //ESP8266串口接收缓冲区

uint8_t Wait_ESP_Reply = 0;        //标志位。0：未在等待ESP回复。1：正在等待ESP回复。

/**
  * @brief  ESP8266 外设 GPIO 初始化函数
  * @note   配置 ESP8266 的 IO 和复位引脚为推挽输出，并设置默认输出状态
  * @param  无
  * @retval 无
  */
void ESP8266_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = ESP8266_RST_PIN|ESP8266_IO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	
	HAL_GPIO_Init(GPIOB ,&GPIO_InitStruct);
	
	HAL_GPIO_WritePin(GPIOB ,ESP8266_RST_PIN|ESP8266_IO_PIN ,GPIO_PIN_SET);
}

/**
  * @brief  ESP8266 硬件复位
  * @note   将 ESP8266 RST 引脚拉低一定时间再拉高，实现复位
  * @param  无
  * @retval 无
  */
void ESP8266_RST(void)
{
	HAL_GPIO_WritePin(GPIOB ,ESP8266_RST_PIN ,GPIO_PIN_SET);
	HAL_Delay(100);
	
	HAL_GPIO_WritePin(GPIOB ,ESP8266_RST_PIN ,GPIO_PIN_RESET);
	HAL_Delay(500);
}

/**
  * @brief  启动 UART 接收
  * @note   配置 UART1 和 UART3 分别使用空闲中断和 DMA 模式接收数据
  * @param  无
  * @retval 无
  */
void UART_StartReceive(void)
{
	HAL_UARTEx_ReceiveToIdle_IT(&huart1 ,Debug_RxBuff ,UART_RX_Buff_LEN);
	HAL_UARTEx_ReceiveToIdle_DMA(&huart3 ,ESP8266_RxBuff ,UART_RX_Buff_LEN);
}

/**
  * @brief  UART 空闲中断/ DMA 接收完成回调函数
  * @note   根据不同的 UART 实例处理接收到的数据
  *         - USART1: 上位机数据到达，转发到 ESP8266 并重新开启 IT 接收
  *         - USART3: ESP8266 数据到达，转发到上位机并重新开启 DMA 接收
  * @param  huart: UART 句柄指针
  * @param  Size: 接收到的数据长度
  * @retval 无
  */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart->Instance == USART1)
	{
		HAL_UART_Transmit(&huart3 ,Debug_RxBuff ,Size ,100);
		memset(Debug_RxBuff ,0 ,UART_RX_Buff_LEN);
		HAL_UARTEx_ReceiveToIdle_IT(&huart1 ,Debug_RxBuff ,UART_RX_Buff_LEN);
	}
	if(huart->Instance == USART3)
	{
		HAL_UART_Transmit(&huart1 ,ESP8266_RxBuff ,Size ,100);
		if(Wait_ESP_Reply == 0)
		memset(ESP8266_RxBuff ,0 ,UART_RX_Buff_LEN);
		HAL_UARTEx_ReceiveToIdle_DMA(&huart3 ,ESP8266_RxBuff ,UART_RX_Buff_LEN);
  }
}

/**
  * @brief  UART 错误回调函数
  * @note   当 UART 出现错误时触发
  *         - USART3: 清除错误标志，并重新开启 DMA 接收
  * @param  huart: UART 句柄指针
  * @retval 无
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART3)
    {
        // 清除帧错误标志
        __HAL_UART_CLEAR_FEFLAG(huart);

        // 重新启动 ESP8266 DMA 接收
        HAL_UARTEx_ReceiveToIdle_DMA(&huart3, ESP8266_RxBuff, UART_RX_Buff_LEN);
    }
}

/**
  * @brief  ESP8266 初始化函数
  * @note   初始化 ESP8266 的 GPIO、串口，并执行复位
  * @param  无
  * @retval 无
  */
void ESP8266_Init(void)
{
    // 复位 ESP8266 模块
    ESP8266_RST();                        

    // 初始化 ESP8266 DMA 接收
    ESP8266_RX_DMA_Init();                                    

    // 初始化 ESP8266 串口
    MX_USART3_UART_Init();                                      

    // 初始化 ESP8266 控制引脚
    ESP8266_GPIO_Init();                                      
        
    printf("请使用串口调试助手发送\"AT+换行回车\"测试ESP8266是否准备好\n\n");

    // 启动 UART 接收任务
    UART_StartReceive();                                      
}

/**
 * @说明：     向通过串口3像esp8266发送字符串。
 * @输入参数： str--待发送的字符串。
 * @返回值：   无。
 */
void ESP8266_SendString(char *str)
{
	Wait_ESP_Reply = 1;
	HAL_UART_Transmit_IT(&huart3 , (uint8_t *)str , strlen(str));
}

/**
 * @说明：     向esp8266发送AT指令
 * @输入参数： cmd--待发送的指令。
 * @返回值：   1：发送成功  0：发送失败/响应超时
 * @note       使用freertos的二值信号量作为响应的标志。
 */
uint8_t ESP8266_Cmd(const char *cmd)
{
	//定义一个缓冲区，用来存放格式化的字符串
	static char buf[128];
	snprintf(buf , sizeof(buf) , "%s\r\n" , cmd);
	
	xSemaphoreTake(TxSemaphore, 0);
	
	ESP8266_SendString(buf);
	
	if (xSemaphoreTake(TxSemaphore, pdMS_TO_TICKS(100)) == pdTRUE)
    {
        return 1; 
    }
    else
    {
        return 0; 
    }
}

/**
 * @说明：     UART 发送完成回调（中断方式）
 * @输入参数： huart: UART 句柄指针
 * @返回值：   无
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART3)  // 发送到ESP8266完成
    {
        if (TxSemaphore != NULL)
        {
            // 这是一个 FreeRTOS 专属的标志位，记录“本次 Give 是否唤醒了一个比当前更高优先级的任务”
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
					
            //GiveFromISR
            xSemaphoreGiveFromISR(TxSemaphore, &xHigherPriorityTaskWoken);

            // 强制 CPU 在退出这个中断的瞬间，直接去执行 Task_Wifi
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
}

/**
 * @说明：     测试esp8266能否正常接收指令
 * @输入参数： 无
 * @返回值：   1 ：能正常工作。0 ：无法正常工作。
 */
uint8_t ESP8266_Test(void)
{
	//若指令无法正常发送，则返回0。
	if(ESP8266_Cmd("AT") == 0)
	{
		return 0;
	}
	
	// 确保接收缓冲区以 '\0' 结尾，防止字符串操作越界。
  ESP8266_RxBuff[UART_RX_Buff_LEN - 1] = '\0';
	
	//延时10ms。
	HAL_Delay(10);
	
	//若发送指令"AT"之后，没有回复"OK"，则返回0。
	if(strstr((char *)ESP8266_RxBuff , "OK") == NULL)
	{
		return 0;
	}
	
	//若回复"OK"，则可以正常使用。
	return 1;
}


/**
 * @说明：     复位esp8266。
 * @输入参数： 无
 * @返回值：   1 ：复位完成。0 ：复位失败。
 */
uint8_t ESP8266_Soft_RST(void)
{
	//若指令无法正常发送，则返回0。
	if(ESP8266_Cmd("AT+RST") == 0)
	{
		return 0;
	}
	
	// 确保接收缓冲区以 '\0' 结尾，防止字符串操作越界。
  ESP8266_RxBuff[UART_RX_Buff_LEN - 1] = '\0';
	
	//延时2s。
	HAL_Delay(2000);
	
		//若发送指令"AT"之后，没有回复"OK"，则返回0。
	if(strstr((char *)ESP8266_RxBuff , "ready") == NULL)
	{
		return 0;
	}
	
	//若回复"OK"，则可以正常使用。
	return 1;
	
}


/**
	* @说明：     设置传输模式
	* @输入参数： mode: 工作模式枚举类型 (STA/AP/STA+AP)
	* @返回值：   1 ：设置完成。0 ：设置失败。
	*/
uint8_t ESP8266_NetMode(ENUM_Net_ModeTypeDef mode)
{
	char *cmd;
	
	switch(mode)
	{
		case STA:
			cmd = "AT+CWMODE=1";
		  break;
		case AP:
			cmd = "AT+CWMODE=2";
			break;
		case STA_AP:
			cmd = "AT+CWMODE=3";
			break;
		default:
			return 0;
	}
	
	//若指令无法正常发送，则返回0。
	if(ESP8266_Cmd(cmd) == 0)
	{
		return 0;
	}
	
	// 确保接收缓冲区以 '\0' 结尾，防止字符串操作越界。
  ESP8266_RxBuff[UART_RX_Buff_LEN - 1] = '\0';
	
	//延时10ms。
	HAL_Delay(10);
	
	//若发送指令之后，没有回复"OK"，则返回0。
	if(strstr((char *)ESP8266_RxBuff , "OK") == NULL)
	{
		return 0;
	}
	
	//若回复"OK"，则设置成功。
	return 1;
	
}

/**
	* @说明：     设置连接模式。
	* @输入参数： mode: 单练接（0）/多连接（1）。
	* @返回值：   1 ：设置完成。0 ：设置失败。
	*/
uint8_t ESP8266_ConnMode(ENUM_ConnModeTypeDef mode)
{
	char *cmd;
	
	if(mode == 0)
	{
		cmd = "AT+CIPMUX=0";
	}
	else
	{
		cmd = "AT+CIPMUX=1";
	}
	
	//若指令无法正常发送，则返回0。
	if(ESP8266_Cmd(cmd) == 0)
	{
		return 0;
	}
	
	// 确保接收缓冲区以 '\0' 结尾，防止字符串操作越界。
  ESP8266_RxBuff[UART_RX_Buff_LEN - 1] = '\0';
	
	//延时10ms。
	HAL_Delay(10);
	
	//若发送指令之后，没有回复"OK"，则返回0。
	if(strstr((char *)ESP8266_RxBuff , "OK") == NULL)
	{
		return 0;
	}
	
	//若回复"OK"，则设置成功。
	return 1;
	
}

/**
  * @brief  连接指定 Wi-Fi
  * @param  ssid: Wi-Fi 名称
  * @param  password: Wi-Fi 密码
  * @retval 0 表示成功，非 0 表示失败及错误类型
  *         1：连接超时
  *         2：密码错误
  *         3：找不到目标 AP
  *         4：连接失败（其他原因）
  */
uint8_t ESP8266_ConnectWiFi(const char *ssid, const char *password)
{
	char cmd[128];
	snprintf(cmd, sizeof(cmd), "AT+CWJAP=\"%s\",\"%s\"", ssid, password); // 构造连接 Wi-Fi 指令

   //若指令无法正常发送，则返回4。
	if(ESP8266_Cmd(cmd) == 0)
	{
		return 4;
	}
	
	// 确保接收缓冲区以 '\0' 结尾，防止字符串操作越界。
  ESP8266_RxBuff[UART_RX_Buff_LEN - 1] = '\0';
	
	//延时6s。
	HAL_Delay(6000);
	
	if(strstr((char *)ESP8266_RxBuff , "OK"))
		return 0;
	if(strstr((char *)ESP8266_RxBuff , "+CWJAP:1"))
		return 1;
	if(strstr((char *)ESP8266_RxBuff , "+CWJAP:2"))
		return 2;
	if(strstr((char *)ESP8266_RxBuff , "+CWJAP:3"))
		return 3;
	if(strstr((char *)ESP8266_RxBuff , "+CWJAP:4"))
		return 4;
	
	return 1;
}

/**
  * @brief  通过 ESP8266 连接到 TCP 服务器
  * @note   根据是否为多连接模式，选择是否使用连接编号
  * @param  ip: 服务器 IP 地址
  * @param  port: 服务器端口
  * @param  conn_id: 连接编号（多连接模式有效，单连接模式忽略，取值 0~4）
  * @retval 1 表示连接成功，0 表示连接失败或超时
  */
uint8_t ESP8266_ConnectTCPServer(const char *ip, uint16_t port, uint8_t conn_id)
{
    /* 定义 AT 指令缓冲区 */
    char cmd[128];

    /* ESP8266 最多支持 5 个连接，编号范围 0~4
       编号非法直接返回失败 */
    if(conn_id > 4) return 0;

    /* 根据连接编号生成 AT 指令
       conn_id = 0: 单连接模式
       conn_id > 0: 多连接模式 */
    if(conn_id == 0)
        snprintf(cmd, sizeof(cmd), "AT+CIPSTART=\"TCP\",\"%s\",%d", ip, port);
    else
        snprintf(cmd, sizeof(cmd), "AT+CIPSTART=%d,\"TCP\",\"%s\",%d", conn_id, ip, port);
		
		 //若指令无法正常发送，则返回0。
		if(ESP8266_Cmd(cmd) == 0)
		{
			return 0;
		}
		
		// 确保接收缓冲区以 '\0' 结尾，防止字符串操作越界。
		ESP8266_RxBuff[UART_RX_Buff_LEN - 1] = '\0';
		
		//延时5s。
		HAL_Delay(5000);
		
		if(strstr((char *)ESP8266_RxBuff , "OK")|| strstr((char *)ESP8266_RxBuff , "CONNECT"))
			return 1;
		
		return 0;
}

/**
  * @brief  设置 ESP8266 透传模式
  * @note   根据 mode 参数选择开启或关闭透传模式
  * @param  mode: TRANSPARENT_OFF 关闭透传模式
  *               TRANSPARENT_ON  开启透传模式
  * @retval 1 表示设置成功，0 表示失败
  */
uint8_t ESP8266_SetTransparentMode(ENUM_TransparentModeTypeDef mode)
{
    /* 定义 AT 指令缓冲区 */
    char *cmd;

    /* 根据模式生成 AT 指令
       TRANSPARENT_OFF: 关闭透传模式
       TRANSPARENT_ON:  开启透传模式 */
    if(mode == TRANSPARENT_OFF)
        cmd = "AT+CIPMODE=0";
    else
        cmd = "AT+CIPMODE=1";
		
		//若指令无法正常发送，则返回0。
		if(ESP8266_Cmd(cmd) == 0)
		{
			return 0;
		}
		
		// 确保接收缓冲区以 '\0' 结尾，防止字符串操作越界。
		ESP8266_RxBuff[UART_RX_Buff_LEN - 1] = '\0';
		
		//延时10ms。
		HAL_Delay(10);
		
		if(strstr((char *)ESP8266_RxBuff , "OK"))
			return 1;
		
		return 0;
}
