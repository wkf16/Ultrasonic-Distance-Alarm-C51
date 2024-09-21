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

unsigned int distance;
sbit Buzzer = P2^5;

unsigned char ThrNum = 10;
unsigned char Address;
unsigned char Command;
unsigned char Command_old;
unsigned int Loop_count = 0;
unsigned char Data[16];
void AT_Init(void);

float T;

char putchar(char dat);
void menu(unsigned char, float);
void Delay_ms(unsigned int);
void LCD_ShowInt(unsigned char, unsigned char, int);
void Buzzer_Time(unsigned int ms);   

void main()
{
	UART_Init();
	LCD_Init();
	LCD_ShowString(1, 1, "AT_Init"); // 第一行显示 "Time:"
	Delay(500);
	AT_Init();
	P3 |= 0xFC;	
	
	IR_Init();
	HC_SR04_Init();
	DS1302_Init();
	DS1302_SetTime();
	while(1)
	{
		Loop_count++;
		distance = HC_SR04_GetDistance(); // 获取超声波测得的距离
		DS18B20_ConvertT();	//转换温度
		T=DS18B20_ReadT();	//读取温度
		//printf("%d\r\n", Loop_count);
		if (Loop_count > 20){
			//UART_Init();
			Loop_count = 0;
			printf("AT+MQTTPUB=0,\"/sys/k1ryxzSF8vb/0c4qP0fwXkAUsGu2Mfhd/thing/event/property/post\",\"{\\\"params\\\":{\\\"DetectDistance\\\":%d}}\",1,0", distance);
			printf("\r\n");
		}
	    if(IR_GetDataFlag() || IR_GetRepeatFlag()) // 如果收到数据帧或者收到连发帧
	    {
	        Address = IR_GetAddress();    // 获取遥控器地址码
	        Command = IR_GetCommand();    // 获取遥控器命令码
	
	        Delay(80);  // 加一个短暂的延时用于去抖动，防止机械抖动影响
	
	        // 再次检查按键状态，确保按键没有在去抖时间内改变
	        if(Command == IR_GetCommand()) {
	            if(Command == 0x52)       // 如果遥控器VOL-按键按下
	            {
	                ThrNum--;             // Num自减
	                Command = Command_old;
	            }
	            if(Command == 0x18)       // 如果遥控器VOL+按键按下
	            {
	                ThrNum++;             // Num自增
	                Command = Command_old;                        
	            }
	            LCD_WriteCommand(0x01);    // 光标复位，清屏
	
	            switch(Command){
	                case 0x45:
	                    DS1302_Init();
	                    break;
	                case 0x47:
						P3 |= 0xFC;
	                    DS18B20_ConvertT(); // 上电先转换一次温度，防止第一次读数据错误
	                    Delay(100);          
	                    break;
	            }
	
	            Command_old = Command;     // 记录上次按键
	        }
	    }
	    menu(Command, T);
	}
}  

void menu(unsigned char menu_option, float T) {
    switch (menu_option) {
        case 0x45:
            // 菜单1: 直接在LCD1602上显示时间
            DS1302_ReadTime(); // 从DS1302读取时间
            // 显示时间在LCD1602 YY/MM/DD HH:MM:SS
            LCD_ShowString(1, 1, "Time:"); // 第一行显示 "Time:"
            LCD_ShowNum(1, 7, DS1302_Time[0], 2); // 年
            LCD_ShowChar(1, 9, '/');
            LCD_ShowNum(1, 10, DS1302_Time[1], 2); // 月
            LCD_ShowChar(1, 12, '/');
            LCD_ShowNum(1, 13, DS1302_Time[2], 2); // 日

            LCD_ShowNum(2, 1, DS1302_Time[3], 2); // 小时
            LCD_ShowChar(2, 3, ':');
            LCD_ShowNum(2, 4, DS1302_Time[4], 2); // 分
            LCD_ShowChar(2, 6, ':');
            LCD_ShowNum(2, 7, DS1302_Time[5], 2); // 秒
            break;
													   
        case 0x46:
	    // 菜单2: 显示超声波传感器的距离
	
	    // 检查是否距离在有效范围内
	    if (distance != 0xFFFF) {
	        // 显示超声波测得的距离，单位为cm
	        LCD_ShowString(1, 1, "Distance:"); 
	        LCD_ShowNum(1, 10, distance, 3);    
	        LCD_ShowString(1, 13, "cm");        
	
	        // 显示阈值ThrNum在第二行
	        LCD_ShowString(2, 1, "Threshold:");
	        LCD_ShowNum(2, 11, ThrNum, 3);
	        LCD_ShowString(2, 14, "cm");
	
	        // 如果距离小于阈值，执行特定的处理逻辑
	        if (distance < ThrNum) {
				LCD_ShowString(1, 16, "!");
	        } else {
			    LCD_ShowString(1, 16, " ");
	        }
	    } else {
	        // 如果超出范围
	        LCD_ShowString(1, 1, "Out of Range");
	        // 清除第二行显示
	        LCD_ShowString(2, 1, "                "); // 用空格清除第二行
	    }
			Delay(10);
            break;
		case 0x47:
			LCD_ShowString(1,1,"Temperature:");
			if(T<0)				//如果温度小于0
			{
				LCD_ShowChar(2,1,'-');	//显示负号
				T=-T;			//将温度变为正数
			}
			else				//如果温度大于等于0
			{
				LCD_ShowChar(2,1,'+');	//显示正号
			}
			LCD_ShowNum(2,2,T,3);		//显示温度整数部分
			LCD_ShowChar(2,5,'.');		//显示小数点
			LCD_ShowNum(2,6,(unsigned long)(T*10000)%10000,4);//显示温度小数部分
			break;
        default:
            // 显示无效
			LCD_ShowString(1,1,"key_number:");
            LCD_ShowHexNum(2,5,menu_option,2);
            break;
    }
}
	
// 蜂鸣器按照1000Hz的频率响ms时间
void Buzzer_Time(unsigned int ms)
{
	unsigned int i;
	for(i=0;i<ms*2;i++)//控制时间：ms给100，蜂鸣器响100ms
	{
		Buzzer = !Buzzer;//蜂鸣器引脚电平翻转发声
		Delay_ms(1);//控制频率：每隔500us翻转一次
		//高电平500us，低电平500us，T = 1000us，f = 1/10^-3(s) = 1000HZ
	}
} 



void AT_Init(void)
{
    printf("AT+CIPSNTPCFG=1,8,\"cn.ntp.org.cn\",\"ntp.sjtu.edu.cn\"");
	printf("\r\n");
	Delay(500);
    printf("AT+MQTTUSERCFG=0,1,\"NULL\",\"0c4qP0fwXkAUsGu2Mfhd&k1ryxzSF8vb\",\"80c67c1aadd0037752bae746b772036605b600c19c9025f12746757286b9e99b\",0,0,\"\"");
	printf("\r\n");
	Delay(500);
    printf("AT+MQTTCLIENTID=0,\"k1ryxzSF8vb.0c4qP0fwXkAUsGu2Mfhd|securemode=2\\,signmethod=hmacsha256\\,timestamp=1726867041527|\"");
	printf("\r\n");
	Delay(500);
    printf("AT+MQTTCONN=0,\"iot-06z00acui5vtoxn.mqtt.iothub.aliyuncs.com\",1883,1");
	printf("\r\n");
	Delay(500);
}	 	
char putchar(char dat)
{
	UART_SendByte(dat);           
	return dat;                    
}
void USART( ) interrupt  4
{    
    static unsigned char i=0;
	if(RI==1)
	{	RI=0;
		if(i>=16)   //收到字符串超过数组长度
		{ 
			i=0;
			UART_SendString("Data overflow !\r\n");
			memset(Data,0x00,sizeof(Data)); //数组清零
		}
        else
        {
			Data[i++]=SBUF;
			if( (Data[i-1]=='\n')  && (Data[i-2]=='\r') )  //判断结尾字符为回车换行符
			{
				UART_SendString(Data) ;   //将收到的内容发送出去
                memset(Data,0x00,sizeof(Data));//数组清零
                i=0;                 
			}
        }
	}   
}