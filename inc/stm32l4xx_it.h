#ifndef STM32L4XX_H
#define STM32L4XX_H

#include "stm32l432xx.h"
#include "stm32l4xx_hal.h"

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
// void SVC_Handler(void);
void DebugMon_Handler(void);
// void PendSV_Handler(void);
// void SysTick_Handler(void);

#endif // STM32L4XX_H