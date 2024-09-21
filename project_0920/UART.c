#include <REG52.H>
#include "UART.h"
#include "intrins.h"
#include <stdio.h>

/**
  * @brief  串口初始化，使用定时器2生成115200bps波特率
  * @param  无
  * @retval 无
  */
void UART_Init()		//115200bps@11.0592MHz
{
   SCON=0x50;   
   TH2=0xFF;           
   TL2=0xFD;   
   RCAP2H=0xFF;   
   RCAP2L=0xFD; 

/*****************/
   TCLK=1;   
   RCLK=1;   
   C_T2=0;   
   EXEN2=0;

/*****************/
   TR2=1 ;
   TI = 1;
   ES=1;
   EA=1;
}

/**
  * @brief  串口发送一个字节数据
  * @param  Byte 要发送的一个字节数据
  * @retval 无
  */
void UART_SendByte(unsigned char Byte)
{
	SBUF = Byte;			// 将数据加载到串口发送寄存器
	while(!TI);			// 等待发送完成
	TI = 0;					// 清除发送完成标志位
}

/**
  * @brief  串口发送字符串
  * @param  s 要发送的字符串指针
  * @retval 无
  */
void UART_SendString(char *s)
{
    while(*s)				// 检查字符串结束标志
    {
        UART_SendByte(*s++); // 发送当前字符
    }
}
