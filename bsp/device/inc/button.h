/**
 ****************************************************************************
 * @file        button.h
 * @author      Long Dang
 * @version     V0.1
 * @date        01-May-2017
 * @copyright   LGPLv3
 * @brief       This is the header of the button IO handlers.
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
#ifndef BUTTON_H_
#define BUTTON_H_

/** @addtogroup BSP_DEVICE_BUTTON
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_bsp.h"

/* Exported types ------------------------------------------------------------*/
/**
 * @struct _button_callback_t
 * This type define the button callback structure.
 */
typedef struct _button_callback_t
{
	void (*action)(void); /*!< button callback */
} button_callback_t;

/* Exported constants --------------------------------------------------------*/
#define BUTTONn		(9)	/*!< Number of supported button on board */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
bool button_configIRQHandler(button_callback_t *pbuttonCallbackArray);

/**@}BSP_DEVICE_BUTTON*/
#endif /* BUTTON_H_ */

/********************** (TM) PnL - Programming and Leverage ****END OF FILE****/
