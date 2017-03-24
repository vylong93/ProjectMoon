/**
 ****************************************************************************
 * @file        text.h
 * @author      Long Dang
 * @version     V0.1
 * @date        23-March-2017
 * @copyright   LGPLv3
 * @brief       This is the header of the Text library.
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
#ifndef TEXT_H_
#define TEXT_H_

/** @defgroup LIB Library
 * @{
 */
/** @defgroup LIB_TEXT Text
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "graphic.h"

/* Exported types ------------------------------------------------------------*/
/**
 * @typedef text_align_t
 * This type is use for the text alignment specification.
 */
typedef enum
{
	LEFT, /*!< Align to the left side */
	RIGHT, /*!< Align to the right side */
	CENTER /*!< Align to the center */
} text_align_t;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void text_setCursor(int32_t i32CursorX, int32_t i32CursorY);
void text_setTextSize(uint32_t ui32TextSize);
void text_setTextColor(pixel_color_t colorForeground,
		pixel_color_t colorBackground);
void text_setWrapText(bool bWrapText);
int32_t text_drawChar(char c, int32_t i32CornerX, int32_t i32CornerY,
		int32_t i32Size);
int32_t text_drawString(const char *pcString, int32_t i32PositionX,
		int32_t i32CornerY, int32_t i32Size, text_align_t align);

/**
 * @} LIB_TEXT
 */
/**
 * @} LIB
 */
#endif /* TEXT_H_ */

/********************** (TM) PnL - Programming and Leverage ****END OF FILE****/
