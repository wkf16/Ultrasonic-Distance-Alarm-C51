#ifndef __HC_SR04_H__
#define __HC_SR04_H__

#include <reg52.h>

// ���Ŷ���
sbit Trig = P2^0;  // HC-SR04��Trig����
sbit Echo = P2^1;  // HC-SR04��Echo����

// ��������
void HC_SR04_Init(void);                // ��ʼ��HC-SR04
unsigned int HC_SR04_GetDistance(void); // ��ȡ���룬���ص�λ��cm
void Delay_ms(unsigned int ms);         // ���뼶��ʱ����

#endif
