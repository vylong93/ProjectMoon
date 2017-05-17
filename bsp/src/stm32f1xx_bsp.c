/**
 ****************************************************************************
 * @file        stm32f1xx_bsp.c
 * @author      Long Dang
 * @version     V0.1
 * @date        18-December-2016
 * @copyright   LGPLv3
 * @brief       This file provides set of board support package functions.
 ****************************************************************************
 * @attention
 *
 * <h2><center>&trade; PnL - Programming and Leverage </center></h2>
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
/** @addtogroup BSP
 * @{
 */
/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_bsp.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/
void SystemClock_Config(void);

/* Private function prototypes -----------------------------------------------*/
/**
 * @brief  System Clock Configuration.
 *         The system Clock is configured as follow:
 *         @arg System Clock source            = PLL (HSI)
 *         @arg SYSCLK(Hz)                     = 48000000
 *         @arg HCLK(Hz)                       = 48000000
 *         @arg AHB Prescaler                  = 1
 *         @arg APB1 Prescaler                 = 2
 *         @arg APB2 Prescaler                 = 1
 *         @arg PLLMUL                         = 6
 *         @arg Flash Latency(WS)              = 2
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_ClkInitTypeDef clkinitstruct =
	{ 0 };
	RCC_OscInitTypeDef oscinitstruct =
	{ 0 };
	RCC_PeriphCLKInitTypeDef periphinitstruct =
	{ 0 };

	/* Configure PLL ------------------------------------------------------*/
	/* PREDIV1 configuration: PREDIV1CLK = HSE / HSEPredivValue = 8 / 1 = 8 MHz */
	/* PLL configuration: PLLCLK = PREDIV1CLK * PLLMUL = 8 * 9 = 72 MHz */
	/* Enable HSE and activate PLL with HSE as source */
	oscinitstruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	oscinitstruct.HSIState = RCC_HSI_OFF;
	oscinitstruct.LSEState = RCC_LSE_OFF;
	oscinitstruct.HSEState = RCC_HSE_ON;
	oscinitstruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	oscinitstruct.PLL.PLLState = RCC_PLL_ON;
	oscinitstruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	oscinitstruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_OK != HAL_RCC_OscConfig(&oscinitstruct))
	{
		/* Initialization Error */
		while (true);
	}

	/* Select PLL as system clock source
	 * and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
	clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK; /* SYSCLK = PLLCLK = 72 MHz */
	clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1; /* HCLK = SYSCLK / 1 = 72 MHz  */
	clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2; /* PCLK1 = HCLK / 2 = 36 MHz */
	clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1; /* PCLK2 = HCLK / 1 = 72 MHz */
	if (HAL_OK != HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2))
	{
		/* Initialization Error */
		while (true);
	}

	/* Configure USB Clock = PLLCLK / 1 = 72 / 1.5 = 48 MHz ----------------*/
	periphinitstruct.PeriphClockSelection = RCC_PERIPHCLK_USB;
	periphinitstruct.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
	if (HAL_OK != HAL_RCCEx_PeriphCLKConfig(&periphinitstruct))
	{
		/* Initialization Error */
		while (true);
	}
}

/* Exported functions prototype ----------------------------------------------*/
/**
 * @brief  Initialize Board Support Package as follow.
 *         @arg Initialize STM32F103xB HAL library
 *         @arg Configure the system clock
 *         @arg Initialize LED port
 * @retval bool: Status of initialization
 *			@arg true: succeeded
 *			@arg false: failed
 */
bool bsp_init(void)
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

	/* Configure the system clock to 48 MHz */
	SystemClock_Config();

	/* Ensure to use all bits for preemption priority and no bit for sub-priority */
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

	return true;
}

/**@}BSP*/
/********************** (TM) PnL - Programming and Leverage ****END OF FILE****/
