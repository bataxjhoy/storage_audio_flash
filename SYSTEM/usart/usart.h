#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 


#define FRAME_HEADER1      0X5A //Frame_header1 
#define FRAME_HEADER2      0XA5 //Frame_header2 
#define MAX_REC_LEN  			 4108 //一包数据最大接收长度


#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
	  	
//如果想串口中断接收，请不要注释以下宏定义
void uart_init(u32 bound);
void usart1_send(u8 data);
#endif


