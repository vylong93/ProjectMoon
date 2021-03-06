/**
 ****************************************************************************
 * @file        lcd.h
 * @author      Long Dang
 * @version     V0.1
 * @date        12-March-2017
 * @copyright   LGPLv3
 * @brief       This module declare low level APIs to control the LCD module.
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
#ifndef LCD_H_
#define LCD_H_

/** @addtogroup BSP_DEVICE_LCD
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_bsp.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define bsp_lcd_delay_ms(x) bsp_delay_ms(x) /*!< Wrapper BSP API */

/* Exported functions --------------------------------------------------------*/
bool bsp_lcd_init(void);
void bsp_lcd_reset(void);
bool bsp_lcd_sendCommand(uint8_t ui8Command);
bool bsp_lcd_sendData(uint8_t * pui8Buffer, uint16_t ui16Size);

/**@}BSP_DEVICE_LCD*/
#endif /* LCD_H_ */

/********************** (TM) PnL - Programming and Leverage ****END OF FILE****/
