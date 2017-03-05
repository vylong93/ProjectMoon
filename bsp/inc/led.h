/**
 ****************************************************************************
 * @file	led.h
 * @author	Long Dang
 * @version	0.1
 * @date	18 Dec 2016
 * @brief	Header file of LED APIs.
 * 			@note All the LEDs must be connected as Active low configuration.
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LED_H_
#define LED_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_bsp.h"

/* Exported types ------------------------------------------------------------*/
/*! @enum An enum type for LED */
/**
 * Each on board LED corresponding to one value of this enum type.
 */
typedef enum
{
	LED0 = 0,		/*!< Enum value LED0. */
	LED_RED = LED0	/*!< Enum value LED_RED. */
} Led_TypeDef;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
void bsp_led_init(Led_TypeDef Led);
void bsp_led_deinit(Led_TypeDef Led);
void bsp_led_on(Led_TypeDef Led);
void bsp_led_off(Led_TypeDef Led);
void bsp_led_toggle(Led_TypeDef Led);

#endif /* LED_H_ */

/***************************** (C) COPYRIGHT 2017 PnL **********END OF FILE****/
