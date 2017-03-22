/**
 ****************************************************************************
 * @file        graphic.c
 * @author      Long Dang
 * @version     V0.1
 * @date        19-March-2017
 * @copyright   LGPLv3
 * @brief       This file implement the Graphic library for mono color LCD.
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
/** @defgroup LIB_GRAPHIC Graphic
 * @{
 */
/* Includes ------------------------------------------------------------------*/
#include "graphic.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t *g_pui8GraphicBuffer = NULL; /*!< Pointer to the display buffer of the Driver */

/* Private functions declaration ---------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Exported functions prototype ----------------------------------------------*/
/**
 * @brief  Acquire the graphic buffer from Display driver.
 * @retval None
 */
void graphic_init(void) {
	g_pui8GraphicBuffer = display_getRenderBufferPointer();
}

/**
 * @brief  Draw one pixel to the Graphic buffer
 * @param  x: Horizontal axis value of the 2D Cartesian coordinate.
 * @param  y: Vertical axis inverted value of the 2D Cartesian coordinate.
 * @param  color: Selected pixel state.
 *          This parameter can be one of the following values:
 *			@arg WHITE
 *			@arg BLACK
 *			@arg INVERSE
 * @retval None
 */
void graphic_drawPixel(int16_t x, int16_t y, pixel_color_t color) {
	if ((NULL == g_pui8GraphicBuffer)
			|| (x < 0) || (x >= DISPLAY_WIDTH)
			|| (y < 0) || (y >= DISPLAY_HEIGHT)) {
		/* No need to draw anything */
		return;
	}

	int32_t i32PageColumnIndex = x + (y / 8) * DISPLAY_WIDTH;
	switch (color)
	{
	case WHITE:
		g_pui8GraphicBuffer[i32PageColumnIndex] |= (1 << (y & 7));
		break;
	case BLACK:
		g_pui8GraphicBuffer[i32PageColumnIndex] &= ~(1 << (y & 7));
		break;
	case INVERSE:
		g_pui8GraphicBuffer[i32PageColumnIndex] ^= (1 << (y & 7));
		break;
	}
}


/**
 * @} LIB_GRAPHIC
 */
/********************** (TM) PnL - Programming and Leverage ****END OF FILE****/
