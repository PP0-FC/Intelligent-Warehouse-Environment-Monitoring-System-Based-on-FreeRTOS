#ifndef __APP_OLED_H
#define __APP_OLED_H

#include "main.h"
#include "bsp_oled.h"
#include "bsp_dht11.h"

void OLED_UI_Init(void);                          // OLED显示界面初始化
void OLED_DATA_Update(DHT11_DATA_TYPEDEF *data);  // OLED显示数据更新

#endif
