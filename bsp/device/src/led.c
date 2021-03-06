/**
 ****************************************************************************
 * @file        led.c
 * @author      Long Dang
 * @version     V0.1
 * @date        18-December-2016
 * @copyright   LGPLv3
 * @brief       This file contains APIs to control the LED.
 * @note        All the LEDs must be connected as Active low configuration.
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
/** @addtogroup BSP_DEVICE_LED
 * @{
 */
/** @defgroup BSP_DEVICE_LED_PRIVATE LED (Private)
 * @{
 */
/* Includes ------------------------------------------------------------------*/
#include "led.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define LEDn                             2	/*!< Number of supported LED on board */

#define LEDx_GPIO_CLK_ENABLE(__INDEX__)   do { if((__INDEX__) == 0) LED0_GPIO_CLK_ENABLE(); else if ((__INDEX__) == 1) LED1_GPIO_CLK_ENABLE();} while(0)
#define LEDx_GPIO_CLK_DISABLE(__INDEX__)  (((__INDEX__) == 0) ? LED0_GPIO_CLK_DISABLE() : (((__INDEX__) == 1) ? LED1_GPIO_CLK_DISABLE : 0))

/** @defgroup BSP-LED0 LED0-RED pin constant
 * @{
 */
#define LED0_PIN                         GPIO_PIN_13
#define LED0_GPIO_PORT                   GPIOC
#define LED0_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOC_CLK_ENABLE()
#define LED0_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOC_CLK_DISABLE()
/**
 * @} BSP-LED0
 */

/** @defgroup BSP-LED1 LED1-RED pin constant
 * @{
 */
#define LED1_PIN                         GPIO_PIN_9
#define LED1_GPIO_PORT                   GPIOA
#define LED1_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOA_CLK_ENABLE()
#define LED1_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOA_CLK_DISABLE()
/**
 * @} BSP-LED1
 */


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
GPIO_TypeDef* LED_PORT[LEDn] = { LED0_GPIO_PORT, LED1_GPIO_PORT };	/*!< LED's port array */
const uint16_t LED_PIN[LEDn] = { LED0_PIN, LED1_PIN }; /*!< LED's pin array */

/* Private functions declaration ---------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Exported functions prototype ----------------------------------------------*/
/**
 * @brief  Configures LED GPIO.
 * @param  Led: Led to be configured.
 *          This parameter can be one of the following values:
 *			@arg LED_RED
 * @retval None
 */
void bsp_led_init(Led_TypeDef Led)
{
	GPIO_InitTypeDef gpioinitstruct;

	/* Enable the GPIO_LED Clock */
	LEDx_GPIO_CLK_ENABLE(Led);

	/* Configure the GPIO_LED pin */
	gpioinitstruct.Pin = LED_PIN[Led];
	gpioinitstruct.Mode = GPIO_MODE_OUTPUT_PP;
	gpioinitstruct.Pull = GPIO_NOPULL;
	gpioinitstruct.Speed = GPIO_SPEED_FREQ_HIGH;

	/* Initialize the GPIO_LED pin */
	HAL_GPIO_Init(LED_PORT[Led], &gpioinitstruct);

	/* Set PIN to switch off the LED - Active Low */
	HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_SET);
}

/**
 * @brief  DeInit LEDs.
 * @param  Led: LED to be de-init.
 *			This parameter can be one of the following values:
 *			@arg  LED_RED
 * @note Led DeInit does not disable the GPIO clock nor disable the Mfx
 * @retval None
 */
void bsp_led_deinit(Led_TypeDef Led)
{
	GPIO_InitTypeDef gpio_init_structure;

	/* Turn off LED - Active Low */
	HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_SET);

	/* DeInit the GPIO_LED pin */
	gpio_init_structure.Pin = LED_PIN[Led];
	HAL_GPIO_DeInit(LED_PORT[Led], gpio_init_structure.Pin);
}

/**
 * @brief  Turns selected LED On (Active Low).
 * @param  Led: Specifies the Led to be set on.
 *			This parameter can be one of following parameters:
 *			@arg LED_RED
 * @retval None
 */
void bsp_led_on(Led_TypeDef Led)
{
	HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_RESET);
}

/**
 * @brief  Turns selected LED Off (Active Low).
 * @param  Led: Specifies the Led to be set off.
 *			This parameter can be one of following parameters:
 *			@arg LED_RED
 * @retval None
 */
void bsp_led_off(Led_TypeDef Led)
{
	HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_SET);
}

/**
 * @brief  Toggles the selected LED.
 * @param  Led: Specifies the Led to be toggled.
 *			This parameter can be one of following parameters:
 *			@arg LED_RED
 * @retval None
 */
void bsp_led_toggle(Led_TypeDef Led)
{
	HAL_GPIO_TogglePin(LED_PORT[Led], LED_PIN[Led]);
}

/**@}BSP_DEVICE_LED_PRIVATE*/
/**@}BSP_DEVICE_LED*/
/********************** (TM) PnL - Programming and Leverage ****END OF FILE****/
