#ifndef __BSP_USART_H__
#define __BSP_USART_H__

#include "main.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;
extern DMA_HandleTypeDef hdma_usart3_rx; 

void MX_USART1_UART_Init(void);
void MX_USART3_UART_Init(void);
void ESP8266_RX_DMA_Init(void);
void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle);
void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle);

#endif /* __BSP_USART_H__ */

