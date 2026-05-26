/*
 * peripheral_setup.c
 *
 *  Created on: Apr 17, 2026
 *      Author: serhatt
 */
#include "peripheral_setup.h"

Status_t RCC_Init(void)
{
	//GPIOA Clock
	RCC->AHB1ENR |= (1 << 0);
	RCC->AHB1ENR |= (1 << 2); // GPIOC Clock

	//DMA2 Clock
	RCC->AHB1ENR |= (1 << 22);
	//ADC clock
	RCC->APB2ENR |= (1 << 8);
	//USART6
	RCC->APB2ENR |= (1 << 5);

	//Timer clock
	RCC->APB1ENR |= (1 << 0);

	return STATUS_OK;
}


Status_t GPIO_Init()
{
	//PA1 Pin mode Analog
	GPIOA->MODER |= (0x3 << 2);
	//PA2 Pin Mode Input
	GPIOA->MODER &= ~(0x3 << 4);
	GPIOC->MODER |= (2 << 12) | (2 << 14); // Alternate Function modu
	GPIOC->AFR[0] |= (8 << 24) | (8 << 28); // AF8 (USART6)
}

Status_t TIM2_Init()
{
	//FORMULA is HZ = Clock/(PRESCALER + 1)*(PERIOD + 1)
		//Because we will measure 5khz
		//PRESCALER
		TIM2->PSC = 0;
		//PERIOD
		TIM2->ARR = 1599;

		//MMS (Master Mode selection) = Update When timer count 1 to 99 adc will
		//take a sample 10000 time
		TIM2->CR2 |= (0x2 << 4);
		TIM2->CR1 |= (1 << 0);
}

Status_t ADC1_Init()
{


	ADC1->CR2 |= (1 << 0)          // ADON — ADC'yi aç
	             | (0x01 << 28)    // yükselen kenarda harici tetik
	             | (0x6 << 24)     // TIM2 TRGO tetik kaynağı
	             | (0x3 << 8);     // DMA — DMA isteği sürekli tekrar etsin


	ADC1->SQR3 = 1;              //CH1 seç (PA1 analog giriş)

}
//USART DMA
Status_t DMA2_Stream6_Init()
{
		//USART6 Channel 5 Stream1 Stream6
		//Stream 6 for TX
		DMA2_Stream6->CR &= ~(1 << 0);
		DMA2_Stream6->CR |= 0;

		//Channel 5 selected
		DMA2_Stream6->CR |= (0x5 << 25);
		//Circular Mode
		DMA2_Stream6->CR &= ~(1 << 8);
		//DIRECTION MEMORY TO PERIPHERAL
		DMA2_Stream6->CR |= (0x1 << 6);
		//TCIE Transfer Completed
		DMA2_Stream6->CR &= ~(1 << 4);
		//Data Size
		DMA2_Stream6->NDTR = 4;
		//Peripheral Adress
		DMA2_Stream6->PAR = (uint32_t) &(USART6->DR);
		//Memory Adress
		DMA2_Stream6->M0AR = (uint32_t)tx_buffer;
		//MINC Active
		DMA2_Stream6->CR |= (1 << 10);
		//
		//Enable Stream6
		DMA2_Stream6->CR |= (1 << 0);

}
//ADC DMA
Status_t DMA2_Stream0_Init(){
		//ADC DMA
		DMA2_Stream0->CR &= ~(1 << 0);
		while (DMA2_Stream0->CR & (1 << 0))
			; // Wait until DMA stop

		DMA2_Stream0->CR |= 0;
		//PERIPHERAL ADDRESS
		DMA2_Stream0->PAR = ((uint32_t) &(ADC1->DR));
		//TODO DOUBLE BUFFER
		//MEMORY ADDRESS
		DMA2_Stream0->M0AR = (uint32_t) (adc_buffer);

		//PACKAGE SIZE
		DMA2_Stream0->NDTR = 2048;
		//CHANNEL 0 to use
		DMA2_Stream0->CR &= ~(0x7 << 25);
		//MEMORY SIZE  will use 16 bit
		DMA2_Stream0->CR |= (0x1 << 13);
		//PERIPHERAL DATA SIZE we will use 16 bit
		DMA2_Stream0->CR |= (0x1 << 11);
		//MEMORY INCREMENT MODE
		DMA2_Stream0->CR |= (1 << 10);
		//CIRCULAR MODE
		DMA2_Stream0->CR |= (1 << 8);
		//PERIPHERAL TO MEMORY DIR
		DMA2_Stream0->CR &= ~(0x3 << 6);
		//TCIE ENABLE
		DMA2_Stream0->CR |= (1 << 4);
		//HTIE ENABLE
		DMA2_Stream0->CR |= (1 << 3);
		//Number of package
		//STREAM ENABLE
		DMA2_Stream0->CR |= (1 << 0);

}

Status_t DMA2_Stream1_Init(){

	//Stream 1 For RX
	DMA2_Stream1->CR &= ~(1 << 0);
	DMA2_Stream1->CR |= 0;
	//Channel 5 selected
	DMA2_Stream1->CR |= (0x5 << 25);
	//Circular Mode
	DMA2_Stream1->CR |= (1 << 8);
	//DIRECTION MEMORY TO PERIPHERAL
	DMA2_Stream1->CR |= (0x1 << 6);
	//TCIE Transfer Completed
	DMA2_Stream1->CR |= (1 << 4);
	//Enable Stream1
	DMA2_Stream1->CR |= (1 << 0);

}
Status_t Usart_Init()
{
	    USART6->BRR = BAUD_RATE;
		USART6->CR1 |= (1 << 13);
		USART6->CR1 |= (1 << 3);
		USART6->CR1 |= (1 << 2);
		USART6->CR3 |= (1 << 7);

}

Status_t NVIC_Init()
{
	NVIC_EnableIRQ(DMA2_Stream0_IRQn);
}
