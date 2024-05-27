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

u8 rec_sta=0;		//录音状态
					//[7]:0,没有开启录音;1,已经开启录音;
					//[6:1]:保留
					//[0]:0,正在录音;1,暂停录音;
u8 record_flag=-1;//缓存标志	
u8 play_flag=-1;//缓存标志	

u8 playRunVoice=0;
u8 readBuf[DMA_BUF_SIZE];
int main(void)
{ 
	u8 rval=0;
	int count=0,i=0;
	
	u32 baseAddrFlash=0;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);			//延时初始化
 
	uart_init(115200);		//初始化串口1波特率为115200 
	KEY_Init();
	LED_Init();					//初始化LED	
	
	POWER_ON=1;//打开电源控制 优先上电
	delay_ms(500);
	
	TIM3_Int_Init(999,83);//1ms定时器	
 
	W25QXX_Init();
	if(W25QXX_ReadID()==W25Q64)								//检测到W25Q64
	{
		printf("welcom to use!\n");
		//W25QXX_Erase_Chip();
	}
	
	
	WM8978_Init();				//初始化WM8978
	WM8978_HPvol_Set(50,50);	//耳机音量设置
	WM8978_SPKvol_Set(63);		//喇叭音量设置
	my_mem_init(SRAMIN);		//初始化内部内存池
	my_mem_init(SRAMCCM);		//初始化CCM内存池 

	i2srecbuf1=mymalloc(SRAMIN,DMA_BUF_SIZE);//I2S录音内存1申请
	i2srecbuf2=mymalloc(SRAMIN,DMA_BUF_SIZE);//I2S录音内存2申请  
			
	i2splaybuf1 = mymalloc(SRAMIN,DMA_BUF_SIZE);
	i2splaybuf2 =mymalloc(SRAMIN,DMA_BUF_SIZE);
	if(!i2srecbuf1||!i2srecbuf2||!i2splaybuf1||!i2splaybuf2)rval=1;
		 
	if(rval==0)		
	{
		recoder_enter_rec_mode();	//进入录音模式
		playRunVoice=1;//播放提示音
		baseAddrFlash=0*512*1024;
	}
		while(1){
			if(playRunVoice==1 && play_flag==1)
			{
					play_flag=-1;
				//读出音频
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
				playRunVoice=0;//停止播放
				count=0;
				delay_ms(1000);
				//播放完清空
			}
			
			
			delay_us(10);
		}			
		myfree(SRAMIN,i2srecbuf1);	//释放内存
		myfree(SRAMIN,i2srecbuf2);	//释放内存  
		myfree(SRAMIN,i2splaybuf1);	//释放内存
		myfree(SRAMIN,i2splaybuf2);	//释放内存 

		
}


 



