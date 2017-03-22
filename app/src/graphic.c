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
void graphic_init(void)
{
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
void graphic_drawPixel(int16_t x, int16_t y, pixel_color_t color)
{
	if ((NULL == g_pui8GraphicBuffer) || (x < 0) || (x >= DISPLAY_WIDTH)
			|| (y < 0) || (y >= DISPLAY_HEIGHT))
	{
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
 * @brief  Draw horizontal line to the Graphic buffer
 * @param  x: Horizontal axis value of the 2D Cartesian coordinate.
 * @param  y: Vertical axis inverted value of the 2D Cartesian coordinate.
 * @param  w: Width of the line in pixel unit.
 * @param  color: Selected pixel state.
 *          This parameter can be one of the following values:
 *			@arg WHITE
 *			@arg BLACK
 *			@arg INVERSE
 * @retval None
 */
void graphic_drawFastHLine(int16_t x, int16_t y, int16_t w, pixel_color_t color)
{
	/* Do bounds/limit checks */
	if ((y < 0) || (y >= DISPLAY_HEIGHT))
	{
		return;
	}

	/* Make sure we don't try to draw below 0 */
	if (x < 0)
	{
		w += x;
		x = 0;
	}

	/* Make sure we don't go off the edge of the display */
	if ((x + w) > DISPLAY_WIDTH)
	{
		w = (DISPLAY_WIDTH - x);
	}

	/* If our width is now negative, punt */
	if (w <= 0)
	{
		return;
	}

	/* Set up the pointer for  movement through the buffer */
	register uint8_t *pui8BufferPointer = g_pui8GraphicBuffer;
	/* Adjust the buffer pointer for the current row */
	pui8BufferPointer += ((y / 8) * DISPLAY_WIDTH);
	/* and offset x columns in */
	pui8BufferPointer += x;

	register uint8_t ui8PixelMask = 1 << (y & 7);

	switch (color)
	{
	case WHITE:
		while (w--)
		{
			*pui8BufferPointer++ |= ui8PixelMask;
		}
		break;
	case BLACK:
		ui8PixelMask = ~ui8PixelMask;
		while (w--)
		{
			*pui8BufferPointer++ &= ui8PixelMask;
		}
		break;
	case INVERSE:
		while (w--)
		{
			*pui8BufferPointer++ ^= ui8PixelMask;
		}
		break;
	}
}

/**
 * @brief  Draw horizontal line to the Graphic buffer
 * @param  x: Horizontal axis value of the 2D Cartesian coordinate.
 * @param  __y: Vertical axis inverted value of the 2D Cartesian coordinate.
 * @param  __h: Height of the line in pixel unit.
 * @param  color: Selected pixel state.
 *          This parameter can be one of the following values:
 *			@arg WHITE
 *			@arg BLACK
 *			@arg INVERSE
 * @retval None
 */
void graphic_drawFastVLine(int16_t x, int16_t __y, int16_t __h,
		pixel_color_t color)
{
	/* Do nothing if we're off the left or right side of the screen */
	if ((x < 0) || (x >= DISPLAY_WIDTH))
	{
		return;
	}

	/* Make sure we don't try to draw below 0 */
	if (__y < 0)
	{
		/* __y is negative, this will subtract enough from __h to account for __y being 0 */
		__h += __y;
		__y = 0;
	}

	/* Make sure we don't go past the height of the display */
	if ((__y + __h) > DISPLAY_HEIGHT)
	{
		__h = (DISPLAY_HEIGHT - __y);
	}

	/* If our height is now negative, punt */
	if (__h <= 0)
	{
		return;
	}

	/* This display doesn't need ints for coordinates,
	 use local byte registers for faster juggling */
	register uint8_t y = __y;
	register uint8_t h = __h;

	/* Set up the pointer for fast movement through the buffer */
	register uint8_t *pui8BufferPointer = g_pui8GraphicBuffer;
	/* Adjust the buffer pointer for the current row */
	pui8BufferPointer += ((y / 8) * DISPLAY_WIDTH);
	/* and offset x columns in */
	pui8BufferPointer += x;

	/* 1. Do the first partial byte, if necessary - this requires some masking */
	register uint8_t mod = (y & 7);
	if (mod)
	{
		/* Mask off the high n bits we want to set */
		mod = 8 - mod;

		/* NOTE - lookup table results in a nearly 10% performance improvement in fill* functions */
		// register uint8_t ui8PixelMask = ~(0xFF >> (mod));
		static uint8_t pui8PixelPreMask[8] =
		{ 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE };
		register uint8_t ui8PixelMask = pui8PixelPreMask[mod];

		/* Adjust the mask if we're not going to reach the end of this byte */
		if (h < mod)
		{
			ui8PixelMask &= (0XFF >> (mod - h));
		}

		switch (color)
		{
		case WHITE:
			*pui8BufferPointer |= ui8PixelMask;
			break;
		case BLACK:
			*pui8BufferPointer &= ~ui8PixelMask;
			break;
		case INVERSE:
			*pui8BufferPointer ^= ui8PixelMask;
			break;
		}

		/* Fast exit if we're done here! */
		if (h < mod)
		{
			return;
		}

		h -= mod;

		pui8BufferPointer += DISPLAY_WIDTH;
	}

	/* 2. Write solid bytes while we can - effectively doing 8 rows at a time */
	if (h >= 8)
	{
		if (INVERSE == color)
		{ /* separate copy of the code so we don't impact performance
		 of the black/white write version with an extra comparison per loop */
			do
			{
				*pui8BufferPointer = ~(*pui8BufferPointer);

				/* Adjust the buffer forward 8 rows worth of data */
				pui8BufferPointer += DISPLAY_WIDTH;

				/* Adjust h & y (there's got to be a faster way for me to do this,
				 but this should still help a fair bit for now) */
				h -= 8;
			} while (h >= 8);
		}
		else
		{
			/* Store a local value to work with */
			register uint8_t val = (color == WHITE) ? 255 : 0;

			do
			{
				/* Write our value in */
				*pui8BufferPointer = val;

				/* Adjust the buffer forward 8 rows worth of data */
				pui8BufferPointer += DISPLAY_WIDTH;

				/* Adjust h & y (there's got to be a faster way for me to do this,
				 but this should still help a fair bit for now) */
				h -= 8;
			} while (h >= 8);
		}
	}

	/* 3. Now do the final partial byte, if necessary */
	if (h)
	{
		mod = h & 7;
		/* This time we want to mask the low bits of the byte,
		 VS the high bits we did above */
		// register uint8_t ui8PixelMask = (1 << mod) - 1;
		/* NOTE - lookup table results in a nearly 10% performance improvement
		 in fill functions */
		static uint8_t pui8PixelPostMask[8] =
		{ 0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F };
		register uint8_t ui8PixelMask = pui8PixelPostMask[mod];
		switch (color)
		{
		case WHITE:
			*pui8BufferPointer |= ui8PixelMask;
			break;
		case BLACK:
			*pui8BufferPointer &= ~ui8PixelMask;
			break;
		case INVERSE:
			*pui8BufferPointer ^= ui8PixelMask;
			break;
		}
	}
}

/**
 * @brief  Draw rectangle shape to the Graphic buffer
 * @param  x: Horizontal axis value of the 2D Cartesian coordinate.
 * @param  y: Vertical axis inverted value of the 2D Cartesian coordinate.
 * @param  w: Width of the rectangle in pixel unit.
 * @param  h: Height of the rectangle in pixel unit.
 * @param  color: Selected pixel state.
 *          This parameter can be one of the following values:
 *			@arg WHITE
 *			@arg BLACK
 *			@arg INVERSE
 * @retval None
 */
void graphic_drawRect(int16_t x, int16_t y, int16_t w, int16_t h,
		pixel_color_t color)
{
	graphic_drawFastHLine(x, y, w, color);
	graphic_drawFastHLine(x, y + h - 1, w, color);
	graphic_drawFastVLine(x, y, h, color);
	graphic_drawFastVLine(x + w - 1, y, h, color);
}

/**
 * @brief  Fill rectangle shape to the Graphic buffer
 * @param  x: Horizontal axis value of the 2D Cartesian coordinate.
 * @param  y: Vertical axis inverted value of the 2D Cartesian coordinate.
 * @param  w: Width of the rectangle in pixel unit.
 * @param  h: Height of the rectangle in pixel unit.
 * @param  color: Selected pixel state.
 *          This parameter can be one of the following values:
 *			@arg WHITE
 *			@arg BLACK
 *			@arg INVERSE
 * @retval None
 */
void graphic_fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
		pixel_color_t color)
{
	for (int16_t i = x; i < x + w; i++)
	{
		graphic_drawFastVLine(i, y, h, color);
	}
}

/**
 * @} LIB_GRAPHIC
 */
/********************** (TM) PnL - Programming and Leverage ****END OF FILE****/
