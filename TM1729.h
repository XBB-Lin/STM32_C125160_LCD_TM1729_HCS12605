#ifndef __TM1729_H
#define __TM1729_H

#include "stm32f10x.h"

/**
 * @brief 数据类型枚举定义
 * 
 * 定义了系统支持的所有显示数据类型，每种类型对应不同的显示区域和格式
 */
typedef enum {
    DATA_TYPE_SIGNAL = 0,   ///< 信号强度显示 (0-5级)
    DATA_TYPE_BELL,         ///< 铃铛图标显示 (0关闭/1开启)
    DATA_TYPE_HOUR,         ///< 小时显示 (0-23)
    DATA_TYPE_MINUTE,       ///< 分钟显示 (0-59)
    DATA_TYPE_SCORE,        ///< 分数显示 (0-99)
    DATA_TYPE_BATTERY,      ///< 电池电量显示 (0-4级)
    DATA_TYPE_TEMP1,        ///< 温度1显示 (-9~99℃)
    DATA_TYPE_TEMP2,        ///< 温度2显示 (-9~99℃)
    DATA_TYPE_HUMIDITY,     ///< 湿度显示 (0-99%RH)
    DATA_TYPE_CO2           ///< 二氧化碳浓度显示 (0-999ppm)
} DataType;

// I2C基本函数
void TM1729_Init(void);
void TM1729_Clear(void);
void TM1729_DisplayAll(void);
int TM1729_DisplaySet(DataType type,int data);
void App_Init(void);

/** @defgroup TM1729_Commands TM1729驱动命令定义
 * @{
 */
#define TM1729_SLAVE_ADDRESS     0x7C    ///< TM1729从设备地址
#define TM1729_ICSET             0xEA    ///< IC设置命令
#define TM1729_BLKCTL            0xF0    ///< 块控制命令
#define TM1729_DISCTL            0xA2    ///< 显示控制命令
#define TM1729_MODESET           0xC8    ///< 模式设置命令
#define TM1729_APCTL             0xFC    ///< AP控制命令
#define TM1729_ADSET             0x00    ///< AD设置命令
/**
 * @}
 */

#endif