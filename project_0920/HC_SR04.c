#include "HC_SR04.h"
#include <reg52.h>

unsigned long Timer_Count = 0;  // 计数器，用于记录Echo高电平持续时间

// 初始化定时器1
void Timer1_Init(void) {
    TMOD &= 0x0F;    // 清零定时器1的控制位（高四位）
    TMOD |= 0x10;    // 设置定时器1为模式1（16位定时器）
    TH1 = 0x00;      // 定时器1高8位清零
    TL1 = 0x00;      // 定时器1低8位清零
    TF1 = 0;         // 清除定时器1溢出标志
    TR1 = 0;         // 关闭定时器1
	EA = 1;
}

// 微秒延时函数（适用于11.0592MHz晶振）
void Delay_us(unsigned int us) {
    while (us--) {
        // 每个循环大约1us
        unsigned char i = 2;  // 调整此值以适应晶振频率
        while (--i);
    }
}

// 毫秒延时函数
void Delay_ms(unsigned int ms) {
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 112; j > 0; j--);
}

// 发送Trig脉冲信号
void HC_SR04_Trigger(void) {
    Trig = 1;         // Trig引脚高电平
    Delay_us(20);     // 保持高电平20us（确保大于10us）
    Trig = 0;         // Trig引脚低电平，发送结束
}

// 读取Echo引脚高电平持续时间，并转换为距离（单位：cm）
unsigned int HC_SR04_GetDistance(void) {
    unsigned int time_high = 0;
    unsigned long time = 0;
    unsigned int distance = 0;
    unsigned char TH1_temp = 0;
    unsigned char TL1_temp = 0;

    HC_SR04_Trigger(); // 发送Trig脉冲

    Timer1_Init();     // 初始化定时器1
    while (!Echo);     // 等待Echo引脚变高（开始计时）
    TR1 = 1;           // 启动定时器1

    while (Echo) {     // 当Echo引脚保持高电平时
        if (TF1) {     // 检查定时器1是否溢出
            TF1 = 0;   // 清除溢出标志
            Timer_Count++;  // 计数溢出次数
            if (Timer_Count > 3) {  // 超过最大测量时间（约23ms）
                TR1 = 0;    // 停止定时器1
                return 0xFFFF;  // 返回超出范围的值
            }
        }
    }
    TR1 = 0;           // 停止定时器1

    // 获取计时值
    TH1_temp = TH1;
    TL1_temp = TL1;
    time_high = (TH1_temp << 8) | TL1_temp;  // 合成16位计数值
    time = ((unsigned long)Timer_Count << 16) + time_high;  // 计算总计数值

    // 计算距离
    // 每个计数值对应的时间为1.085μs
    // 距离（cm）=（时间（us） * 声速（340m/s）） / 2 / 10000
    // 简化公式：距离（cm）=（时间（计数值） * 1.085 * 340） / 2 / 10000
    // 距离（cm）= 时间 * 0.018445
    // 为避免浮点运算，转换为：
    distance = (unsigned int)((time * 18445) / 1000000);

    return distance;
}

// 初始化HC-SR04
void HC_SR04_Init(void) {
    Trig = 0;  // 初始化Trig引脚为低电平
    Timer1_Init();  // 初始化定时器1
}
