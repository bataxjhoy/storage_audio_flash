//#include "sys.h"
//#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"
#include "key.h"
#include "led.h"
#include "wm8978.h"
#include "recorder.h" 

#include "malloc.h"
#include "timer.h"	
#include "i2s.h"  
#include "w25qxx.h" 

signed short*    i2srecbuf1 ;
signed short*    i2srecbuf2 ; 
signed short*    i2splaybuf1 ;
signed short*    i2splaybuf2 ;

u8 rec_sta=0;		//¼��״̬
					//[7]:0,û�п���¼��;1,�Ѿ�����¼��;
					//[6:1]:����
					//[0]:0,����¼��;1,��ͣ¼��;
u8 record_flag=-1;//�����־	
u8 play_flag=-1;//�����־	

u8 playRunVoice=0;
u8 readBuf[DMA_BUF_SIZE];
int main(void)
{ 
	u8 rval=0;
	int count=0,i=0;
	
	u32 baseAddrFlash=0;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);			//��ʱ��ʼ��
 
	uart_init(115200);		//��ʼ������1������Ϊ115200 
	KEY_Init();
	LED_Init();					//��ʼ��LED	
	
	POWER_ON=1;//�򿪵�Դ���� �����ϵ�
	delay_ms(500);
	
	TIM3_Int_Init(999,83);//1ms��ʱ��	
 
	W25QXX_Init();
	if(W25QXX_ReadID()==W25Q64)								//��⵽W25Q64
	{
		printf("welcom to use!\n");
		//W25QXX_Erase_Chip();
	}
	
	
	WM8978_Init();				//��ʼ��WM8978
	WM8978_HPvol_Set(50,50);	//������������
	WM8978_SPKvol_Set(63);		//������������
	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
	my_mem_init(SRAMCCM);		//��ʼ��CCM�ڴ�� 

	i2srecbuf1=mymalloc(SRAMIN,DMA_BUF_SIZE);//I2S¼���ڴ�1����
	i2srecbuf2=mymalloc(SRAMIN,DMA_BUF_SIZE);//I2S¼���ڴ�2����  
			
	i2splaybuf1 = mymalloc(SRAMIN,DMA_BUF_SIZE);
	i2splaybuf2 =mymalloc(SRAMIN,DMA_BUF_SIZE);
	if(!i2srecbuf1||!i2srecbuf2||!i2splaybuf1||!i2splaybuf2)rval=1;
		 
	if(rval==0)		
	{
		recoder_enter_rec_mode();	//����¼��ģʽ
		playRunVoice=1;//������ʾ��
		baseAddrFlash=0*512*1024;
	}
		while(1){
			if(playRunVoice==1 && play_flag==1)
			{
					play_flag=-1;
				//������Ƶ
					W25QXX_Read(readBuf,baseAddrFlash+count*DMA_BUF_SIZE,DMA_BUF_SIZE);
					for( i=0;i<DMA_BUF_SIZE/2;i++)
					{
						i2splaybuf1[i]=readBuf[i*2] |(readBuf[i*2+1]<<8);//buf1
					}
					count++;					
			}
			else if(playRunVoice==1 && play_flag==0)
			{
					play_flag=-1;
					W25QXX_Read(readBuf,baseAddrFlash+count*DMA_BUF_SIZE,DMA_BUF_SIZE);
					for( i=0;i<DMA_BUF_SIZE/2;i++)
					{
						i2splaybuf2[i]=readBuf[i*2] |(readBuf[i*2+1]<<8);//buf2
					}
					count++;			
			}
			if(count==1000*1024/(DMA_BUF_SIZE))
			{
				playRunVoice=0;//ֹͣ����
				count=0;
				delay_ms(1000);
				//���������
			}
			
			
			delay_us(10);
		}			
		myfree(SRAMIN,i2srecbuf1);	//�ͷ��ڴ�
		myfree(SRAMIN,i2srecbuf2);	//�ͷ��ڴ�  
		myfree(SRAMIN,i2splaybuf1);	//�ͷ��ڴ�
		myfree(SRAMIN,i2splaybuf2);	//�ͷ��ڴ� 

		
}


 



