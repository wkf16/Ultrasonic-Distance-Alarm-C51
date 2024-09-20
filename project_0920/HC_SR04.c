#include "HC_SR04.h"
#include <reg52.h>

unsigned long Timer_Count = 0;  // �����������ڼ�¼Echo�ߵ�ƽ����ʱ��

// ��ʼ����ʱ��1
void Timer1_Init(void) {
    TMOD &= 0x0F;    // ���㶨ʱ��1�Ŀ���λ������λ��
    TMOD |= 0x10;    // ���ö�ʱ��1Ϊģʽ1��16λ��ʱ����
    TH1 = 0x00;      // ��ʱ��1��8λ����
    TL1 = 0x00;      // ��ʱ��1��8λ����
    TF1 = 0;         // �����ʱ��1�����־
    TR1 = 0;         // �رն�ʱ��1
	EA = 1;
}

// ΢����ʱ������������11.0592MHz����
void Delay_us(unsigned int us) {
    while (us--) {
        // ÿ��ѭ����Լ1us
        unsigned char i = 2;  // ������ֵ����Ӧ����Ƶ��
        while (--i);
    }
}

// ������ʱ����
void Delay_ms(unsigned int ms) {
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 112; j > 0; j--);
}

// ����Trig�����ź�
void HC_SR04_Trigger(void) {
    Trig = 1;         // Trig���Ÿߵ�ƽ
    Delay_us(20);     // ���ָߵ�ƽ20us��ȷ������10us��
    Trig = 0;         // Trig���ŵ͵�ƽ�����ͽ���
}

// ��ȡEcho���Ÿߵ�ƽ����ʱ�䣬��ת��Ϊ���루��λ��cm��
unsigned int HC_SR04_GetDistance(void) {
    unsigned int time_high = 0;
    unsigned long time = 0;
    unsigned int distance = 0;
    unsigned char TH1_temp = 0;
    unsigned char TL1_temp = 0;

    HC_SR04_Trigger(); // ����Trig����

    Timer1_Init();     // ��ʼ����ʱ��1
    while (!Echo);     // �ȴ�Echo���ű�ߣ���ʼ��ʱ��
    TR1 = 1;           // ������ʱ��1

    while (Echo) {     // ��Echo���ű��ָߵ�ƽʱ
        if (TF1) {     // ��鶨ʱ��1�Ƿ����
            TF1 = 0;   // ��������־
            Timer_Count++;  // �����������
            if (Timer_Count > 3) {  // ����������ʱ�䣨Լ23ms��
                TR1 = 0;    // ֹͣ��ʱ��1
                return 0xFFFF;  // ���س�����Χ��ֵ
            }
        }
    }
    TR1 = 0;           // ֹͣ��ʱ��1

    // ��ȡ��ʱֵ
    TH1_temp = TH1;
    TL1_temp = TL1;
    time_high = (TH1_temp << 8) | TL1_temp;  // �ϳ�16λ����ֵ
    time = ((unsigned long)Timer_Count << 16) + time_high;  // �����ܼ���ֵ

    // �������
    // ÿ������ֵ��Ӧ��ʱ��Ϊ1.085��s
    // ���루cm��=��ʱ�䣨us�� * ���٣�340m/s���� / 2 / 10000
    // �򻯹�ʽ�����루cm��=��ʱ�䣨����ֵ�� * 1.085 * 340�� / 2 / 10000
    // ���루cm��= ʱ�� * 0.018445
    // Ϊ���⸡�����㣬ת��Ϊ��
    distance = (unsigned int)((time * 18445) / 1000000);

    return distance;
}

// ��ʼ��HC-SR04
void HC_SR04_Init(void) {
    Trig = 0;  // ��ʼ��Trig����Ϊ�͵�ƽ
    Timer1_Init();  // ��ʼ����ʱ��1
}
