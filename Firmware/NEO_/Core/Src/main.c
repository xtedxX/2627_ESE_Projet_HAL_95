/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "NEO_P.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_I2C3_Init();
	MX_USART2_UART_Init();
	MX_TIM1_Init();
	/* USER CODE BEGIN 2 */
	NeoPixel_Init(&htim1, TIM_CHANNEL_1);
	HAL_Delay(500);
	/* USER CODE END 2 */

	/* Initialize leds */
	BSP_LED_Init(LED_GREEN);

	/* Initialize USER push-button, will be used to trigger an interrupt each time it's pressed.*/
	BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* ====================================================================
		       * 1. BASS PUMP (Kick Electro 128 BPM avec effet de fondu)
		       * ==================================================================== */
		      for (int beat = 0; beat < 8; beat++)
		      {
		          for (int fade = 10; fade >= 0; fade--)
		          {
		              if (beat % 2 == 0) {
		                  // Kick pair : Violet Cyberpunk
		                  NeoPixel_Fill((180 * fade) / 10, 0, (255 * fade) / 10);
		              } else {
		                  // Kick impair : Cyan Electrique
		                  NeoPixel_Fill(0, (200 * fade) / 10, (255 * fade) / 10);
		              }
		              NeoPixel_Send();
		              HAL_Delay(35);
		          }
		          HAL_Delay(80);
		      }

		      /* ====================================================================
		       * 2. BUILD-UP : DOUBLE LASER QUI ACCÉLÈRE (Cyan vs Magenta)
		       * ==================================================================== */
		      int delay_laser = 65;
		      for (int tour = 0; tour < 7; tour++)
		      {
		          for (int i = 0; i < NEOPIXEL_NUM_LEDS; i++)
		          {
		              NeoPixel_Clear();

		              // Laser 1 (Cyan) + sa traînée
		              NeoPixel_SetLED(i, 0, 255, 255);
		              NeoPixel_SetLED((i + 11) % 12, 0, 40, 40);

		              // Laser 2 opposé (Magenta) + sa traînée
		              NeoPixel_SetLED((i + 6) % 12, 255, 0, 150);
		              NeoPixel_SetLED((i + 5) % 12, 40, 0, 25);

		              NeoPixel_Send();
		              HAL_Delay(delay_laser);
		          }
		          // Accélération à chaque tour (montée électro)
		          if (delay_laser > 15) delay_laser -= 8;
		      }

		      /* ====================================================================
		       * 3. THE DROP : STROBOSCOPE ULTRA-RAPIDE (Blanc / Rouge / Bleu)
		       * ==================================================================== */
		      for (int s = 0; s < 16; s++)
		      {
		          if (s % 3 == 0)      NeoPixel_Fill(150, 150, 150); // Flash Blanc
		          else if (s % 3 == 1) NeoPixel_Fill(255, 0, 50);    // Flash Rouge/Rose
		          else                 NeoPixel_Fill(0, 50, 255);    // Flash Bleu

		          NeoPixel_Send();
		          HAL_Delay(30);

		          NeoPixel_Clear();
		          NeoPixel_Send();
		          HAL_Delay(30);
		      }

		      /* ====================================================================
		       * 4. EQUALIZER / VU-MÈTRE SYMÉTRIQUE (Gauche & Droite)
		       * ==================================================================== */
		      uint8_t peaks[8] = {3, 6, 2, 5, 6, 3, 6, 6}; // Hauteurs des basses
		      for (int p = 0; p < 8; p++)
		      {
		          // Montée du son
		          for (int lvl = 0; lvl <= peaks[p]; lvl++)
		          {
		              NeoPixel_Clear();
		              for (int i = 0; i < lvl; i++)
		              {
		                  // Vert en bas (0-1), Jaune au milieu (2-3), Rouge en haut (4-5)
		                  uint8_t r = (i < 2) ? 0   : (i < 4 ? 200 : 255);
		                  uint8_t g = (i < 2) ? 200 : (i < 4 ? 150 : 0);
		                  uint8_t b = (i < 4) ? 0   : 40;

		                  NeoPixel_SetLED(i, r, g, b);      // Côté gauche (0 à 5)
		                  NeoPixel_SetLED(11 - i, r, g, b); // Côté droit (11 à 6)
		              }
		              NeoPixel_Send();
		              HAL_Delay(20);
		          }
		          // Descente du son
		          for (int lvl = peaks[p]; lvl >= 0; lvl--)
		          {
		              NeoPixel_Clear();
		              for (int i = 0; i < lvl; i++)
		              {
		                  uint8_t r = (i < 2) ? 0   : (i < 4 ? 200 : 255);
		                  uint8_t g = (i < 2) ? 200 : (i < 4 ? 150 : 0);
		                  uint8_t b = (i < 4) ? 0   : 40;

		                  NeoPixel_SetLED(i, r, g, b);
		                  NeoPixel_SetLED(11 - i, r, g, b);
		              }
		              NeoPixel_Send();
		              HAL_Delay(25);
		          }
		      }

		      /* ====================================================================
		       * 5. REBOND ALTERNÉ (Moitié Gauche / Moitié Droite en rythme)
		       * ==================================================================== */
		      for (int bounce = 0; bounce < 8; bounce++)
		      {
		          NeoPixel_Clear();
		          if (bounce % 2 == 0)
		          {
		              // Allume les LEDs 0 à 5 en Orange/Rouge
		              for (int i = 0; i < 6; i++) NeoPixel_SetLED(i, 255, 40, 0);
		          }
		          else
		          {
		              // Allume les LEDs 6 à 11 en Bleu/Violet
		              for (int i = 6; i < 12; i++) NeoPixel_SetLED(i, 80, 0, 255);
		          }
		          NeoPixel_Send();
		          HAL_Delay(220);
		      }
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 10;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
