#ifndef __HC_SR04_H__
#define __HC_SR04_H__

#include <reg52.h>

// 引脚定义
sbit Trig = P2^0;  // HC-SR04的Trig引脚
sbit Echo = P2^1;  // HC-SR04的Echo引脚

// 函数声明
void HC_SR04_Init(void);                // 初始化HC-SR04
unsigned int HC_SR04_GetDistance(void); // 获取距离，返回单位：cm
void Delay_ms(unsigned int ms);         // 毫秒级延时函数

#endif
