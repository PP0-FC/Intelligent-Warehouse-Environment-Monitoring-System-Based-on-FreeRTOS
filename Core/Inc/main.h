#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void Error_Handler(void);
void SystemClock_Config(void);

#define LED_R_Pin GPIO_PIN_1
#define LED_R_GPIO_Port GPIOA

#endif /* __MAIN_H */

