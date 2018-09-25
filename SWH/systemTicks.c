#include <stm32f10x_rcc.h>
#include "systemTicks.h"
#include "defines.h"

volatile uint32_t micros = 0;

void Systick_Init(void) {
    RCC_ClocksTypeDef RCC_Clocks;
    RCC_GetClocksFreq(&RCC_Clocks);

    SysTick_Config((RCC_Clocks.HCLK_Frequency / SYSTICK));
}

void InitSystemTicks(void) {
    micros = 0;
    Systick_Init();
}

void SysTick_Handler(void) {
    micros++;
}

#pragma GCC push_options
#pragma GCC optimize ("O0")
void _DelayUS(uint32_t aDelay) {
    volatile long now = micros;
    for(;;) {
        if (TIMEOUT(now, aDelay))
            break;
    }
}

#pragma GCC pop_options
