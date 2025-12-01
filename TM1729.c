#include "stm32f10x.h"
#include "Delay.h"
#include "TM1729.h"

/**
 * @file    TM1729.c
 * @brief   基于GPIO模拟I2C总线与TM1729驱动芯片通信的实现文件
 * 
 * 本文件实现了通过STM32的GPIO引脚模拟I2C总线协议，
 * 与TM1729 LCD驱动芯片进行通信，控制数码管显示屏的显示内容。
 * 支持多种数据显示类型，包括时间、温度、湿度、信号强度等。
 */

#define MAX_LIGHT 26

/**
 * @brief 显示数据缓存数组
 * 
 * 存储发送到TM1729芯片的显示数据，每个元素控制若干个LCD段。
 * 数组初始化为默认显示状态。
 */
uint8_t displayData[MAX_LIGHT] = {0x80,0x00,0x00,0x80,0x00,0x80,0x80,0x80,0x00,0x10,
                 0x00,0x80,0x00,0x00,0x80,0x82,0x00,0x00,0x00,0x00,
                 0x00,0x00,0x00,0x00,0x00,0x00};

/* 引脚配置层 -------------------------------------------------------------*/

/**
 * @brief 设置I2C时钟线(SCL)电平
 * 
 * @param BitValue 电平值: 0-低电平, 1-高电平
 */
void MyI2C_W_SCL(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_13, (BitAction)BitValue);  // PB13 SCL
	// Delay_us(10);
}

/**
 * @brief 设置I2C数据线(SDA)电平
 * 
 * @param BitValue 电平值: 0-低电平, 1-高电平
 */
void MyI2C_W_SDA(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_14, (BitAction)BitValue);  // PB14 SDA
	// Delay_us(10);
}

/**
 * @brief 初始化I2C引脚
 * 
 * 配置PB13为SCL引脚，PB14为SDA引脚，开漏输出模式
 */
void MyI2C_Init(void)
{
	// 开启GPIOB时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;  // PB13:SCL, PB14:SDA
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_14);
}

/* 协议层 ------------------------------------------------------------------*/

/**
 * @brief 发送I2C起始信号
 * 
 * SDA下降沿发生在SCL高电平期间，标志着一次I2C传输的开始
 */
void MyI2C_Start(void)
{
	MyI2C_W_SDA(1);
    Delay_us(3);
	MyI2C_W_SCL(1);
    Delay_us(3);
    MyI2C_W_SDA(0);
    Delay_us(3);
	MyI2C_W_SCL(0);
    Delay_us(3);
}

/**
 * @brief 发送I2C停止信号
 * 
 * SDA上升沿发生在SCL高电平期间，标志着一次I2C传输的结束
 */
void MyI2C_Stop(void)
{
	MyI2C_W_SCL(1);
    Delay_us(3);
	MyI2C_W_SDA(0);
    Delay_us(3);
    MyI2C_W_SDA(1);
    Delay_us(3);
    MyI2C_W_SDA(0);
    MyI2C_W_SCL(0);
}

/**
 * @brief 通过I2C发送一个字节数据
 * 
 * @param Byte 要发送的字节数据
 */
void MyI2C_SendByte(uint8_t Byte)
{
    MyI2C_W_SCL(0);
	uint8_t i;
	for (i = 0; i < 8; i ++)
	{
		MyI2C_W_SCL(0);
        if(Byte & 0x80) {
            MyI2C_W_SDA(1);
        } else {
            MyI2C_W_SDA(0);
        }
        MyI2C_W_SCL(1);
        Byte = Byte << 1;
	}
    MyI2C_W_SCL(0);
    Delay_us(3);
    MyI2C_W_SDA(0);
    Delay_us(3);
    MyI2C_W_SCL(1);
    Delay_us(6);
    MyI2C_W_SCL(0);
    Delay_us(3);
}

/**
 * @brief 向TM1729芯片写入显示数据
 * 
 * @param pData 指向要写入数据的指针
 * @param len 数据长度
 */
void TM1729_WriteData(uint8_t *pData, uint8_t len)
{
    uint8_t i;
    
    MyI2C_Start();
    MyI2C_SendByte(TM1729_SLAVE_ADDRESS);
    MyI2C_SendByte(TM1729_ADSET);
    
    for(i = 0; i < len; i++)
    {
        MyI2C_SendByte(pData[i]);
    }
    
    MyI2C_Stop();
}

/* TM1729功能函数 ----------------------------------------------------------*/

/**
 * @brief 初始化TM1729芯片
 * 
 * 发送初始化序列，配置TM1729的工作模式和显示参数
 */
void TM1729_Init(void)
{
    MyI2C_Init();
    
    Delay_ms(10);

    MyI2C_Stop();
    MyI2C_Start();
    MyI2C_SendByte(TM1729_SLAVE_ADDRESS);
    MyI2C_SendByte(TM1729_ICSET);
    MyI2C_SendByte(TM1729_DISCTL);
    MyI2C_SendByte(TM1729_BLKCTL);
    MyI2C_SendByte(TM1729_APCTL);
    MyI2C_SendByte(TM1729_MODESET);
    MyI2C_Stop();
}

/**
 * @brief 清除所有显示
 * 
 * 将所有显示位置为不亮状态
 */
void TM1729_Clear(void)
{
    uint8_t clearData[MAX_LIGHT] = {0};
    TM1729_WriteData(clearData, MAX_LIGHT);
}

/**
 * @brief 全显测试
 * 
 * 点亮所有LCD段，用于检测显示是否正常工作
 */
void TM1729_DisplayAll(void)
{
	uint8_t i;
    uint8_t fillData[MAX_LIGHT];
    
    for(i = 0; i < MAX_LIGHT; i++) {
        fillData[i] = 0xFF; // 所有管子都亮
    }
    
    TM1729_WriteData(fillData, MAX_LIGHT);
}


/**
 * @brief 应用初始化显示
 * 
 * 使用默认显示数据初始化屏幕显示
 */
void App_Init(void)
{
	TM1729_WriteData(displayData,MAX_LIGHT);
}

/**
 * @brief 数码管显示数据表(1-9)
 * 
 * 适用于第1-9段数码管的显示数据，仅使用后7位
 */
uint8_t sge_data_1_9[11] = { //适用于1-9的数码管显示数据，只取后7位
    0x20, // -: 010 0000
    0x5F, // 0: 101 1111
    0x50, // 1: 101 0000
    0x6B, // 2: 110 1011
    0x79, // 3: 111 1001
    0x74, // 4: 111 0100
    0x3D, // 5: 011 1101
    0x3F, // 6: 011 1111
    0x58, // 7: 101 1000
    0x7F, // 8: 111 1111
    0x7D  // 9: 111 1101
};

/**
 * @brief 数码管显示数据表(10-15)
 * 
 * 适用于第10-15段数码管的显示数据，使用前4位和后3位
 */
uint8_t sge_data_10_15[11] = { //适用于10-15的数码管显示数据，取前4位和后3位
    0x02, // -: 0000 0010
    0xF5, // 0: 1111 0101
    0x05, // 1: 0000 0101
    0xD3, // 2: 1101 0011
    0x97, // 3: 1001 0111
    0x27, // 4: 0010 0111
    0xB6, // 5: 1011 0110
    0xF6, // 6: 1111 0110
    0x15, // 7: 0001 0101
    0xF7, // 8: 1111 0111
    0xB7  // 9: 1011 0111
};


/**
  * @brief  根据数据类型和数值设置TM1729显示内容
  * @param  type: 数据类型枚举值，决定显示的内容类型（如信号强度、时间、温度等）
  * @param  data: 要显示的具体数值，根据type的不同含义不同：
  *               - 信号强度(DATA_TYPE_SIGNAL): 0-5级
  *               - 时间(DATA_TYPE_HOUR/DATA_TYPE_MINUTE): 小时(0-23)或分钟(0-59)
  *               - 温度(DATA_TYPE_TEMP1/DATA_TYPE_TEMP2): 实际温度值(-9~99)
  *               - 分数(DATA_TYPE_SCORE): 分数值(0-99)
  *               - 电量(DATA_TYPE_BATTERY): 电量百分比(0-4级)
  *               - 湿度(DATA_TYPE_HUMIDITY): 湿度百分比(0-99)
  *               - CO2浓度(DATA_TYPE_CO2): CO2浓度值(0-999)
  *               - 铃铛(DATA_TYPE_BELL): 铃铛控制值(0关闭/1开启)
  * @retval int: 函数执行状态，0表示成功，-1表示参数错误，-2表示未知数据类型
  */
int TM1729_DisplaySet(DataType type,int data)
{
    switch(type)
    {
        case DATA_TYPE_SIGNAL:  //传值范围为0-5
            if(data>=0 && data <=5) {
                if(data == 0) {
                    displayData[9] &= 0x1F;  // 0001 1111 - 所有信号格都为0
                    displayData[13] &= 0x7F; // 0111 1111 - 所有信号格都为0
                    displayData[12] &= 0x7F; // 0111 1111 - 所有信号格都为0
                }
                if(data == 1) {
                    displayData[9] &= 0x1F;  // 0001 1111 - 所有信号格都为0
                    displayData[13] &= 0x7F; // 0111 1111 - 所有信号格都为0
                    displayData[12] &= 0x7F; // 0111 1111 - 所有信号格都为0
                    displayData[9] |= 0x20; //  0010 0000
                }
                if(data == 2) {
                    displayData[9] &= 0x1F;  // 0001 1111 - 所有信号格都为0
                    displayData[13] &= 0x7F; // 0111 1111 - 所有信号格都为0
                    displayData[12] &= 0x7F; // 0111 1111 - 所有信号格都为0
                    displayData[9] |=  0x60; //0110 0000
                }
                if(data == 3) {
                    displayData[9] &= 0x1F;  // 0001 1111 - 所有信号格都为0
                    displayData[13] &= 0x7F; // 0111 1111 - 所有信号格都为0
                    displayData[12] &= 0x7F; // 0111 1111 - 所有信号格都为0
                    displayData[9] |= 0xE0; // 1110 0000
                }
                if(data == 4) {
                    displayData[9] &= 0x1F;  // 0001 1111 - 所有信号格都为0
                    displayData[13] &= 0x7F; // 0111 1111 - 所有信号格都为0
                    displayData[12] &= 0x7F; // 0111 1111 - 所有信号格都为0
                    displayData[9] |= 0xE0; // 1110 0000
                    displayData[13] |= 0x80; // 1000 0000
                }
                if(data == 5) {
                    displayData[9] &= 0x1F;  // 0001 1111 - 所有信号格都为0
                    displayData[13] &= 0x7F; // 0111 1111 - 所有信号格都为0
                    displayData[12] &= 0x7F; // 0111 1111 - 所有信号格都为0
                    displayData[9] |= 0xE0; // 1110 0000
                    displayData[13] |= 0x80; // 1000 0000
                    displayData[12] |= 0x80; // 1000 0000
                }
            } else {
                return -1; // 参数错误
            }
            TM1729_WriteData(displayData, MAX_LIGHT);
            return 0;
            
        case DATA_TYPE_BELL:  //传值范围为0 or 1  铃铛亮:1，铃铛灭:0
            if (data == 1)
            {
                displayData[10] |= 0x80; //1000 0000
            } 
            else if (data == 0)
            {
                displayData[10] &= 0x7F; //0111 1111
            }
            else
            {
                return -1; // 参数错误
            }
            TM1729_WriteData(displayData, MAX_LIGHT);
            return 0;
            
        case DATA_TYPE_HOUR:    //传值范围为0-23
            if(data >= 0 && data <= 23) {
                int h = data / 10;
                int l = data % 10;
                //清空
                displayData[9] &= 0xF0; //1111 0000
                displayData[10] &= 0x80; //1000 0000
                displayData[11] &= 0x8F; //1000 1111


                displayData[9]  |= (sge_data_10_15[h+1] >> 4);
                displayData[10] |= ((sge_data_10_15[h+1] & 0x07) << 4);
                displayData[10] |= (sge_data_10_15[l+1] >> 4);
                displayData[11] |= ((sge_data_10_15[l+1] & 0x07) << 4);
            } else {
                return -1; // 参数错误
            }
            TM1729_WriteData(displayData, MAX_LIGHT);
            return 0;
            
        case DATA_TYPE_MINUTE:    //传值范围为0-59
            if(data >= 0 && data <= 59) {
                int h = data / 10;
                int l = data % 10; 
                //清空
                displayData[11] &= 0xF0; //1111 0000
                displayData[12] &= 0x80; //1000 0000
                displayData[13] &= 0x8F; //1000 1111

                displayData[11] |= (sge_data_10_15[h+1] >> 4);
                displayData[12] |= ((sge_data_10_15[h+1] & 0x07) << 4);
                displayData[12] |= (sge_data_10_15[l+1] >> 4);
                displayData[13] |= ((sge_data_10_15[l+1] & 0x07) << 4);
                
            } else {
                return -1; // 参数错误
            }
            TM1729_WriteData(displayData, MAX_LIGHT);
            return 0;
            
        case DATA_TYPE_SCORE:   // 传值范围为0-99
            if(data >= 0 && data <= 99) {
                int h = data / 10;
                int l = data % 10;
                //清空
                displayData[13] &= 0xF0; //1111 0000
                displayData[14] &= 0x80; //1000 0000
                displayData[15] &= 0x8F; //1000 1111

                displayData[13] |= (sge_data_10_15[h+1] >> 4);
                displayData[14] |= ((sge_data_10_15[h+1] & 0x07) << 4);
                displayData[14] |= (sge_data_10_15[l+1] >> 4);
                displayData[15] |= ((sge_data_10_15[l+1] & 0x07) << 4);
            } else {
                return -1; // 参数错误
            }
            TM1729_WriteData(displayData, MAX_LIGHT);
            return 0;
            
        case DATA_TYPE_BATTERY: //传值范围为：0-4
            if(data >=0 && data <=4) {
                if(data == 0){
                    displayData[4] &= 0x7F;    //0111 1111
                    displayData[15] &= 0xF2;   //1111 0010
                }
                if(data == 1){
                    displayData[4] |= 0x80;    //1000 0000
                    displayData[15] &= 0xF2;   //1111 0010
                }
                if(data == 2){
                    displayData[4] |= 0x80;    //1000 0000
                    displayData[15] &= 0xF2;   //1111 0010
                    displayData[15] |= 0x08;    //0000 0100
                }
                if(data == 3){
                    displayData[4] |= 0x80;    //1000 0000
                    displayData[15] &= 0xF2;   //1111 0010
                    displayData[15] |= 0x09;    //0000 1001
                }
                if(data == 4){
                    displayData[4] |= 0x80;    //1000 0000
                    displayData[15] &= 0xF2;   //1111 0010
                    displayData[15] |= 0x0D;    //0000 1101
                }
            } else {
                return -1; // 参数错误
            }
            TM1729_WriteData(displayData, MAX_LIGHT);
            return 0;
            
        case DATA_TYPE_TEMP1:   //传值范围为：-9-99
            if(data >= -9 && data <= 99){
                if(data < 0){
                    displayData[8] &= 0x80;
                    displayData[8] |= sge_data_1_9[0];
                } else {
                    int h = data / 10;
                    displayData[8] &= 0x80;
                    displayData[8] |= sge_data_1_9[h+1];
                }
                int l = (data < 0) ? -data : (data % 10);
                displayData[7] &= 0x80;     //1000 0000
                displayData[7] |= sge_data_1_9[l+1];
            }
            else {
                return -1; // 参数错误
            }
            TM1729_WriteData(displayData, MAX_LIGHT);
            return 0;
            
        case DATA_TYPE_TEMP2:   //传值范围为：-9-99
            if(data >= -9 && data <= 99){
                if(data < 0){
                    displayData[6] &= 0x80;     //1000 0000
                    displayData[6] |= sge_data_1_9[0];
                } else {
                    int h = data / 10;
                    displayData[6] &= 0x80;     //1000 0000
                    displayData[6] |= sge_data_1_9[h+1];
                }
                int l = (data < 0) ? -data : (data % 10);
                displayData[5] &= 0x80;     //1000 0000
                displayData[5] |= sge_data_1_9[l+1];
            }
            else {
                return -1; // 参数错误
            }
            TM1729_WriteData(displayData, MAX_LIGHT);
            return 0;
            
        case DATA_TYPE_HUMIDITY:   //传值范围为0-99
            if (data >= 0 && data <= 99) {
                int h = data / 10;
                int l = data % 10;
                displayData[4] &= 0x80;     //1000 0000
                displayData[4] |= sge_data_1_9[h+1];
                displayData[3] &= 0x80;     //1000 0000
                displayData[3] |= sge_data_1_9[l+1];
            }
            else {
                return -1; // 参数错误
            }
            TM1729_WriteData(displayData, MAX_LIGHT);
            return 0;
            
        case DATA_TYPE_CO2:     //传值范围为：0-999
            if(data >= 0 && data <= 999){
                int h = data / 100;
                int m = (data / 10) % 10;
                int l = data % 10;
                displayData[2] |= sge_data_1_9[h+1];
                displayData[1] |= sge_data_1_9[m+1];
                displayData[0] &= 0x80;     //1000 0000
                displayData[0] |= sge_data_1_9[l+1];
            } else {
                return -1; // 参数错误
            }
            TM1729_WriteData(displayData, MAX_LIGHT);
            return 0;
            
        default:
            return -2;
    }
}