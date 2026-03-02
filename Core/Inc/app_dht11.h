#ifndef __APP_DHT11_H
#define __APP_DHT11_H

#include "main.h"

/* 函数声明 */
void Dht11_ReadAndPrint(void);   //读取DHT11传感器数据并打印结果
void Dht11_ReadAndShow(void);    //读取DHT11传感器数据并再OLED屏上面显示

#endif
