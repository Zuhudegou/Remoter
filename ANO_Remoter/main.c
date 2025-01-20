/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "sysconfig.h"
#include "ANO_Param.h"
#include "ANO_Drv_Flash.h"
uint8_t 	NRF_ENABLE = 0;


//配置时钟
void RCC_Configuration(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1  | RCC_APB2Periph_GPIOA 
                          |RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC
													|RCC_APB2Periph_ADC1  | RCC_APB2Periph_AFIO 
                          |RCC_APB2Periph_SPI1  
													, ENABLE );
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3 | RCC_APB1Periph_I2C1| RCC_APB1Periph_TIM3, ENABLE );
}

u16 cnt_test;

int main(void)
{
	cnt_test=1;
	RCC_Configuration();	//配置时钟
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);		//中断优先级组别设置
	
	cnt_test=2;
	ANO_LED_Init();     //初始化LED	+ 蜂鸣器
	
	cnt_test=3;
	ANO_Param_READ();   //参数初始化			
	
	cnt_test=4;
	USB_HID_Init();	    //初始化USB	
	
	cnt_test=5;
	ANO_SPI_Init();	    //初始化NRF所用SPI	
	
	cnt_test=6;
	NRF_ENABLE = ANO_NRF_Check();//检查NRF连接是否正常	
	
	if(NRF_ENABLE)     //NRF24L01模块如果连接正常，则将NRF初始化为主发送模式
	{
		ANO_NRF_Init(MODEL_TX2,ANO_Param.NRF_Channel);  //设置频率信道
		Show.NRF_Err = 0;
	}
	else Show.NRF_Err = 1;
		
	cnt_test=7;
	ANO_LED_0_FLASH();//开机闪烁，以及蜂鸣器
	ADC1_Init();  		//初始化ADC采样	
	KEY_Init();				//按键初始化	
	OLED_Init();			//初始化屏幕
	
	cnt_test=8;
	//每2ms调用一次中断回调函数 SysTick_IRQ()
	SysTick_Config(GetSystemCoreClock() / 500);  //初始化系统滴答定时器,每次(1s/500=2ms)触发中断，，，SystemCoreClock是时钟频率，在system_stm32f10x.c定义
	
	while (1)                              //  注意！！！  全部运行程序在 ANO_Scheduler.c 程序里
	{																			 //  注意！！！  全部运行程序在 ANO_Scheduler.c 程序里
		if(Show.oled_delay)//50ms一次	  		 //  注意！！！  全部运行程序在 ANO_Scheduler.c 程序里
		{
			Show_Duty();  //显示程序
			Show.oled_delay=0;
			
			
			
			cnt_test++;
			if(cnt_test>=20){
				cnt_test=20;
			}
			
			
				
		}
	}
}
