#ifndef __DS1302_H__
#define __DS1302_H__

#include <reg52.h>

// 引脚定义
sbit DS1302_SCLK = P3^6;
sbit DS1302_IO = P3^4;
sbit DS1302_CE = P3^5;
extern unsigned int DS1302_Time[7];

// 函数声明
void DS1302_Init(void); // DS1302初始化函数
void DS1302_WriteByte(unsigned char command, unsigned char Data);
unsigned char DS1302_ReadByte(unsigned char command);
void DS1302_SetTime(void); // 设置DS1302内部时间的函数
void DS1302_ReadTime(void); // 读取DS1302内部时间的函数
unsigned int BCD_to_Decimal(unsigned char);

#endif					   
