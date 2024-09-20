#include <REG52.H>
#include <stdio.h>
#include  "DS1302.h"
#include "LCD1602.h"
#include "Delay.h"
#include "HC_SR04.h"
#include "DS18B20.h"
#include "IR.h"

unsigned int distance;
sbit Buzzer = P2^5;

unsigned char ThrNum = 10;
unsigned char Address;
unsigned char Command;
unsigned char Command_old;

float T;

void menu(unsigned char, float);
void Delay_ms(unsigned int);
void LCD_ShowInt(unsigned char, unsigned char, int);
void Buzzer_Time(unsigned int ms);

void main()
{
	P3 = 0xFF;	
	LCD_Init();
	IR_Init();
	HC_SR04_Init();
	DS1302_Init();
	DS1302_SetTime();
	while(1)
	{
	    if(IR_GetDataFlag() || IR_GetRepeatFlag()) // ����յ�����֡�����յ�����֡
	    {
	        P3 = 0xFF;
	        Address = IR_GetAddress();    // ��ȡң������ַ��
	        Command = IR_GetCommand();    // ��ȡң����������
	
	        Delay(20);  // ��һ�����ݵ���ʱ����ȥ������20ms������ֹ��е����Ӱ��
	
	        // �ٴμ�鰴��״̬��ȷ������û����ȥ��ʱ���ڸı�
	        if(Command == IR_GetCommand()) {
	            if(Command == 0x52)       // ���ң����VOL-��������
	            {
	                ThrNum--;             // Num�Լ�
	                Command = Command_old;
	            }
	            if(Command == 0x18)       // ���ң����VOL+��������
	            {
	                ThrNum++;             // Num����
	                Command = Command_old;                        
	            }
	            LCD_WriteCommand(0x01);    // ��긴λ������
	
	            switch(Command){
	                case 0x45:
	                    DS1302_Init();
	                    break;
	                case 0x47:
	                    DS18B20_ConvertT(); // �ϵ���ת��һ���¶ȣ���ֹ��һ�ζ����ݴ���
	                    Delay(100);          
	                    break;
	            }
	
	            Command_old = Command;     // ��¼�ϴΰ���
	        }
	    }
	    menu(Command, T);
	}
}

void menu(unsigned char menu_option, float T) {
    switch (menu_option) {
        case 0x45:
            // �˵�1: ֱ����LCD1602����ʾʱ��
            DS1302_ReadTime(); // ��DS1302��ȡʱ��
            // ��ʾʱ����LCD1602 YY/MM/DD HH:MM:SS
            LCD_ShowString(1, 1, "Time:"); // ��һ����ʾ "Time:"
            LCD_ShowNum(1, 7, DS1302_Time[0], 2); // ��
            LCD_ShowChar(1, 9, '/');
            LCD_ShowNum(1, 10, DS1302_Time[1], 2); // ��
            LCD_ShowChar(1, 12, '/');
            LCD_ShowNum(1, 13, DS1302_Time[2], 2); // ��

            LCD_ShowNum(2, 1, DS1302_Time[3], 2); // Сʱ
            LCD_ShowChar(2, 3, ':');
            LCD_ShowNum(2, 4, DS1302_Time[4], 2); // ��
            LCD_ShowChar(2, 6, ':');
            LCD_ShowNum(2, 7, DS1302_Time[5], 2); // ��
            break;
													   
        case 0x46:
	    // �˵�2: ��ʾ�������������ľ���
	    distance = HC_SR04_GetDistance(); // ��ȡ��������õľ���
	
	    // ����Ƿ��������Ч��Χ��
	    if (distance != 0xFFFF) {
	        // ��ʾ��������õľ��룬��λΪcm
	        LCD_ShowString(1, 1, "Distance:"); 
	        LCD_ShowNum(1, 10, distance, 3);    
	        LCD_ShowString(1, 13, "cm");        
	
	        // ��ʾ��ֵThrNum�ڵڶ���
	        LCD_ShowString(2, 1, "Threshold:");
	        LCD_ShowNum(2, 11, ThrNum, 3);
	        LCD_ShowString(2, 14, "cm");
	
	        // �������С����ֵ��ִ���ض��Ĵ����߼�
	        if (distance < ThrNum) {
				LCD_ShowString(1, 16, "!");
	        } else {
			    LCD_ShowString(1, 16, " ");
	        }
	    } else {
	        // ���������Χ
	        LCD_ShowString(1, 1, "Out of Range");
	        // ����ڶ�����ʾ
	        LCD_ShowString(2, 1, "                "); // �ÿո�����ڶ���
	    }
			Delay(10);
            break;
		case 0x47:
			LCD_ShowString(1,1,"Temperature:");
			DS18B20_ConvertT();	//ת���¶�
			T=DS18B20_ReadT();	//��ȡ�¶�
			if(T<0)				//����¶�С��0
			{
				LCD_ShowChar(2,1,'-');	//��ʾ����
				T=-T;			//���¶ȱ�Ϊ����
			}
			else				//����¶ȴ��ڵ���0
			{
				LCD_ShowChar(2,1,'+');	//��ʾ����
			}
			LCD_ShowNum(2,2,T,3);		//��ʾ�¶���������
			LCD_ShowChar(2,5,'.');		//��ʾС����
			LCD_ShowNum(2,6,(unsigned long)(T*10000)%10000,4);//��ʾ�¶�С������
			break;
        default:
            // ��ʾ��Ч
			LCD_ShowString(1,1,"key_number:");
            LCD_ShowHexNum(2,5,menu_option,2);
            break;
    }
}
	/*
// ����������1000Hz��Ƶ����msʱ��
void Buzzer_Time(unsigned int ms)
{
	unsigned int i;
	for(i=0;i<ms*2;i++)//����ʱ�䣺ms��100����������100ms
	{
		Buzzer = !Buzzer;//���������ŵ�ƽ��ת����
		Delay_ms(1);//����Ƶ�ʣ�ÿ��500us��תһ��
		//�ߵ�ƽ500us���͵�ƽ500us��T = 1000us��f = 1/10^-3(s) = 1000HZ
	}
} */

void LCD_ShowInt(unsigned char Line, unsigned char Column, int Number) {
    char buffer[16];  // ���ڴ洢ת������ַ���
    sprintf(buffer, "%d", Number);  // ������ת��Ϊ�ַ���
    LCD_ShowString(Line, Column, buffer);  // ��ʾ�ַ���
}