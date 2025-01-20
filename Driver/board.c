#include "board.h"
#include "ANO_Scheduler.h"

static volatile uint32_t usTicks = 0;

uint32_t SysTick_count = 0; //10ms一次的计数器，并非1ms一次。只是名字相同而已
uint32_t SysTick_count_2ms = 0; //2ms一次的计数器

void cycleCounterInit(void)
{
    RCC_ClocksTypeDef clocks;
    RCC_GetClocksFreq(&clocks);
    usTicks = clocks.SYSCLK_Frequency / 1000000;
}

void Delay(vu32 nCount)
{
	for(; nCount!= 0;nCount--);
}

void SysTick_IRQ(void)
{
	SysTick_count_2ms++;//新增2ms计数器,,, SysTick_count用作10ms一次了
	Loop_check();

}

