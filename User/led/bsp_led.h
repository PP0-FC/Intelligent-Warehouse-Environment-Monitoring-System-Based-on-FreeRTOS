
#ifndef __BSP_LED_H
#define __BSP_LED_H

#include "main.h"

/* LED ???? */
#define LED_R_Pin GPIO_PIN_1
#define LED_R_GPIO_Port GPIOA

/* LED ????? */
#define LED_R_ON()       HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_RESET)   // ?? LED_R
#define LED_R_OFF()      HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_SET)     // ?? LED_R
#define LED_R_Toggle()   HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin)                  // ?? LED_R

void LED_GPIO_Init(void);           // LED GPIO ?????

#endif /*__BSP_LED_H */

