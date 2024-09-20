#ifndef __DS1302_H__
#define __DS1302_H__

#include <reg52.h>

// ���Ŷ���
sbit DS1302_SCLK = P3^6;
sbit DS1302_IO = P3^4;
sbit DS1302_CE = P3^5;
extern unsigned int DS1302_Time[7];

// ��������
void DS1302_Init(void); // DS1302��ʼ������
void DS1302_WriteByte(unsigned char command, unsigned char Data);
unsigned char DS1302_ReadByte(unsigned char command);
void DS1302_SetTime(void); // ����DS1302�ڲ�ʱ��ĺ���
void DS1302_ReadTime(void); // ��ȡDS1302�ڲ�ʱ��ĺ���
unsigned int BCD_to_Decimal(unsigned char);

#endif					   
