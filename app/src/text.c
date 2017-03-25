/**
 ****************************************************************************
 * @file        text.c
 * @author      Long Dang
 * @version     V0.1
 * @date        19-March-2017
 * @copyright   LGPLv3
 * @brief       This file implement the Text library.
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
/** @defgroup LIB_TEXT Text
 * @{
 */
/* Includes ------------------------------------------------------------------*/
#include "text.h"
#include "font5x7.c"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define pgmReadByte(addr) (*(const unsigned char *)(addr)) /*!< Return byte data in the char pointer */
#define NUMBER_OF_CHARACTER	(8) /*!< Number of character supported in print number function. Avoid to use dynamic memory to increase performance. */

/* Private variables ---------------------------------------------------------*/
int32_t g_i32CursorX = 0;
int32_t g_i32CursorY = 0;
pixel_color_t g_colorForeground = WHITE;
pixel_color_t g_colorBackground = BLACK;
int32_t g_i32TextSize = 1;
bool g_bWrapText = false;

/* Private functions declaration ---------------------------------------------*/
void text_write(char c);

/* Private function prototypes -----------------------------------------------*/
/**
 * @brief  Push the character on screen at the current cursor position and update the cursor.
 * 		   Wrap test will be consider to print the remain text in the next line or skip it.
 * @param  c: Character in ASCII code.
 * @retval None
 */
void text_write(char c)
{
	if ('\n' == c)
	{
		/* New line: just update cursor, no need to draw anything */
		g_i32CursorY += (g_i32TextSize * FONT_HEIGHT);
		g_i32CursorX = 0;
	}
	else if ('\r' == c)
	{
		/* skip */
	}
	else
	{
		g_i32CursorX += text_drawChar(c, g_i32CursorX, g_i32CursorY,
				g_i32TextSize);
		if (g_bWrapText
				&& (g_i32CursorX > (DISPLAY_WIDTH - g_i32TextSize * FONT_WIDTH)))
		{
			g_i32CursorY += (g_i32TextSize * FONT_HEIGHT);
			g_i32CursorX = 0;
		}
		else
		{
			/* skip */
		}
	}
}

/* Exported functions prototype ----------------------------------------------*/
/**
 * @brief  Set the cursor position for drawing text.
 * @param  i32CursorX: Horizontal axis value of the 2D Cartesian coordinate.
 * @param  i32CursorY: Vertical axis inverted value of the 2D Cartesian coordinate.
 * @retval None
 */
void text_setCursor(int32_t i32CursorX, int32_t i32CursorY)
{
	g_i32CursorX = i32CursorX;
	g_i32CursorY = i32CursorY;
}

/**
 * @brief  Set the character size for drawing text.
 * @param  i32TextSize: Text size value.
 * @retval None
 */
void text_setTextSize(int32_t i32TextSize)
{
	g_i32TextSize = (i32TextSize > 0) ? (i32TextSize) : (1);
}

/**
 * @brief  Set the foreground and background color of the character for drawing text.
 * @param  colorForeground: Foreground color value.
 * @param  colorBackground: Background color value.
 *          This parameter can be one of the following values:
 *			@arg WHITE
 *			@arg BLACK
 *			@arg INVERSE
 * @retval None
 */
void text_setTextColor(pixel_color_t colorForeground,
		pixel_color_t colorBackground)
{
	g_colorForeground = colorForeground;
	g_colorBackground = colorBackground;
}

/**
 * @brief  Set enable/disable the wrap feature for drawing string.
 * @param  bWrapText: wrap text flag value.
 *          This parameter can be one of the following values:
 *			@arg true: wrap text when meet end of line.
 *			@arg false: do not wrap text.
 * @retval None
 */
void text_setWrapText(bool bWrapText)
{
	g_bWrapText = bWrapText;
}

/**
 * @brief  Draw a character at specific position (top, left) with specific size.
 * @param  c: Character in ASCII code
 * @param  i32CornerX: Left corner position of the character. This is horizontal axis value of the 2D Cartesian coordinate.
 * @param  i32CornerY: Top corner position of the character. This is vertical axis inverted value of the 2D Cartesian coordinate.
 * @param  i32Size: Size of the character.
 * @retval int32_t: Return the number of column cost by drawing the specific character.
 * 					This value is useful for adjust the cursor Y position.
 */
int32_t text_drawChar(char c, int32_t i32CornerX, int32_t i32CornerY,
		int32_t i32Size)
{
	if ((i32CornerX >= DISPLAY_WIDTH) || /* Clip right */
	(i32CornerY >= DISPLAY_HEIGHT) || /* Clip bottom */
	((i32CornerX + FONT_WIDTH * i32Size - 1) < 0) || /* Clip left */
	((i32CornerY + FONT_HEIGHT * i32Size - 1) < 0)) /* Clip top */
	{
		return 0;
	}

	int32_t i32ActualFontWidth = FONT_WIDTH - 1;
	int32_t i, j;
	for (i = 0; i < FONT_WIDTH; i++)
	{
		uint8_t ui8PixelColumnValue;
		if (i == i32ActualFontWidth)
		{
			ui8PixelColumnValue = 0x0; /* Boundary margin */
		}
		else
		{
			ui8PixelColumnValue = pgmReadByte(
					g_FontTable + (c * i32ActualFontWidth) + i);
		}

		for (j = 0; j < FONT_HEIGHT; j++)
		{
			if (ui8PixelColumnValue & 0x1)
			{
				if (1 == i32Size)
				{
					graphic_drawPixel(i32CornerX + i, i32CornerY + j,
							g_colorForeground);
				}
				else
				{
					graphic_fillRect(i32CornerX + (i * i32Size),
							i32CornerY + (j * i32Size), i32Size, i32Size,
							g_colorForeground);
				}
			}
			else if (g_colorBackground != g_colorForeground)
			{
				if (1 == i32Size)
				{
					graphic_drawPixel(i32CornerX + i, i32CornerY + j,
							g_colorBackground);
				}
				else
				{
					graphic_fillRect(i32CornerX + i * i32Size,
							i32CornerY + j * i32Size, i32Size, i32Size,
							g_colorBackground);
				}
			}
			ui8PixelColumnValue >>= 1;
		}
	}
	return (FONT_WIDTH * i32Size);
}

/**
 * @brief  Draw a string at specific position with specific size and alignment.
 * @param  pcString: Pointer to a character array in ASCII code.
 * @param  i32PositionX: Alignment position of the character. This is horizontal axis value of the 2D Cartesian coordinate.
 * @param  i32CornerY: Top corner position of the character. This is vertical axis inverted value of the 2D Cartesian coordinate.
 * @param  i32Size: Size of the character.
 * @param  align: Text alignment configuration.
 *          This parameter can be one of the following values:
 *			@arg LEFT
 *			@arg RIGHT
 *			@arg CENTER
 * @retval int32_t: Return the number of column cost by drawing the entire string.
 * 					This value is useful for adjust the cursor Y position.
 */
int32_t text_drawString(const char *pcString, int32_t i32PositionX,
		int32_t i32CornerY, int32_t i32Size, text_align_t align)
{
	char *pcPointer = pcString;

	/* Calculate the total pixel width of the string */
	int32_t i32TotalPixelWidth = 0;
	while (*pcPointer)
	{
		i32TotalPixelWidth += FONT_WIDTH;
		++pcPointer;
	}
	i32TotalPixelWidth = i32TotalPixelWidth * i32Size;

	/* Calculate the left corner position of the string */
	int32_t i32CornerX;
	if (CENTER == align)
	{
		i32CornerX = i32PositionX - i32TotalPixelWidth / 2;
	}
	else if (RIGHT == align)
	{
		i32CornerX = i32PositionX - i32TotalPixelWidth;
	}
	else /* LEFT alignment */
	{
		i32CornerX = i32PositionX;
	}

	/* Make sure the string not bound of the display border */
	if (i32CornerX < 0)
	{
		i32CornerX = 0;
	}

	int32_t i32LeftCornerX = i32CornerX;
	int32_t i32OffsetX;
	while (*pcString)
	{
		i32OffsetX = text_drawChar(*pcString, i32CornerX, i32CornerY, i32Size);
		i32CornerX += i32OffsetX; /* Move cursor right */
		pcString++;
	}

	return (i32CornerX - i32LeftCornerX);
}

/**
 * @brief  Print a string to the display at current cursor position.
 * @param  pcString: Pointer to a character array in ASCII code.
 * @retval None
 */
void text_printString(const char *pcString)
{
	while (*pcString)
	{
		text_write(*pcString);
		++pcString;
	}
}

/**
 * @brief  Print a number in ASCII format to the display at current cursor position.
 * @param  pcString: Pointer to a character array in ASCII code.
 * @retval None
 */
void text_printNumber(uint32_t ui32Value)
{
	char pcString[NUMBER_OF_CHARACTER + 1 /* End string */] =
	{ 0 };
	int i;
	do
	{
		/* Shift right all character number */
		for (i = (NUMBER_OF_CHARACTER - 1); i > 0; i--)
		{
			pcString[i] = pcString[i - 1];
		}
		/* Get the current character */
		pcString[0] = (ui32Value % 10) + '0';
		ui32Value /= 10;
	} while (ui32Value);

	text_printString(pcString);
}

/**
 * @brief  Print character by character of a string at current cursor position.
 * @param  pcString: Pointer to a character array in ASCII code.
 * @param  speed: Output text speed specification.
 *          This parameter can be one of the following values:
 *			@arg FAST
 *			@arg NORMAL
 *			@arg SLOW
 * @retval None
 */
void text_putString(const char *pcString, text_speed_t speed)
{
	while (*pcString)
	{
		text_write(*pcString);
		pcString++;
		/* FIXME: Render request update entire screen that cause power waste here.
		 Should only commit the difference part. */
		graphic_render();
		graphic_delay_ms(speed);
	}
}

/**
 * @brief  Project Trademark animation.
 * @retval None
 */
void text_showTrademarkAnimation(void)
{
	char *pcString = "<< powered by PnL >>";
	char *pcStringEmtpy = "                    ";
	int32_t i32StringLength = 20;
	int32_t i32CursorX = (DISPLAY_WIDTH - (i32StringLength * FONT_WIDTH)) / 2;
	int32_t i32CursorY = 12;

	graphic_clearRenderBuffer();
	text_setTextSize(1);

	/* Print in inverted color */
	text_setCursor(i32CursorX, i32CursorY);
	text_setTextColor(BLACK, WHITE);
	text_putString(pcString, FAST);
	graphic_delay_ms(1000);

	/* Flash screen */
	graphic_setInvertMode();
	graphic_delay_ms(100);
	graphic_setNormalMode();

	/* Print in normal color */
	text_setCursor(i32CursorX, i32CursorY);
	text_setTextColor(WHITE, BLACK);
	text_putString(pcString, NORMAL);
	graphic_delay_ms(500);

	/* Text out */
	text_setCursor(i32CursorX, i32CursorY);
	text_putString(pcStringEmtpy, SLOW);
}

/**
 * @} LIB_TEXT
 */
/********************** (TM) PnL - Programming and Leverage ****END OF FILE****/
