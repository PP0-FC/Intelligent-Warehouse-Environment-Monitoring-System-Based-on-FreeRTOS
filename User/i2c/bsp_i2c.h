#ifndef __BSP_I2C_H
#define __BSP_I2C_H

#include "main.h"

extern I2C_HandleTypeDef hi2c1;     // I2C1 句柄

void MX_I2C1_Init(void);            // I2C1 初始化函数
void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle);      // I2C MSP 初始化函数
void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle);    // I2C MSP 反初始化函数

#endif
