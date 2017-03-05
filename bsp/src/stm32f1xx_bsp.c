/**
 ****************************************************************************
 * @file	stm32f1xx_bsp.c
 * @author	Long Dang
 * @version	V1.0.0
 * @date	18 Dec 2016
 * @brief	File provides set of firmware functions to manage for STM32F1xx.
 ****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2017 PnL </center></h2>
 *
 * This file is part of Project Moon.
 *
 *   Project Moon is free embedded software: you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public License as
 *   published by the Free Software Foundation, either version 3 of the
 *   License, or (at your option) any later version.
 *
 *   Project Moon is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *   See the GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with Project Moon.
 *   If not, see <http://www.gnu.org/licenses>.
 ****************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_bsp.h"
#include "led.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/
void SystemClock_Config(void);

/* Private function prototypes -----------------------------------------------*/
/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follow :
 *            System Clock source            = PLL (HSI)
 *            SYSCLK(Hz)                     = 64000000
 *            HCLK(Hz)                       = 64000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 2
 *            APB2 Prescaler                 = 1
 *            PLLMUL                         = 16
 *            Flash Latency(WS)              = 2
 * @param  None
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_ClkInitTypeDef clkinitstruct =
	{ 0 };
	RCC_OscInitTypeDef oscinitstruct =
	{ 0 };

	/* Configure PLL ------------------------------------------------------*/
	/* PLL configuration: PLLCLK = (HSI / 2) * PLLMUL = (8 / 2) * 16 = 64 MHz */
	/* PREDIV1 configuration: PREDIV1CLK = PLLCLK / HSEPredivValue = 64 / 1 = 64 MHz */
	/* Enable HSI and activate PLL with HSi_DIV2 as source */
	oscinitstruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	oscinitstruct.HSEState = RCC_HSE_OFF;
	oscinitstruct.LSEState = RCC_LSE_OFF;
	oscinitstruct.HSIState = RCC_HSI_ON;
	oscinitstruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	oscinitstruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	oscinitstruct.PLL.PLLState = RCC_PLL_ON;
	oscinitstruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
	oscinitstruct.PLL.PLLMUL = RCC_PLL_MUL16;
	if (HAL_OK != HAL_RCC_OscConfig(&oscinitstruct))
	{
		/* Initialization Error */
		while (true);
	}

	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
	 clocks dividers */
	clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
	clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;
	if (HAL_OK != HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2))
	{
		/* Initialization Error */
		while (true);
	}
}

/* Exported functions prototype ----------------------------------------------*/
/**
 * @brief  Initialize Board Support Package as follow :
 *            Initialize STM32F103xB HAL library
 *            Configure the system clock
 *            Initialize LED port
 * @param  None
 * @retval None
 */
void bsp_init(void)
{
	/* STM32F103xB HAL library initialization:
	 - Configure the Flash prefetch
	 - Systick timer is configured by default as source of time base, but user
	 can eventually implement his proper time base source (a general purpose
	 timer for example or other time source), keeping in mind that Time base
	 duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
	 handled in milliseconds basis.
	 - Set NVIC Group Priority to 4
	 - Low Level Initialization
	 */
	HAL_Init();

	/* Configure the system clock to 64 MHz */
	SystemClock_Config();

	/* Enable the LED0 */
	bsp_led_init(LED_RED);
}

/***************************** (C) COPYRIGHT 2017 PnL **********END OF FILE****/
