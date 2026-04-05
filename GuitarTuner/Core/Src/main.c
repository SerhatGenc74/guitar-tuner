#include "main.h"
#include <stdio.h>
#define ARM_MATH_CM4
#include "arm_math.h"
#define FFT_BUFFER_SIZE 2048
#define BAUD_RATE 0x008B

float fftbuffin[FFT_BUFFER_SIZE];
float fftbufout[FFT_BUFFER_SIZE];
float mag_buffer[FFT_BUFFER_SIZE / 2];
float32_t maks_hz = 0;
arm_rfft_fast_instance_f32 fftHandler;
uint8_t fftflag = 0;
uint8_t veridoldu = 0;
float32_t maks_genlik;
uint32_t maks_indeks;
uint16_t adc_buffer[2048];
uint32_t freq_buffer;
uint8_t tx_buffer[5];
int main(void) {
	arm_rfft_fast_init_f32(&fftHandler, FFT_BUFFER_SIZE);

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

	//PA1 Pin mode Analog
	GPIOA->MODER |= (0x3 << 2);

	GPIOC->MODER |= (2 << 12) | (2 << 14); // Alternate Function modu
	GPIOC->AFR[0] |= (8 << 24) | (8 << 28); // AF8 (USART6) 

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

	ADC1->CR2 |= (1 << 30) | (1 << 0) | (0x01 << 28) | (0x6 << 24) | (0x3 << 8);
	//PA1 pin output analog
	ADC1->SQR3 = 1;

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



	

	//ADC DMA
	DMA2_Stream0->CR &= ~(1 << 0);
	while (DMA2_Stream0->CR & (1 << 0))
		; // Wait until DMA stop

	DMA2_Stream0->CR |= 0;
	//PERIPHERAL ADDRESS
	DMA2_Stream0->PAR = ((uint32_t) &(ADC1->DR));
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





	USART6->BRR = BAUD_RATE;
	USART6->CR1 |= (1 << 13);
	USART6->CR1 |= (1 << 3);
	USART6->CR1 |= (1 << 2);
	USART6->CR3 |= (1 << 7);


	NVIC_EnableIRQ(DMA2_Stream0_IRQn);

	while (1) {
		if (veridoldu == 2) {
			Process_Buffer();
			veridoldu = 0;
		}

	}
}

void Process_Buffer() {

	for (int n = 0; n < FFT_BUFFER_SIZE; n++) {
		fftbuffin[n] = ((float32_t) adc_buffer[n] - 2048.0f);
	}
	arm_rfft_fast_f32(&fftHandler, fftbuffin, fftbufout, fftflag);

	arm_cmplx_mag_f32(fftbufout, mag_buffer, 1024);
	arm_max_f32(&mag_buffer[1], 1023, &maks_genlik, &maks_indeks);

	maks_indeks += 1;
		//Parabolic Interpolation
		//y1 Left side , y2 Peak , y3 Right side
		//d = 0.5f* y1 - y3 / y1-2*y2+y3
		float y1 = mag_buffer[maks_indeks - 1];
	    float y2 = mag_buffer[maks_indeks];
	    float y3 = mag_buffer[maks_indeks + 1];
	    float denominator = y1-2*y2+y3;

	    float d = 0;
	    if (denominator != 0) {
	            d = 0.5f * (y1 - y3) / denominator;
	        }
	    float32_t hassas = (float32_t)maks_indeks + d;
	    maks_hz = hassas * 10000.0f / 2048.0f;

	    tx_buffer[0] = 0xAA;
	    memcpy(&tx_buffer[1], &maks_hz, 4);s
	    DMA2_Stream6->CR &= ~(1 << 0);
	    while(DMA2_Stream6->CR & (1 << 0));
	    //FLAG Clean
	    DMA2->HIFCR |= (1 << 21) | (1 << 19) | (1 << 18) | (1 << 16);

	    DMA2_Stream6->M0AR = (uint32_t)tx_buffer;
	    DMA2_Stream6->NDTR = 5;
	    DMA2_Stream6->CR |= (1 << 0);

}

void DMA2_Stream0_IRQHandler(void) {
	//When half of buffer fill flag set 1;
	if (DMA2->LISR & (1 << 4)) {
		DMA2->LIFCR = (1 << 4);
		veridoldu = 1;

	}
	if (DMA2->LISR & (1 << 5)) {
		DMA2->LIFCR = (1 << 5);
		veridoldu = 2;
	}
}
