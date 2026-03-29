# 🏠 基于 FreeRTOS 的室内环境智能检测系统
*(Intelligent Indoor Environment Monitoring System based on FreeRTOS)*

![Build Status](https://img.shields.io/badge/Build-Passing-brightgreen.svg)
![RTOS](https://img.shields.io/badge/RTOS-FreeRTOS-blue.svg)
![Platform](https://img.shields.io/badge/Platform-STM32-blue.svg)
![Cloud](https://img.shields.io/badge/IoT_Cloud-Bemfa-orange.svg)

## 📖 项目简介
本项目是一个基于 STM32 微控制器与 FreeRTOS 实时操作系统的物联网环境监测终端。系统能够高精度地实时采集室内温湿度数据，通过 OLED 屏幕进行本地可视化展示，并依托 ESP8266 无线模块，通过 TCP 透传协议将数据稳定上报至巴法云（Bemfa Cloud）物联网平台，实现微信小程序端的远程实时监控与历史曲线绘制。

## ✨ 核心特性
*  **RTOS 架构**：采用 FreeRTOS 进行多任务调度。
* **队列解耦**：使用队列，分别实现传感器与OLED之间的数据传输和传感器与服务器的数据传输。
* **稳定的网络防掉线机制**：具备完善的 TCP 断线重连、AT 指令状态机重置以及防御“透传陷阱”的底层逻辑。
* **云端全闭环**：支持微信小程序实时查看，并自动生成温湿度历史折线图。

## 🧰 硬件清单
* **主控芯片**：STM32F103C8T6 
* **温湿度传感器**：DHT11 (单总线通信)
* **无线通信模块**：ESP8266 (Wi-Fi 串口透传)
* **显示模块**：0.96 寸 OLED (I2C接口)

## 🧬 系统架构图 (任务流)

* `Task_Sensor`：每 2 秒严格按时序读取 DHT11，将数据分发至双队列。
* `Task_Oled`：阻塞等待本地队列，获取数据后立即刷新屏幕 UI。
* `Task_Wifi`：阻塞等待网络队列，处理字符串拼接，执行非阻塞中断发送。

## 🚀 快速上手

### 1. 硬件引脚接线
| 模块 | STM32 引脚 | 备注 |
| :--- | :--- | :--- |
| **DHT11** | PB12 | 单总线数据引脚 |
| **OLED** | SCL:PB6, SDA:PB7 | I2C 接口 |
| **ESP8266** | TX:PB11, RX:PB10 | 使用 USART3 与单片机通信 |
| **ESP8266_RST** | PB9 | 硬件复位引脚 |

### 2. 云端配置 (配置你的私钥)

使用本代码前，请打开 `app_esp8266.h` 文件，将以下宏定义修改为你自己的配置：
```c
#define     macUser_ESP8266_ApSsid          "***"          //要连接的热点的名称
#define     macUser_ESP8266_ApPwd           "***"  	       //要连接的热点的
#define     BEMFA_UID                       "***"          //你的巴法云 32 位私钥
#define     BEMFA_TOPIC                     "***"          //你的巴法云主题 
```
📝 **新手指南**：关于如何获取私钥（UID）以及创建 TCP 主题，请查阅 巴法云 TCP 创客云官方文档。

**文档连接**：https://cloud.bemfa.com/docs/src/tcp.html

## 🎨 显示取模说明 (OLED Display)

本项目使用 0.96 寸 OLED (SSD1306)，若需修改或新增显示字符（如中文名称、图标），请务必参考以下取模配置，否则将导致显示乱码。

### 1. 取模工具推荐
推荐使用 **PCtoLCD2002** 或在线取模工具。

### 2. 核心取模参数 (关键)
请按照以下设置配置取模软件，本项目底层驱动函数与之匹配：

| 参数项目 | 配置值 | 说明 |
| :--- | :--- | :--- |
| **取模方式** | **阴码** | 数据位为 1 时点亮像素，0 时熄灭 |
| **取模顺序** | **列行式** | 先沿垂直列扫描，再换行 |
| **输出方向** | **逆向 (低位在前)** | 字节的 LSB 对应屏幕上方像素 |
| **字体大小** | **16x16 / 12x12** | 常用中文字体规格 |

### 3. 代码结构说明
* 中文字库文件：`Hardware/OLED/oledfont.h`
* 汉字数组 
`const uint8_t chinese_library_16x16[][16]` ，在调用时确保索引正确。

## 📂 工程目录结构

```text
📦 室内环境检测系统 - [FreeRTOS]
├── 📂 Hardware               # 核心外设驱动与业务层 (🔥 核心亮点：BSP与APP分离)
│   ├── 📂 dht11              # 温湿度传感器 (单总线驱动)
│   ├── 📂 esp8266            # Wi-Fi 模块 (透传驱动与巴法云连接逻辑)
│   └── 📂 oled               # OLED 屏幕 (I2C驱动、包含字模 bsp_fonts)
│
├── 📂 User                   # 用户主逻辑与底层配置
│   ├── 📂 dwt                # DWT 精确微秒级延时驱动
│   ├── 📂 i2c                # I2C 总线底层支持
│   ├── 📂 led                # 状态指示灯底层支持
│   ├── 📂 usart              # 串口通信底层支持
│   ├── 📄 freertos.c         # 🧠 FreeRTOS 任务创建、队列与调度核心
│   ├── 📄 main.c             # 主程序入口与外设初始化
│   ├── 📄 main.h             # 全局宏定义
│   ├── 📄 stm32f1xx_it.c     # 硬件中断服务函数 (ISR)
│   └── 📄 stm32f1xx_hal_conf.h # HAL 库功能裁剪配置文件
│
├── 📂 FreeRTOS               # FreeRTOS V10+ 官方内核源码 (Inc/Portable/Scr)
├── 📂 Libraries              # STM32 底层库函数 (HAL)
├── 📂 Project                # Keil MDK 工程目录 (仅保留 .uvprojx 等工程配置)
├── 📄 .gitignore             # Git 提交忽略配置 (屏蔽 .o/.axf 等编译产物)
└── 📄 readme.md              # 项目自述文件