#include "recorder.h" 
#include "malloc.h"
#include "usart.h"
#include "wm8978.h"
#include "i2s.h"
#include "led.h"
#include "delay.h"
#include "key.h"
#include "string.h" 

extern	u8 rec_sta;			
extern int16_t *i2srecbuf1;
extern int16_t *i2srecbuf2; 
extern int16_t EnCodeBufA[DMA_BUF_SIZE];
extern int16_t EnCodeBufB[DMA_BUF_SIZE];

extern int16_t *i2splaybuf1;
extern int16_t *i2splaybuf2;
extern u8 record_flag;
extern u8 play_flag;
//¼�� I2S_DMA�����жϷ�����.���ж�����д������
void i2s_dma_rx_callback(void) 
{    
	u16 bw;
	u8 res;
	if(rec_sta==0X80)//¼��ģʽ
	{  
		if(DMA1_Stream0->CR&(1<<19))
		{
			//memcpy ( i2splaybuf1, i2srecbuf1, DMA_BUF_SIZE);//��¼�Է�
			//memcpy ( &EnCodeBufA, &i2srecbuf1[0], DMA_BUF_SIZE);
			record_flag=0;
			
		}else
		{
			//memcpy ( i2splaybuf2, i2srecbuf2, DMA_BUF_SIZE);//��¼�Է�
			//memcpy ( &EnCodeBufB, &i2srecbuf2[0], DMA_BUF_SIZE );
			record_flag=1;
		}
	}
	else record_flag=-1;
}  

//WAV����ʱ,I2S DMA����ص�����
void i2s_dma_tx_callback(void) 
{   
	u16 i;
	if(DMA1_Stream7->CR&(1<<19))
	{
		play_flag=1;
	}else 
	{
		play_flag=0;
	}
} 



//����PCM ¼��ģʽ 		  
void recoder_enter_rec_mode(void)
{
	WM8978_ADDA_Cfg(1,1);		//����ADC
	//WM8978_Input_Cfg(1,1,0);	//��������ͨ��(MIC&LINE IN)��������
	WM8978_Output_Cfg(1,0);		//δ����BYPASS��� 
	WM8978_MIC_Gain(35);		//MIC�������� 
	
	
	WM8978_I2S_Cfg(2,0);		//�����ֱ�׼,16λ���ݳ���
	I2S3_Init(I2S_Standard_Phillips,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_16b);			//�����ֱ�׼,��������,ʱ�ӵ͵�ƽ��Ч,16λ֡���� 
	I2S3ext_Init(I2S_Standard_Phillips,I2S_Mode_SlaveRx,I2S_CPOL_Low,I2S_DataFormat_16b);		//�����ֱ�׼,�ӻ�����,ʱ�ӵ͵�ƽ��Ч,16λ֡����	
	I2S3_SampleRate_Set(16000);	//���ò����� 
 	I2S3_TX_DMA_Init( ); 		//����TX DMA 
	//DMA1_Stream7->CR&=~(1<<7);	//�رմ�������ж�(���ﲻ���ж�������) 
	I2S3ext_RX_DMA_Init( ); 	//����RX DMA
	i2s_rx_callback=i2s_dma_rx_callback;//�ص�����ָwav_i2s_dma_callback
	i2s_tx_callback=i2s_dma_tx_callback;			//�ص�����ָwav_i2s_dma_callback
 	
	I2S_Rec_Start(); 	//��ʼI2S���ݽ���(�ӻ�)
	I2S_Play_Start();	//��ʼI2S���ݷ���(����)
}  































