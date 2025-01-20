#ifndef __BOARD_H__
#define __BOARD_H__
#include "stm32f10x.h"
#include "ANO_Drv_Uart.h"
#include "ANO_Drv_Uart3.h"
#include "ANO_Drv_SPI.h"
#include "ANO_Drv_Nrf24l01.h"
#include "ANO_Drv_LED.h"
#include "ANO_Drv_hid.h"
#include "ANO_Drv_ADC.h"
#include "ANO_DT.h"

//#define GPIO_Remap_SWJ_JTAGDisable  ((uint32_t)0x00300200)  /*!< JTAG-DP Disabled and SW-DP Enabled */

#define HW_TYPE	1
#define HW_VER	3
#define BL_VER	100
#define PT_VER	400
/***************LED GPIO定义******************/
//这个是红色LED，PB9现用于按键SET键 
#define ANO_RCC_LED_0					RCC_APB2Periph_GPIOB

//LED2
#define ANO_GPIO_LED_0				GPIOB
//#define ANO_Pin_LED_0				GPIO_Pin_9 	//b9用作 按键set
#define ANO_Pin_LED_0					GPIO_Pin_13 //LED2: B13

//LED1
//这个实际是蓝色LED,用于指示与飞控是否有连接
#define LED_Red_GPIO		      GPIOB
#define LED_Red_Pin		        GPIO_Pin_1			//LED1: B1


//蓝色用于指示与飞控是否有连接
#define ANO_LED_blue_ON  		GPIO_SetBits(LED_Red_GPIO, LED_Red_Pin)
#define ANO_LED_blue_OFF 		GPIO_ResetBits(LED_Red_GPIO, LED_Red_Pin)

//#define LED_Red_On   		GPIO_ResetBits(ANO_GPIO_LED_0, ANO_Pin_LED_0)
//#define LED_Red_OFF  		GPIO_SetBits(ANO_GPIO_LED_0, ANO_Pin_LED_0)

//红色用于按键响应，跟蜂鸣器同时动作，这个io可以与蜂鸣器io共用
#define LED_Red_On   				GPIO_SetBits(ANO_GPIO_LED_0, ANO_Pin_LED_0)
#define LED_Red_OFF  				GPIO_ResetBits(ANO_GPIO_LED_0, ANO_Pin_LED_0)

//#define LED_Red_On   		//取消，与蜂鸣器io共用		
//#define LED_Red_OFF  		//取消，与蜂鸣器io共用		


/*********************************************/
/***************蜂鸣器 GPIO定义******************/

//#define BEEP_P              GPIOC
//#define BEEP		            GPIO_Pin_13

#define BEEP_P              GPIOB
#define BEEP		            GPIO_Pin_10

#define BEEP_L          GPIO_ResetBits(BEEP_P, BEEP)
#define BEEP_H          GPIO_SetBits(BEEP_P, BEEP)



/*********************************************/
/***************I2C GPIO定义******************/
//#define ANO_GPIO_I2C	GPIOC  //bug
#define ANO_GPIO_I2C	GPIOB

//#define I2C_Pin_SCL		GPIO_Pin_14
//#define I2C_Pin_SDA		GPIO_Pin_15

//SCL(B14) 和 K1(B8) 对调
//SDA(B15) 和 K2(B7) 对调

#define I2C_Pin_SCL		GPIO_Pin_8
#define I2C_Pin_SDA		GPIO_Pin_7


#define ANO_RCC_I2C		RCC_APB2Periph_GPIOB
/*********************************************/
/***************UART1 GPIO定义******************/
#define ANO_RCC_UART1			RCC_APB2Periph_GPIOA
#define ANO_GPIO_UART1		GPIOA
#define ANO_UART1_Pin_TX	GPIO_Pin_9
#define ANO_UART1_Pin_RX	GPIO_Pin_10
/*********************************************/
/***************SPI GPIO定义******************/
#define ANO_GPIO_SPI		GPIOA
#define RCC_GPIO_SPI		RCC_APB2Periph_GPIOA
#define SPI_Pin_SCK			GPIO_Pin_5
#define SPI_Pin_MISO		GPIO_Pin_6
#define SPI_Pin_MOSI		GPIO_Pin_7

#define NRF_CE_GPIO		GPIOA		
#define NRF_CE_Pin		GPIO_Pin_15	
#define NRF_CSN_GPIO	GPIOA		
#define NRF_CSN_Pin		GPIO_Pin_4	
#define NRF_IRQ_GPIO	GPIOB
#define NRF_IRQ_Pin		GPIO_Pin_1		//这个引脚实际并未使用，板子实际连接的是A8

/*********************************************/
/***************硬件中断优先级******************/
#define NVIC_UART_P	5
#define NVIC_UART_S	1
/***********************************************/
extern uint32_t SysTick_count; //10ms一次的计数器
extern uint32_t SysTick_count_2ms; //2ms一次的计数器

void Delay(vu32 nCount);
void cycleCounterInit(void);
void SysTick_IRQ(void);

#endif 
