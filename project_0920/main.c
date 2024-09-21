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
	LCD_ShowString(1, 1, "AT_Init"); // 第一行显示 "Time:"
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
		distance = HC_SR04_GetDistance(); // 获取超声波测得的距离
		DS18B20_ConvertT();	//转换温度
		T=DS18B20_ReadT();	//读取温度
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
	    if(IR_GetDataFlag() || IR_GetRepeatFlag()) // 如果收到数据帧或者收到连发帧
	    {

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
				Buzzer_Time(50);
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
    // 提取小时
    DS1302_Time[3] = (pTime[0] - '0') * 10 + (pTime[1] - '0');  // 时
    // 提取分钟
    DS1302_Time[4] = (pTime[3] - '0') * 10 + (pTime[4] - '0');  // 分
    // 提取秒
    DS1302_Time[5] = (pTime[6] - '0') * 10 + (pTime[7] - '0');  // 秒
}

/**
  * @brief  串口中断服务函数，处理AT指令返回
  */
void USART() interrupt 4
{    
    static unsigned char i = 0;
    char *pTime;  // 用于指向时间部分的指针
    
    // 判断是否接收到数据
    if (RI == 1)
    {
        RI = 0;  // 清除接收标志位
        /*Data[i++] = SBUF;  // 将接收到的字节存储到 Data 数组中

        // 如果缓冲区满了，重置接收
        if (i >= sizeof(Data))
        {
            i = 0;
            memset(Data, 0x00, sizeof(Data));  // 清空缓冲区
            UART_SendString("Data overflow !\r\n");
        }
        
        // 检测到回车换行，表示一条完整的数据已接收
        if (Data[i - 1] == '\n' && Data[i - 2] == '\r')
        {
            Data[i] = '\0';  // 结束字符串
            
            // 查找 "+CIPSNTPTIME:" 并跳过它，直接读取日期和时间部分
            pTime = strstr(Data, "+CIPSNTPTIME:");
            if (pTime != NULL)
            {
                pTime = strchr(pTime, ' ');  // 跳到第一个空格
                if (pTime != NULL)
                {
                    pTime = strchr(pTime + 1, ' ');  // 跳过月份
                    if (pTime != NULL)
                    {
                        pTime = strchr(pTime + 1, ' ');  // 跳过日期部分到时间部分
                        if (pTime != NULL)
                        {
                            parseTime(pTime + 1);  // 调用手动解析函数解析时间

                            // 显示时间在LCD上
                            // LCD_ShowString(1, 1, (char*)pTime + 1);
							LCD_ShowString(2, 16, "!");
                        }
                    }
                }
            }
            
            // 清空接收缓冲区并重置索引
            memset(Data, 0x00, sizeof(Data));
            i = 0;
        } */
    }
}