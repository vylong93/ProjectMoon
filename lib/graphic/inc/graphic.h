/**
 ****************************************************************************
 * @file        graphic.h
 * @author      Long Dang
 * @version     V0.1
 * @date        19-March-2017
 * @copyright   LGPLv3
 * @brief       This is the header of the Graphic library for mono color LCD.
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
#ifndef GRAPHIC_H_
#define GRAPHIC_H_

/** @addtogroup LIB_GFX
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "display.h"

/* Exported types ------------------------------------------------------------*/
/**
 * @typedef pixel_color_t
 * This type define the target pixel state for drawing in the Graphic buffer.
 */
typedef enum
{
	BLACK = 0, /*!< Pixel passive state: OFF */
	WHITE = 1, /*!< Pixel active state: ON */
	INVERSE = 2 /*!< Invert the current pixel state */
} pixel_color_t;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define	graphic_clearRenderBuffer()	display_clearRenderBuffer() /*!< Wrapper display API */
#define	graphic_render()			display_render() /*!< Wrapper display API */
#define	graphic_delay_ms(x)			display_delay_ms(x) /*!< Wrapper display API */
#define graphic_setInvertMode()		display_setInvertMode() /*!< Wrapper display API */
#define graphic_setNormalMode()		display_setNormalMode() /*!< Wrapper display API */

/* Exported functions --------------------------------------------------------*/
bool graphic_init(void);
void graphic_drawPixel(int16_t x, int16_t y, pixel_color_t color);
void graphic_drawFastHLine(int16_t x, int16_t y, int16_t w,
		pixel_color_t color);
void graphic_drawFastVLine(int16_t x, int16_t __y, int16_t __h,
		pixel_color_t color);
void graphic_drawRect(int16_t x, int16_t y, int16_t w, int16_t h,
		pixel_color_t color);
void graphic_fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
		pixel_color_t color);

/**@}LIB_GFX*/
#endif /* GRAPHIC_H_ */

/********************** (TM) PnL - Programming and Leverage ****END OF FILE****/
