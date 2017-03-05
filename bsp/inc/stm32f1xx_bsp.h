/**
 ****************************************************************************
 * @file	stm32f1xx_bsp.h
 * @author	Long Dang
 * @version	V1.0.0
 * @date	18 Dec 2016
 * @brief	This is Board Support Package header file for Project Moon.
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
#ifndef STM32F1XX_BSP_H_
#define STM32F1XX_BSP_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>				/* uintX_t type */
#include <stdbool.h>			/* BOOL type */
#include "stm32f1xx_hal.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
#define bsp_delay_ms(x)		HAL_Delay(x)

/* Exported functions --------------------------------------------------------*/
void bsp_init(void);

#ifdef __cplusplus
}
#endif

#endif /* STM32F1XX_BSP_H_ */

/***************************** (C) COPYRIGHT 2017 PnL **********END OF FILE****/
