/*
 * NEO_P.h
 *
 *  Created on: 5 oct. 2026
 *      Author: SETUP GAME
 */

#ifndef INC_NEO_P_H_
#define INC_NEO_P_H_

#include "main.h"

#define NEOPIXEL_DIM           4
/* --- Paramètres de l'anneau NeoPixel --- */
#define NEOPIXEL_NUM_LEDS      12
#define NEOPIXEL_RESET_SLOTS   50
#define NEOPIXEL_BUFFER_SIZE   ((NEOPIXEL_NUM_LEDS * 24) + NEOPIXEL_RESET_SLOTS)

/* --- Timings PWM pour horloge Timer à 80 MHz (ARR = 99) --- */
#define NEOPIXEL_PWM_HI        64
#define NEOPIXEL_PWM_LO        32

/* --- Prototypes des fonctions --- */
void NeoPixel_Init(TIM_HandleTypeDef *htim, uint32_t channel);
void NeoPixel_SetLED(uint8_t led, uint8_t red, uint8_t green, uint8_t blue);
void NeoPixel_Fill(uint8_t red, uint8_t green, uint8_t blue);
void NeoPixel_Clear(void);
void NeoPixel_Send(void);


// À appeler dans HAL_TIM_PWM_PulseFinishedCallback() si vous utilisez plusieurs timers,
// sinon géré directement dans neopixel.c
void NeoPixel_DMA_Callback(TIM_HandleTypeDef *htim);

void NeoPixel_SetColorByName(const char *color_name);
void NeoPixel_SetMode(const char *mode_name);


#endif /* INC_NEO_P_H_ */
