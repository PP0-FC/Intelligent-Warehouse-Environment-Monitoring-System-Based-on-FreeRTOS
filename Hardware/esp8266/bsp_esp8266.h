#ifndef __BSP_ESP8266_H
#define __BSP_ESP8266_H

#include "main.h"
#include "bsp_esp8266.h"

#define ESP8266_RST_Port GPIOB
#define ESP8266_RST_PIN  GPIO_PIN_9
#define ESP8266_IO_Port  GPIOB
#define ESP8266_IO_PIN   GPIO_PIN_8

#define UART_RX_Buff_LEN 512

extern uint8_t Debug_RxBuff[UART_RX_Buff_LEN];    //PC串口接收缓冲区
extern uint8_t ESP8266_RxBuff[UART_RX_Buff_LEN];  //ESP8266串口接收缓冲区



/******************************* ESP8266 数据类型定义 ***************************/

/**
  * @brief  ESP8266 网络工作模式枚举类型
  */
typedef enum{
    STA,        // 站点模式（Station），ESP8266 作为客户端连接 Wi-Fi
    AP,         // 接入点模式（Access Point），ESP8266 作为热点
    STA_AP      // 站点+接入点模式，同时作为客户端和热点
} ENUM_Net_ModeTypeDef;

/**
  * @brief  ESP8266 连接模式枚举类型
  */
typedef enum {
    SINGLE_CONN  = 0,  // 单连接模式，一次只能建立一个 TCP/UDP 连接
    MULTI_CONN   = 1  // 多连接模式，可同时建立多个 TCP/UDP 连接
} ENUM_ConnModeTypeDef;

/**
  * @brief  ESP8266 传输模式枚举类型
  */
typedef enum {
    TRANSPARENT_OFF = 0,  // 普通模式
    TRANSPARENT_ON  = 1   // 透传模式
} ENUM_TransparentModeTypeDef;


/************************ 函数声明 ************************/

void ESP8266_GPIO_Init(void);
void ESP8266_RST(void);
void UART_StartReceive(void);
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart);
void ESP8266_Init(void);
void ESP8266_SendString(char *str);
uint8_t ESP8266_Cmd(const char *cmd);
uint8_t ESP8266_Test(void);
uint8_t ESP8266_Soft_RST(void);
uint8_t ESP8266_NetMode(ENUM_Net_ModeTypeDef mode);
uint8_t ESP8266_ConnMode(ENUM_ConnModeTypeDef mode);
uint8_t ESP8266_ConnectWiFi(const char *ssid, const char *password);
uint8_t ESP8266_ConnectTCPServer(const char *ip, uint16_t port, uint8_t conn_id);
uint8_t ESP8266_SetTransparentMode(ENUM_TransparentModeTypeDef mode);

#endif

