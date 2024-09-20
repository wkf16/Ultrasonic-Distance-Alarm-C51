
#include "DS1302.h"


// 对应寄存器位置定义
#define DS1302_SECOND   0x80
#define DS1302_MINUTE   0x82
#define DS1302_HOUR     0x84
#define DS1302_DATE     0x86
#define DS1302_MONTH    0x88
#define DS1302_DAY      0x8A
#define DS1302_YEAR     0x8C
#define DS1302_WP       0x8E

// 定义数组，用于存放设定的时间，顺序：年 月 日 时 分 秒 星期
unsigned int DS1302_Time[7] = {24, 9, 20, 19, 0, 59, 6};

// DS1302初始化
void DS1302_Init() {
    DS1302_CE = 0; // 关闭CE，低电平
    DS1302_SCLK = 0; // 时钟线拉低
}

// DS1302单字节写入函数
void DS1302_WriteByte(unsigned char command, unsigned char Data) {
    unsigned char i;
    DS1302_CE = 1; // 使能CE，拉高电平
    for (i = 0; i < 8; i++) { // 写入命令字节
        DS1302_IO = command & 0x01; // 低位先传输
        command >>= 1;
        DS1302_SCLK = 1;
        DS1302_SCLK = 0;
    }
    for (i = 0; i < 8; i++) { // 写入数据字节
        DS1302_IO = Data & 0x01; // 低位先传输
        Data >>= 1;
        DS1302_SCLK = 1;
        DS1302_SCLK = 0;
    }
    DS1302_CE = 0; // 结束通信
}

// DS1302单字节读取函数
unsigned char DS1302_ReadByte(unsigned char command) {
    unsigned char i, Data = 0x00;
    command |= 0x01; // 设置为读取指令
    DS1302_CE = 1; // 使能CE，拉高电平
    for (i = 0; i < 8; i++) { // 发送命令字节
        DS1302_IO = command & 0x01;
        command >>= 1;
        DS1302_SCLK = 1;
        DS1302_SCLK = 0;
    }
    DS1302_IO = 1; // 释放总线，准备接收数据
    for (i = 0; i < 8; i++) { // 读取数据字节
        DS1302_SCLK = 1;
        if (DS1302_IO) {
            Data |= (0x01 << i); // 低位先读
        }
        DS1302_SCLK = 0;
    }
    DS1302_CE = 0; // 结束通信
	DS1302_IO = 0;
    return Data;
}

// 设置DS1302时间
void DS1302_SetTime() {
    DS1302_WriteByte(DS1302_WP, 0x00); // 关闭写保护
    DS1302_WriteByte(DS1302_YEAR, (DS1302_Time[0] / 10 * 16) + (DS1302_Time[0] % 10)); // 设置年
    DS1302_WriteByte(DS1302_MONTH, (DS1302_Time[1] / 10 * 16) + (DS1302_Time[1] % 10)); // 设置月
    DS1302_WriteByte(DS1302_DATE, (DS1302_Time[2] / 10 * 16) + (DS1302_Time[2] % 10)); // 设置日
    DS1302_WriteByte(DS1302_HOUR, (DS1302_Time[3] / 10 * 16) + (DS1302_Time[3] % 10)); // 设置时
    DS1302_WriteByte(DS1302_MINUTE, (DS1302_Time[4] / 10 * 16) + (DS1302_Time[4] % 10)); // 设置分
    DS1302_WriteByte(DS1302_SECOND, (DS1302_Time[5] / 10 * 16) + (DS1302_Time[5] % 10)); // 设置秒
    DS1302_WriteByte(DS1302_DAY, (DS1302_Time[6] / 10 * 16) + (DS1302_Time[6] % 10)); // 设置星期
    DS1302_WriteByte(DS1302_WP, 0x80); // 启用写保护
}

// 读取DS1302时间
void DS1302_ReadTime() {
    unsigned int Temp;

    Temp = DS1302_ReadByte(DS1302_YEAR); // 读取年
    DS1302_Time[0] = BCD_to_Decimal(Temp); // BCD转十进制

    Temp = DS1302_ReadByte(DS1302_MONTH); // 读取月
    DS1302_Time[1] = BCD_to_Decimal(Temp); // BCD转十进制

    Temp = DS1302_ReadByte(DS1302_DATE); // 读取日
    DS1302_Time[2] = BCD_to_Decimal(Temp); // BCD转十进制

    Temp = DS1302_ReadByte(DS1302_HOUR); // 读取时
    DS1302_Time[3] = BCD_to_Decimal(Temp); // BCD转十进制

    Temp = DS1302_ReadByte(DS1302_MINUTE); // 读取分
    DS1302_Time[4] = BCD_to_Decimal(Temp); // BCD转十进制

    Temp = DS1302_ReadByte(DS1302_SECOND); // 读取秒
    DS1302_Time[5] = BCD_to_Decimal(Temp); // BCD转十进制

    Temp = DS1302_ReadByte(DS1302_DAY); // 读取星期
    DS1302_Time[6] = BCD_to_Decimal(Temp); // BCD转十进制
}

unsigned int BCD_to_Decimal(unsigned int bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F); // 高位 * 10 + 低位
}