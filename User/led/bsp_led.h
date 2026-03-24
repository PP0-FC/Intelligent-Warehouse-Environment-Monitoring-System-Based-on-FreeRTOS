
#ifndef __BSP_LED_H
#define __BSP_LED_H

#include "main.h"

#define LED_R_Pin GPIO_PIN_1
#define LED_R_GPIO_Port GPIOA

#define LED_R_ON()       HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_RESET)   //µãÁÁLED_R
#define LED_R_OFF()      HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_SET)     //Ï¨ÃðLED_R
#define LED_R_Toggle()   HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin)                  //·´×ªLED_R

void LED_GPIO_Init(void);

#endif /*__BSP_LED_H */

