#ifndef __BSP_USART_H__
#define __BSP_USART_H__

#include "main.h"
/****** UART 句柄 ******/
extern UART_HandleTypeDef huart1;           // USART1 句柄
extern UART_HandleTypeDef huart3;           // USART3 句柄
extern DMA_HandleTypeDef hdma_usart3_rx;    // USART3 RX DMA 句柄

/****** 函数声明 ******/
void MX_USART1_UART_Init(void);             // USART1 初始化函数
void MX_USART3_UART_Init(void);             // USART3 初始化函数 
void ESP8266_RX_DMA_Init(void);             // ESP8266 RX DMA 初始化函数
void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle);      // UART MSP 初始化函数
void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle);    // UART MSP 反初始化函数

#endif /* __BSP_USART_H__ */

