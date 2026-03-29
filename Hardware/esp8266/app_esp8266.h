#ifndef __APP_ESP8266_H__
#define __APP_ESP8266_H__

#include "main.h"

/********************************** 用户需要设置的参数**********************************/
#define      macUser_ESP8266_ApSsid                       "PP0"         		    //要连接的热点的名称
#define      macUser_ESP8266_ApPwd                        "Wklwk066"           	//要连接的热点的密钥

#define      macUser_ESP8266_TcpServer_IP                 "119.91.109.180"      //要连接的服务器的 IP
#define      macUser_ESP8266_TcpServer_Port               8344               		//要连接的服务器的端口
#define      BEMFA_UID                                    "Your_Bemfa_UID"      // 你的巴法云 32 位私钥
#define      BEMFA_TOPIC                                  "DHT11"                 //你的巴法云主题 

/************ 函数声明 ************/
void ESP8266_Task(void);
void ESP8266_Tranmit(void);
void ESP8266_Bemfa(void);

#endif

