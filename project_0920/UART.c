#include <REG52.H>
#include "UART.h"
#include "intrins.h"
#include <stdio.h>

/**
  * @brief  ���ڳ�ʼ����ʹ�ö�ʱ��2����115200bps������
  * @param  ��
  * @retval ��
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
  * @brief  ���ڷ���һ���ֽ�����
  * @param  Byte Ҫ���͵�һ���ֽ�����
  * @retval ��
  */
void UART_SendByte(unsigned char Byte)
{
	SBUF = Byte;			// �����ݼ��ص����ڷ��ͼĴ���
	while(!TI);			// �ȴ��������
	TI = 0;					// ���������ɱ�־λ
}

/**
  * @brief  ���ڷ����ַ���
  * @param  s Ҫ���͵��ַ���ָ��
  * @retval ��
  */
void UART_SendString(char *s)
{
    while(*s)				// ����ַ���������־
    {
        UART_SendByte(*s++); // ���͵�ǰ�ַ�
    }
}
