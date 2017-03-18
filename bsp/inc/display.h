/**
 ****************************************************************************
 * @file        display.h
 * @author      Long Dang
 * @version     V0.1
 * @date        17-March-2017
 * @copyright   LGPLv3
 * @brief       This is the Display driver make use the low level LCD  APIs.
 ****************************************************************************
 * @attention
 *
 * <h2><center>&trade; PnL - Programming and Leverage </center></h2>
 *
 * This file is part of Project Moon.
 *
 *   Project Moon is free embedded software: you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public License as
 *   published by the Free Software Foundation, either version 3 of thescratch
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
#ifndef DISPLAY_H_
#define DISPLAY_H_

/** @defgroup DRV Board driver
 * @{
 */
/** @defgroup DRV_DISPLAY Display
 * @{
 */

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void display_init(void);
void display_turnOn(void);
void display_turnOff(void);
void display_setDim(bool bIsDim);
void display_render(void);

/**
 * @} DRV_DISPLAY
 */
/**
 * @} DRV
 */
#endif /* DISPLAY_H_ */

/********************** (TM) PnL - Programming and Leverage ****END OF FILE****/
