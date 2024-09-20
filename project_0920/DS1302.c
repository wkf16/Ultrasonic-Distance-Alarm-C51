
#include "DS1302.h"


// ��Ӧ�Ĵ���λ�ö���
#define DS1302_SECOND   0x80
#define DS1302_MINUTE   0x82
#define DS1302_HOUR     0x84
#define DS1302_DATE     0x86
#define DS1302_MONTH    0x88
#define DS1302_DAY      0x8A
#define DS1302_YEAR     0x8C
#define DS1302_WP       0x8E

// �������飬���ڴ���趨��ʱ�䣬˳���� �� �� ʱ �� �� ����
unsigned int DS1302_Time[7] = {24, 9, 20, 19, 0, 59, 6};

// DS1302��ʼ��
void DS1302_Init() {
    DS1302_CE = 0; // �ر�CE���͵�ƽ
    DS1302_SCLK = 0; // ʱ��������
}

// DS1302���ֽ�д�뺯��
void DS1302_WriteByte(unsigned char command, unsigned char Data) {
    unsigned char i;
    DS1302_CE = 1; // ʹ��CE�����ߵ�ƽ
    for (i = 0; i < 8; i++) { // д�������ֽ�
        DS1302_IO = command & 0x01; // ��λ�ȴ���
        command >>= 1;
        DS1302_SCLK = 1;
        DS1302_SCLK = 0;
    }
    for (i = 0; i < 8; i++) { // д�������ֽ�
        DS1302_IO = Data & 0x01; // ��λ�ȴ���
        Data >>= 1;
        DS1302_SCLK = 1;
        DS1302_SCLK = 0;
    }
    DS1302_CE = 0; // ����ͨ��
}

// DS1302���ֽڶ�ȡ����
unsigned char DS1302_ReadByte(unsigned char command) {
    unsigned char i, Data = 0x00;
    command |= 0x01; // ����Ϊ��ȡָ��
    DS1302_CE = 1; // ʹ��CE�����ߵ�ƽ
    for (i = 0; i < 8; i++) { // ���������ֽ�
        DS1302_IO = command & 0x01;
        command >>= 1;
        DS1302_SCLK = 1;
        DS1302_SCLK = 0;
    }
    DS1302_IO = 1; // �ͷ����ߣ�׼����������
    for (i = 0; i < 8; i++) { // ��ȡ�����ֽ�
        DS1302_SCLK = 1;
        if (DS1302_IO) {
            Data |= (0x01 << i); // ��λ�ȶ�
        }
        DS1302_SCLK = 0;
    }
    DS1302_CE = 0; // ����ͨ��
	DS1302_IO = 0;
    return Data;
}

// ����DS1302ʱ��
void DS1302_SetTime() {
    DS1302_WriteByte(DS1302_WP, 0x00); // �ر�д����
    DS1302_WriteByte(DS1302_YEAR, (DS1302_Time[0] / 10 * 16) + (DS1302_Time[0] % 10)); // ������
    DS1302_WriteByte(DS1302_MONTH, (DS1302_Time[1] / 10 * 16) + (DS1302_Time[1] % 10)); // ������
    DS1302_WriteByte(DS1302_DATE, (DS1302_Time[2] / 10 * 16) + (DS1302_Time[2] % 10)); // ������
    DS1302_WriteByte(DS1302_HOUR, (DS1302_Time[3] / 10 * 16) + (DS1302_Time[3] % 10)); // ����ʱ
    DS1302_WriteByte(DS1302_MINUTE, (DS1302_Time[4] / 10 * 16) + (DS1302_Time[4] % 10)); // ���÷�
    DS1302_WriteByte(DS1302_SECOND, (DS1302_Time[5] / 10 * 16) + (DS1302_Time[5] % 10)); // ������
    DS1302_WriteByte(DS1302_DAY, (DS1302_Time[6] / 10 * 16) + (DS1302_Time[6] % 10)); // ��������
    DS1302_WriteByte(DS1302_WP, 0x80); // ����д����
}

// ��ȡDS1302ʱ��
void DS1302_ReadTime() {
    unsigned int Temp;

    Temp = DS1302_ReadByte(DS1302_YEAR); // ��ȡ��
    DS1302_Time[0] = BCD_to_Decimal(Temp); // BCDתʮ����

    Temp = DS1302_ReadByte(DS1302_MONTH); // ��ȡ��
    DS1302_Time[1] = BCD_to_Decimal(Temp); // BCDתʮ����

    Temp = DS1302_ReadByte(DS1302_DATE); // ��ȡ��
    DS1302_Time[2] = BCD_to_Decimal(Temp); // BCDתʮ����

    Temp = DS1302_ReadByte(DS1302_HOUR); // ��ȡʱ
    DS1302_Time[3] = BCD_to_Decimal(Temp); // BCDתʮ����

    Temp = DS1302_ReadByte(DS1302_MINUTE); // ��ȡ��
    DS1302_Time[4] = BCD_to_Decimal(Temp); // BCDתʮ����

    Temp = DS1302_ReadByte(DS1302_SECOND); // ��ȡ��
    DS1302_Time[5] = BCD_to_Decimal(Temp); // BCDתʮ����

    Temp = DS1302_ReadByte(DS1302_DAY); // ��ȡ����
    DS1302_Time[6] = BCD_to_Decimal(Temp); // BCDתʮ����
}

unsigned int BCD_to_Decimal(unsigned int bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F); // ��λ * 10 + ��λ
}