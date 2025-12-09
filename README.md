# TM1729 LCD 驱动项目文档

## 项目概述

这是一个基于 STM32 微控制器的 LCD 驱动项目，专门用于控制采用 TM1729 驱动芯片的数码管显示屏。该项目通过 GPIO 模拟 I2C 总线协议与 TM1729 芯片通信，支持多种数据显示类型。

## 功能特性

- 基于 GPIO 的软件 I2C 实现
- 支持 TM1729 LCD 驱动芯片
- 多种数据显示类型支持:
  - 时间显示(小时、分钟)
  - 温度显示(两组温度数据)
  - 湿度显示
  - 信号强度指示
  - 电池电量显示
  - 分数显示
  - CO2 浓度显示
  - 铃铛状态指示

## 硬件连接

- I2C SCL: PB13
- I2C SDA: PB14

## 主要函数说明

### I2C 相关函数

| 函数名称 | 描述 |
|---------|------|
| `MyI2C_Init()` | 初始化 I2C 引脚配置 |
| `MyI2C_Start()` | 发送 I2C 起始信号 |
| `MyI2C_Stop()` | 发送 I2C 停止信号 |
| `MyI2C_SendByte()` | 通过 I2C 发送一个字节 |

### TM1729 芯片控制函数

| 函数名称 | 描述 |
|---------|------|
| `TM1729_Init()` | 初始化 TM1729 芯片 |
| `TM1729_Clear()` | 清除所有显示 |
| `TM1729_DisplayAll()` | 全显测试 |
| `App_Init()` | 应用初始化显示 |

### 显示控制函数

| 函数名称 | 描述 |
|---------|------|
| `TM1729_DisplaySet()` | 根据数据类型和数值设置显示内容 |

### 数据类型定义

```c
typedef enum {
    DATA_TYPE_SIGNAL,    // 信号强度(0-5级)
    DATA_TYPE_BELL,      // 铃铛开关(0/1)
    DATA_TYPE_HOUR,      // 小时(0-23)
    DATA_TYPE_MINUTE,    // 分钟(0-59)
    DATA_TYPE_SCORE,     // 分数(0-99)
    DATA_TYPE_BATTERY,   // 电量(0-4级)
    DATA_TYPE_TEMP1,     // 温度1(-9~99)
    DATA_TYPE_TEMP2,     // 温度2(-9~99)
    DATA_TYPE_HUMIDITY,  // 湿度(0-99)
    DATA_TYPE_CO2        // CO2浓度(0-999)
} DataType;
