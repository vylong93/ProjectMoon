/**
 ****************************************************************************
 * @file        stm32f1xx_bsp.h
 * @author      Long Dang
 * @version     V0.1
 * @date        18-December-2016
 * @copyright   LGPLv3
 * @brief       This is Board Support Package header file for Project Moon.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32F1XX_BSP_H_
#define STM32F1XX_BSP_H_

/** @addtogroup BSP
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>				/* uintX_t type */
#include <stdbool.h>			/* BOOL type */
#include "stm32f1xx_hal.h"		/* HAL library */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
/**
 * @brief Delay function wrapper the HAL delay
 * @param  x value in milisecond
 * @retval None
 */
#define bsp_delay_ms(x)		HAL_Delay(x)

/* Exported functions --------------------------------------------------------*/
bool bsp_init(void);

/**@}BSP*/
#endif /* STM32F1XX_BSP_H_ */

/********************** (TM) PnL - Programming and Leverage ****END OF FILE****/
