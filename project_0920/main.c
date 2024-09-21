#include <REG52.H>
#include <stdio.h>
#include  "DS1302.h"
#include "LCD1602.h"
#include "Delay.h"
#include "HC_SR04.h"
#include "DS18B20.h"
#include "IR.h"
#include "UART.h"
#include "string.h"
#include <stdlib.h>

unsigned int pdata distance;
sbit Buzzer = P2^5;

unsigned char ThrNum = 10;
unsigned char Command;
unsigned char Command_old;
unsigned int Loop_count = 0;
//unsigned char xdata  Data[48];
bit timeReceived = 0;
void AT_Init(void);

float T;

char putchar(char dat);
void menu(unsigned char, float);
void Delay_ms(unsigned int);
void LCD_ShowInt(unsigned char, unsigned char, int);
void Buzzer_Time(unsigned int);
void parseTime(char*);   

void main()
{
	LCD_Init();
	LCD_ShowString(1, 1, "AT_Init"); // ��һ����ʾ "Time:"
	Delay(10000);
	UART_Init();
	AT_Init();
	P3 |= 0xFC;	
	
	IR_Init();
	HC_SR04_Init();
	DS1302_Init();
	DS1302_SetTime();
	while(1)
	{
		Loop_count++;
		distance = HC_SR04_GetDistance(); // ��ȡ��������õľ���
		DS18B20_ConvertT();	//ת���¶�
		T=DS18B20_ReadT();	//��ȡ�¶�
		//printf("%d\r\n", Loop_count);
		if (Loop_count > 20){
			//UART_Init();
			Loop_count = 0;
			printf("AT+MQTTPUB=0,\"/sys/k1ryxzSF8vb/0c4qP0fwXkAUsGu2Mfhd/thing/event/property/post\",\"{\\\"params\\\":{\\\"DetectDistance\\\":%d}}\",1,0\r\n", distance);
			LCD_ShowString(2, 16, "!");
			Delay(500);
			printf("AT+CIPSNTPTIME?\r\n");
	    } else {
			LCD_ShowString(2, 16, " ");
	    }
	    if(IR_GetDataFlag() || IR_GetRepeatFlag()) // ����յ�����֡�����յ�����֡
	    {

	        Command = IR_GetCommand();    // ��ȡң����������
	
	        Delay(80);  // ��һ�����ݵ���ʱ����ȥ��������ֹ��е����Ӱ��
	
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
						P3 |= 0xFC;
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
				Buzzer_Time(50);
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
} 



void AT_Init(void)
{
	printf("AT+CIPSNTPCFG=1,8,\"cn.pool.ntp.org\",\"ntp1.aliyun.com\"\r\n");
	Delay(300);
    printf("AT+CIPSNTPCFG=1,8,\"cn.ntp.org.cn\",\"ntp.sjtu.edu.cn\"\r\n");
	Delay(300);
    printf("AT+MQTTUSERCFG=0,1,\"NULL\",\"0c4qP0fwXkAUsGu2Mfhd&k1ryxzSF8vb\",\"80c67c1aadd0037752bae746b772036605b600c19c9025f12746757286b9e99b\",0,0,\"\"\r\n");
	Delay(300);
    printf("AT+MQTTCLIENTID=0,\"k1ryxzSF8vb.0c4qP0fwXkAUsGu2Mfhd|securemode=2\\,signmethod=hmacsha256\\,timestamp=1726867041527|\"\r\n");
	Delay(300);
    printf("AT+MQTTCONN=0,\"iot-06z00acui5vtoxn.mqtt.iothub.aliyuncs.com\",1883,1\r\n");
	Delay(3000);
}	 	
char putchar(char dat)
{
	UART_SendByte(dat);           
	return dat;                    
}

void parseTime(char *pTime)
{
    // ��ȡСʱ
    DS1302_Time[3] = (pTime[0] - '0') * 10 + (pTime[1] - '0');  // ʱ
    // ��ȡ����
    DS1302_Time[4] = (pTime[3] - '0') * 10 + (pTime[4] - '0');  // ��
    // ��ȡ��
    DS1302_Time[5] = (pTime[6] - '0') * 10 + (pTime[7] - '0');  // ��
}

/**
  * @brief  �����жϷ�����������ATָ���
  */
void USART() interrupt 4
{    
    static unsigned char i = 0;
    char *pTime;  // ����ָ��ʱ�䲿�ֵ�ָ��
    
    // �ж��Ƿ���յ�����
    if (RI == 1)
    {
        RI = 0;  // ������ձ�־λ
        /*Data[i++] = SBUF;  // �����յ����ֽڴ洢�� Data ������

        // ������������ˣ����ý���
        if (i >= sizeof(Data))
        {
            i = 0;
            memset(Data, 0x00, sizeof(Data));  // ��ջ�����
            UART_SendString("Data overflow !\r\n");
        }
        
        // ��⵽�س����У���ʾһ�������������ѽ���
        if (Data[i - 1] == '\n' && Data[i - 2] == '\r')
        {
            Data[i] = '\0';  // �����ַ���
            
            // ���� "+CIPSNTPTIME:" ����������ֱ�Ӷ�ȡ���ں�ʱ�䲿��
            pTime = strstr(Data, "+CIPSNTPTIME:");
            if (pTime != NULL)
            {
                pTime = strchr(pTime, ' ');  // ������һ���ո�
                if (pTime != NULL)
                {
                    pTime = strchr(pTime + 1, ' ');  // �����·�
                    if (pTime != NULL)
                    {
                        pTime = strchr(pTime + 1, ' ');  // �������ڲ��ֵ�ʱ�䲿��
                        if (pTime != NULL)
                        {
                            parseTime(pTime + 1);  // �����ֶ�������������ʱ��

                            // ��ʾʱ����LCD��
                            // LCD_ShowString(1, 1, (char*)pTime + 1);
							LCD_ShowString(2, 16, "!");
                        }
                    }
                }
            }
            
            // ��ս��ջ���������������
            memset(Data, 0x00, sizeof(Data));
            i = 0;
        } */
    }
}