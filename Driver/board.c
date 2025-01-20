#include "board.h"
#include "ANO_Scheduler.h"

static volatile uint32_t usTicks = 0;

uint32_t SysTick_count = 0; //10msһ�εļ�����������1msһ�Ρ�ֻ��������ͬ����
uint32_t SysTick_count_2ms = 0; //2msһ�εļ�����

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
	SysTick_count_2ms++;//����2ms������,,, SysTick_count����10msһ����
	Loop_check();

}

