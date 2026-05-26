#ifndef VARIABLES_H
#define VARIABLES_H
#include <stdint.h>

#define ARM_MATH_CM4
#define __FPU_PRESENT    1U
#define __FPU_USED       1U
#include "arm_math.h"

#define FFT_BUFFER_SIZE 2048
#define MEDIAN_SIZE 5

extern uint16_t adc_buffer[2048];
extern uint8_t  tx_buffer[5];
extern float32_t hanning_window[FFT_BUFFER_SIZE];


extern float pitch_history[MEDIAN_SIZE];
extern int pitch_idx;
extern float    fftbuffin[FFT_BUFFER_SIZE];
extern float    fftbufout[FFT_BUFFER_SIZE];
extern float    mag_buffer[FFT_BUFFER_SIZE / 2];
extern float32_t maks_hz;
extern float32_t maks_genlik;
extern uint32_t  maks_indeks;
extern uint32_t  freq_buffer;
extern arm_rfft_fast_instance_f32 fftHandler;
extern uint8_t   fftflag;
extern uint8_t system_is_open;
extern volatile uint8_t   veridoldu;


#endif
