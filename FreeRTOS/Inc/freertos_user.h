#ifndef __FREERTOS_USER_H
#define __FREERTOS_USER_H

#include "main.h"
#include "FreeRTOS.h"
#include "semphr.h"

extern SemaphoreHandle_t TxSemaphore;

void FreeRTOS_APP_Init(void);

#endif
