/*
 * NEO_P.c
 *
 *  Created on: 5 oct. 2026
 *      Author: SETUP GAME
 */


#include "NEO_P.h"
#include "string.h"

/* --- Variables privées du module --- */
static TIM_HandleTypeDef *neopixel_htim = NULL;
static uint32_t neopixel_channel = TIM_CHANNEL_1;

static uint8_t LED_Data[NEOPIXEL_NUM_LEDS][3];
static uint16_t pwmData[NEOPIXEL_BUFFER_SIZE];
static volatile uint8_t datasentflag = 0;

/**
  * @brief  Initialise le driver NeoPixel avec le Timer et le Channel associés
  * @param  htim: Pointeur vers la structure du Timer (ex: &htim1)
  * @param  channel: Canal PWM utilisé (ex: TIM_CHANNEL_1)
  */
void NeoPixel_Init(TIM_HandleTypeDef *htim, uint32_t channel)
{
    neopixel_htim = htim;
    neopixel_channel = channel;

    NeoPixel_Clear();
    NeoPixel_Send();
}

/**
  * @brief  Définit la couleur d'une LED spécifique (0 à 11)
  */
void NeoPixel_SetLED(uint8_t led, uint8_t red, uint8_t green, uint8_t blue)
{
    if (led >= NEOPIXEL_NUM_LEDS) return;

    LED_Data[led][0] = green; // Format GRB
    LED_Data[led][1] = red;
    LED_Data[led][2] = blue;
}

/**
  * @brief  Remplit toutes les LEDs avec la même couleur
  */
void NeoPixel_Fill(uint8_t red, uint8_t green, uint8_t blue)
{
    for (int i = 0; i < NEOPIXEL_NUM_LEDS; i++)
    {
        NeoPixel_SetLED(i, red, green, blue);
    }
}

/**
  * @brief  Éteint toutes les LEDs (met le buffer à 0)
  */
void NeoPixel_Clear(void)
{
    NeoPixel_Fill(0, 0, 0);
}

/**
  * @brief  Convertit les couleurs en PWM et lance le transfert DMA
  */
void NeoPixel_Send(void)
{
    if (neopixel_htim == NULL) return;

    uint32_t indx = 0;
    uint32_t color;

    for (int i = 0; i < NEOPIXEL_NUM_LEDS; i++)
    {
        color = ((uint32_t)LED_Data[i][0] << 16) |
                ((uint32_t)LED_Data[i][1] << 8)  |
                ((uint32_t)LED_Data[i][2]);

        for (int bit = 23; bit >= 0; bit--)
        {
            if (color & (1UL << bit))
            {
                pwmData[indx] = NEOPIXEL_PWM_HI;
            }
            else
            {
                pwmData[indx] = NEOPIXEL_PWM_LO;
            }
            indx++;
        }
    }

    for (int i = 0; i < NEOPIXEL_RESET_SLOTS; i++)
    {
        pwmData[indx] = 0;
        indx++;
    }

    datasentflag = 0;
    HAL_TIM_PWM_Start_DMA(neopixel_htim, neopixel_channel, (uint32_t *)pwmData, indx);

    while (!datasentflag) {};
}

void NeoPixel_SetColorByName(const char *color_name)
{
    if (color_name == NULL) return;

    if (strcasecmp(color_name, "red") == 0)
    {
        NeoPixel_Fill(240 / NEOPIXEL_DIM, 0, 0);
    }
    else if (strcasecmp(color_name, "blue") == 0)
    {
        NeoPixel_Fill(0, 40 / NEOPIXEL_DIM, 255 / NEOPIXEL_DIM);
    }
    else if (strcasecmp(color_name, "green") == 0)
    {
        NeoPixel_Fill(0, 220 / NEOPIXEL_DIM, 20 / NEOPIXEL_DIM);
    }
    else if (strcasecmp(color_name, "off") == 0)
    {
        NeoPixel_Clear();
    }

    NeoPixel_Send();
}

void NeoPixel_SetMode(const char *mode_name)
{
    if (mode_name == NULL) return;

    /* --- MODE EVIL (Rouge sombre + Pupille agressive) --- */
    if (strcasecmp(mode_name, "evil") == 0)
    {
        // Pulsation d'entrée rapide en rouge
        for (int p = 20; p <= 240; p += 20)
        {
            NeoPixel_Fill(p / NEOPIXEL_DIM, 0, 0);
            NeoPixel_Send();
            HAL_Delay(15);
        }
        // État final Evil : Anneau rouge + 2 points focaux rouge-orangé
        NeoPixel_Fill(180 / NEOPIXEL_DIM, 0, 0);
        NeoPixel_SetLED(0, 255 / NEOPIXEL_DIM, 40 / NEOPIXEL_DIM, 0);
        NeoPixel_SetLED(6, 255 / NEOPIXEL_DIM, 40 / NEOPIXEL_DIM, 0);
        NeoPixel_Send();
    }

    /* --- MODE FRIENDLY (Mix de couleurs joyeuses / Happy Colors) --- */
    else if (strcasecmp(mode_name, "friendly") == 0)
    {
        // Palette de 6 couleurs "Happy" (Or, Turquoise, Vert Pomme, Rose Corail, Cyan, Violet doux)
        const uint8_t happy_palette[6][3] = {
            {255, 180, 0},   // Jaune Or chaleureux
            {0,   240, 160}, // Turquoise / Menthe
            {80,  255, 0},   // Vert Printemps
            {255, 40,  120}, // Rose Corail joyeux
            {0,   180, 255}, // Bleu Ciel
            {180, 60,  255}  // Lavande / Violet
        };

        // Petite rotation joyeuse des couleurs autour de l'anneau avant de se fixer
        for (int spin = 0; spin < 12; spin++)
        {
            for (int i = 0; i < NEOPIXEL_NUM_LEDS; i++)
            {
                uint8_t c_idx = (i + spin) % 6;
                NeoPixel_SetLED(i,
                                happy_palette[c_idx][0] / NEOPIXEL_DIM,
                                happy_palette[c_idx][1] / NEOPIXEL_DIM,
                                happy_palette[c_idx][2] / NEOPIXEL_DIM);
            }
            NeoPixel_Send();
            HAL_Delay(45);
        }
    }

    /* --- MODE IDLE (Bleu calme et posé) --- */
    else if (strcasecmp(mode_name, "idle") == 0)
    {
        // Une respiration douce en bleu puis maintien en bleu repos
        for (int b = 20; b <= 160; b += 8)
        {
            NeoPixel_Fill(0, (b / 3) / NEOPIXEL_DIM, b / NEOPIXEL_DIM);
            NeoPixel_Send();
            HAL_Delay(20);
        }
        for (int b = 160; b >= 100; b -= 8)
        {
            NeoPixel_Fill(0, (b / 3) / NEOPIXEL_DIM, b / NEOPIXEL_DIM);
            NeoPixel_Send();
            HAL_Delay(20);
        }
    }
}

/**
  * @brief  Gère la fin du transfert DMA
  */
void NeoPixel_DMA_Callback(TIM_HandleTypeDef *htim)
{
    if (neopixel_htim != NULL && htim->Instance == neopixel_htim->Instance)
    {
        HAL_TIM_PWM_Stop_DMA(neopixel_htim, neopixel_channel);
        datasentflag = 1;
    }
}

/**
  * @brief  Callback d'interruption HAL (peut être déplacé dans main.c si besoin)
  */
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    NeoPixel_DMA_Callback(htim);
}
