#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 


#define FRAME_HEADER1      0X5A //Frame_header1 
#define FRAME_HEADER2      0XA5 //Frame_header2 
#define MAX_REC_LEN  			 4108 //һ�����������ճ���


#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
	  	
//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart_init(u32 bound);
void usart1_send(u8 data);
#endif


