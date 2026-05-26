/*
 * peripheral_setup.h
 *
 *  Created on: Apr 17, 2026
 *      Author: serhatt
 */

#ifndef SRC_RCC_PERIPHERAL_SETUP_H_
#define SRC_RCC_PERIPHERAL_SETUP_H_
#define BAUD_RATE 0x008B

#include "variables.h"
#include "stm32f4xx.h"
#include "status.h"






Status_t RCC_Init();
Status_t GPIO_Init();
Status_t TIM2_Init();
Status_t ADC1_Init();
Status_t DMA2_Stream6_Init();
Status_t DMA2_Stream0_Init();
Status_t DMA2_Stream1_Init();
Status_t Usart_Init();
Status_t NVIC_Init();

#endif /* SRC_RCC_PERIPHERAL_SETUP_H_ */
