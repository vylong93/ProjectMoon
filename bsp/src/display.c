/**
 ****************************************************************************
 * @file        display.c
 * @author      Long Dang
 * @version     V0.1
 * @date        18-March-2017
 * @copyright   LGPLv3
 * @brief       This file content the implementation of the Display driver.
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
/** @defgroup DRV_DISPLAY Display
 * @{
 */
/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_bsp.h"
#include "lcd.h"
#include "display.h"
#include <string.h> /* memset() */

/* Private typedef -----------------------------------------------------------*/
/**
 * @typedef vcctype_t
 * This type define two VCC configuration of the OLED controller SSD1306.
 */
typedef enum
{
	VCCTYPE_EXTERNAL = 0x01, /*!< Using external VCC supply */
	VCCTYPE_SWITCHCAP = 0x02 /*!< Using internal VCC need to enable switch capacitors */
} vcctype_t;

/* Private define ------------------------------------------------------------*/
#define SSD1306_LCDWIDTH		(128) /*!< LCD width in pixel unit */
#define SSD1306_LCDHEIGHT		(32) /*!< LCD height in pixel unit */
#define DISPLAY_DATA_SIZE		(SSD1306_LCDWIDTH * SSD1306_LCDHEIGHT / 8) /*!< Unit in byte. Every 8 pixel in same column corresponding to one byte data (8-bit) */

/* Private macro -------------------------------------------------------------*/
/* SSD1306 command list */
#define SSD1306_SETCONTRAST			(0x81)
#define SSD1306_DISPLAYALLON_RESUME	(0xA4)
#define SSD1306_DISPLAYALLON		(0xA5)
#define SSD1306_NORMALDISPLAY		(0xA6)
#define SSD1306_INVERTDISPLAY		(0xA7)
#define SSD1306_DISPLAYOFF			(0xAE)
#define SSD1306_DISPLAYON			(0xAF)
#define SSD1306_SETDISPLAYOFFSET	(0xD3)
#define SSD1306_SETCOMPINS			(0xDA)
#define SSD1306_SETVCOMDETECT		(0xDB)
#define SSD1306_SETDISPLAYCLOCKDIV	(0xD5)
#define SSD1306_SETPRECHARGE		(0xD9)
#define SSD1306_SETMULTIPLEX		(0xA8)
#define SSD1306_SETLOWCOLUMN		(0x00)
#define SSD1306_SETHIGHCOLUMN		(0x10)
#define SSD1306_SETSTARTLINE		(0x40)
#define SSD1306_MEMORYMODE			(0x20)
#define SSD1306_COLUMNADDR			(0x21)
#define SSD1306_PAGEADDR  			(0x22)
#define SSD1306_COMSCANINC			(0xC0)
#define SSD1306_COMSCANDEC			(0xC8)
#define SSD1306_SEGREMAP			(0xA0)
#define SSD1306_CHARGEPUMP			(0x8D)

/* Scrolling definitions */
#define SSD1306_ACTIVATE_SCROLL							(0x2F)
#define SSD1306_DEACTIVATE_SCROLL						(0x2E)
#define SSD1306_SET_VERTICAL_SCROLL_AREA				(0xA3)
#define SSD1306_RIGHT_HORIZONTAL_SCROLL					(0x26)
#define SSD1306_LEFT_HORIZONTAL_SCROLL					(0x27)
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL	(0x29)
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL		(0x2A)

/* Private variables ---------------------------------------------------------*/
static const vcctype_t OLED_VCCTYPE = VCCTYPE_SWITCHCAP; /*!< This variable select the VCC configuration of the display_init() function */

/* FIXME: implement the best idea note in bsp_lcd_sendCommand to make the code cleaner: no magic number '1' here */
#define DISPLAY_BUFFER_SIZE	(1 + DISPLAY_DATA_SIZE)
static uint8_t g_ui8DisplayBuffer[DISPLAY_BUFFER_SIZE] = /*!< This is display buffer for the LCD and the format is <control byte 0x40> <data> */
{0x40 /* Control Byte - Co = Data */,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x80, 0x80, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xF8, 0xE0, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80,
0x80, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0xFF,
0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00,
0x80, 0xFF, 0xFF, 0x80, 0x80, 0x00, 0x80, 0x80, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x80, 0x80,
0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x8C, 0x8E, 0x84, 0x00, 0x00, 0x80, 0xF8,
0xF8, 0xF8, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xE0, 0xE0, 0xC0, 0x80,
0x00, 0xE0, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0xC7, 0x01, 0x01,
0x01, 0x01, 0x83, 0xFF, 0xFF, 0x00, 0x00, 0x7C, 0xFE, 0xC7, 0x01, 0x01, 0x01, 0x01, 0x83, 0xFF,
0xFF, 0xFF, 0x00, 0x38, 0xFE, 0xC7, 0x83, 0x01, 0x01, 0x01, 0x83, 0xC7, 0xFF, 0xFF, 0x00, 0x00,
0x01, 0xFF, 0xFF, 0x01, 0x01, 0x00, 0xFF, 0xFF, 0x07, 0x01, 0x01, 0x01, 0x00, 0x00, 0x7F, 0xFF,
0x80, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x01, 0xFF,
0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

0x03, 0x0F, 0x3F, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0xC7, 0xC7, 0x8F,
0x8F, 0x9F, 0xBF, 0xFF, 0xFF, 0xC3, 0xC0, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFC, 0xFC,
0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xF8, 0xF8, 0xF0, 0xF0, 0xE0, 0xC0, 0x00, 0x01, 0x03, 0x03, 0x03,
0x03, 0x03, 0x01, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01,
0x03, 0x01, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01, 0x03, 0x03, 0x00, 0x00,
0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x00, 0x00, 0x00, 0x01, 0x03, 0x01, 0x00, 0x00, 0x00, 0x03,
0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
static uint8_t *g_pui8DisplayDataBuffer = &g_ui8DisplayBuffer[1]; /*!< Pointer to the data region in the display buffer. Skip the control byte. */

static uint8_t g_ui8LogoDisplay[DISPLAY_BUFFER_SIZE] = /*!< This is the project LOGO in display buffer format */
{0x40 /* Control Byte - Co = Data */,
0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x80, 0xC0, 0x60, 0x60,
0x30, 0x30, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xB0, 0xF0, 0x60, 0xE2, 0xC0, 0x80, 0x00,
0x00, 0x00, 0x0C, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x04, 0x44, 0x00, 0x00, 0xFE, 0x4A, 0xFA, 0x0A,
0x0A, 0xEA, 0xEA, 0x2A, 0x2A, 0xEA, 0xCA, 0x0A, 0x8A, 0x8A, 0x8A, 0x0A, 0x0A, 0x8A, 0x8A, 0x8A,
0x8A, 0x0A, 0x0A, 0xAA, 0x0A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x0A, 0x0A, 0x8A, 0x8A, 0x8A,
0x0A, 0x8A, 0xCA, 0xCA, 0x8A, 0x0A, 0x0A, 0xFE, 0x92, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00,
0x00, 0x00, 0x00, 0x00, 0x80, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x80,
0x00, 0x02, 0x00, 0x00, 0x02, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,

0x00, 0x00, 0x00, 0x06, 0x06, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x1C, 0x87, 0x81, 0xD0, 0xB0, 0xA8,
0xAC, 0xC4, 0xDC, 0x94, 0xC2, 0xAC, 0xAC, 0xBE, 0x94, 0xD7, 0x93, 0x8E, 0xCF, 0x39, 0xE5, 0xFF,
0xFC, 0xF0, 0x00, 0x00, 0x00, 0x04, 0x00, 0x40, 0x20, 0x00, 0x00, 0x00, 0xFF, 0x92, 0xFF, 0x00,
0x00, 0x1F, 0x1F, 0x02, 0x02, 0x03, 0x01, 0x00, 0x1F, 0x1F, 0x00, 0x00, 0x0F, 0x1F, 0x11, 0x11,
0x11, 0xDF, 0xC0, 0xFF, 0x00, 0x1F, 0x1F, 0x12, 0x12, 0x12, 0x13, 0x00, 0x0F, 0x1F, 0x19, 0x10,
0x10, 0x00, 0x1F, 0x1F, 0x10, 0x00, 0x00, 0xFF, 0x24, 0xFF, 0x00, 0x00, 0x00, 0x82, 0x80, 0x80,
0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC1, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0xE0, 0x40, 0x00,
0x00, 0x00, 0x00, 0x00, 0x3C, 0xFC, 0xFC, 0xF0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,

0x00, 0x01, 0x02, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x3A, 0xE4, 0xD8, 0xAA, 0x16, 0x12,
0x36, 0xED, 0xF4, 0xFC, 0xCE, 0xCA, 0xCA, 0x2A, 0x3B, 0xF9, 0x1F, 0xFB, 0xFF, 0xCB, 0xFF, 0xF3,
0x3F, 0x0F, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x24, 0xFF, 0x00,
0x00, 0xF8, 0xF8, 0x38, 0xF0, 0xC0, 0x80, 0xF0, 0x38, 0xF8, 0x00, 0xC0, 0x60, 0x20, 0x20, 0x60,
0xE0, 0xC0, 0x00, 0xC0, 0xE0, 0x60, 0x20, 0x60, 0xC0, 0x00, 0xE0, 0xE0, 0x60, 0x60, 0xE0, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x49, 0xFF, 0x00, 0x00, 0x00, 0x0F, 0x1F, 0xFF,
0xFF, 0xFB, 0xF1, 0xF9, 0x7F, 0x3F, 0x1F, 0x00, 0x00, 0xC0, 0xC0, 0xC0, 0xC0, 0xE0, 0xE0, 0xF0,
0xF0, 0xF0, 0xF0, 0xF0, 0x80, 0x01, 0x1F, 0xFF, 0xFF, 0xF8, 0xC0, 0xE0, 0xF0, 0xF0, 0xF0, 0x70,

0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x00, 0x00, 0x40, 0x00, 0x01, 0x03, 0x07, 0x46,
0x0E, 0x0F, 0x1E, 0x1F, 0x1F, 0x1D, 0x1D, 0x1F, 0x1F, 0x0F, 0x0F, 0x07, 0x07, 0x03, 0x01, 0x00,
0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x20, 0x00, 0x7F, 0x49, 0x7F, 0x50,
0x50, 0x57, 0x57, 0x50, 0x51, 0x57, 0x57, 0x51, 0x50, 0x57, 0x50, 0x53, 0x57, 0x56, 0x56, 0x57,
0x53, 0x51, 0x50, 0x53, 0x57, 0x56, 0x54, 0x56, 0x53, 0x50, 0x57, 0x57, 0x50, 0x50, 0x57, 0x56,
0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x5F, 0x52, 0x7F, 0x00, 0x00, 0x06, 0x00, 0x40, 0x00,
0x07, 0x7F, 0xFF, 0xFE, 0xF0, 0x00, 0x00, 0x02, 0x00, 0x03, 0x1F, 0x3F, 0x3F, 0x3F, 0x01, 0x00,
0x00, 0x01, 0x4F, 0x4F, 0x0F, 0x0F, 0x00, 0x03, 0x03, 0x23, 0x03, 0x03, 0x01, 0x31, 0x00, 0x00
};

/* Private functions declaration ---------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Exported functions prototype ----------------------------------------------*/
/**
 * @brief  Initialize the display driver to control the OLED LCD SSD1306.
 * @retval None
 */
void display_init(void)
{
	bsp_lcd_sendCommand(SSD1306_DISPLAYOFF); /* 0xAE */
	bsp_lcd_sendCommand(SSD1306_SETDISPLAYCLOCKDIV); /* 0xD5 */
	bsp_lcd_sendCommand(0x80); /* The suggested ratio 0x80 */

	bsp_lcd_sendCommand(SSD1306_SETMULTIPLEX); /* 0xA8 */
	bsp_lcd_sendCommand(SSD1306_LCDHEIGHT - 1);

	bsp_lcd_sendCommand(SSD1306_SETDISPLAYOFFSET); /* 0xD3 */
	bsp_lcd_sendCommand(0); /* No Offset */
	bsp_lcd_sendCommand(SSD1306_SETSTARTLINE | 0x0); /* Line #0 */
	bsp_lcd_sendCommand(SSD1306_CHARGEPUMP); /* 0x8D */
	if (OLED_VCCTYPE == VCCTYPE_EXTERNAL)
	{
		bsp_lcd_sendCommand(0x10);
	}
	else
	{
		bsp_lcd_sendCommand(0x14);
	}
	bsp_lcd_sendCommand(SSD1306_MEMORYMODE); /* 0x20 */
	bsp_lcd_sendCommand(0x00); /* 0x00 act like ks0108 */
	bsp_lcd_sendCommand(SSD1306_SEGREMAP | 0x1);
	bsp_lcd_sendCommand(SSD1306_COMSCANDEC);

	/* For OLED LCD 128x32 */
	bsp_lcd_sendCommand(SSD1306_SETCOMPINS); /* 0xDA */
	bsp_lcd_sendCommand(0x02);
	bsp_lcd_sendCommand(SSD1306_SETCONTRAST); /* 0x81 */
	bsp_lcd_sendCommand(0x8F);

	bsp_lcd_sendCommand(SSD1306_SETPRECHARGE); /* 0xd9 */
	if (OLED_VCCTYPE == VCCTYPE_EXTERNAL)
	{
		bsp_lcd_sendCommand(0x22);
	}
	else
	{
		bsp_lcd_sendCommand(0xF1);
	}
	bsp_lcd_sendCommand(SSD1306_SETVCOMDETECT); /* 0xDB */
	bsp_lcd_sendCommand(0x40);
	bsp_lcd_sendCommand(SSD1306_DISPLAYALLON_RESUME); /* 0xA4 */
	bsp_lcd_sendCommand(SSD1306_NORMALDISPLAY); /* 0xA6 */

	bsp_lcd_sendCommand(SSD1306_DEACTIVATE_SCROLL);

	/* Turn on OLED panel */
	bsp_lcd_sendCommand(SSD1306_DISPLAYON);
}

/**
 * @brief  Turn on the LCD display panel.
 * @retval None
 */
void display_turnOn(void)
{
	bsp_lcd_sendCommand(SSD1306_DISPLAYON);
}

/**
 * @brief  Turn off the LCD display panel.
 * @retval None
 */
void display_turnOff(void)
{
	bsp_lcd_sendCommand(SSD1306_DISPLAYOFF);
}

/**
 * @brief  Change the LCD display contrast.
 *			The range of the contrast too small to be really useful but
 *			it is useful to dim the display.
 * @param  bIsDim: contrast value selection
 *			@arg true: minimum value (Dim effect)
 *			@arg false: normal value
 * @retval None
 */
void display_setDim(bool bIsDim)
{
	uint8_t ui8ContrastValue;
	if (bIsDim)
	{
		ui8ContrastValue = 0;
	}
	else
	{
		ui8ContrastValue = (VCCTYPE_EXTERNAL == OLED_VCCTYPE) ? (0x9F) : (0xCF);
	}
	bsp_lcd_sendCommand(SSD1306_SETCONTRAST);
	bsp_lcd_sendCommand(ui8ContrastValue);
}

/**
 * @brief  Transmit the display buffer to the LCD device.
 * @note   This function will override the current display data which cause extra power cost.
 *         @arg Idea: For better approach, this function should support renew part of the display.
 *		   @arg Risk: Must consider the extra computation cost to determine the column and page to update.
 * @retval None
 */
void display_render(void)
{
	bsp_lcd_sendCommand(SSD1306_COLUMNADDR);
	bsp_lcd_sendCommand(0); /* Column start address (0 = reset) */
	bsp_lcd_sendCommand(SSD1306_LCDWIDTH - 1); /* Column end address (127 = reset) */

	bsp_lcd_sendCommand(SSD1306_PAGEADDR);
	bsp_lcd_sendCommand(0); /* Page start address (0 = reset) */
	bsp_lcd_sendCommand(3); /* Page end address for Height in 32pxl */

	bsp_lcd_sendData((uint8_t *) g_ui8DisplayBuffer, DISPLAY_BUFFER_SIZE);
}

/**
 * @brief  Transmit the LOGO buffer to the LCD device in fullscreen mode.
 * @retval None
 */
void display_render_logo(void)
{
	bsp_lcd_sendCommand(SSD1306_COLUMNADDR);
	bsp_lcd_sendCommand(0); /* Column start address (0 = reset) */
	bsp_lcd_sendCommand(SSD1306_LCDWIDTH - 1); /* Column end address (127 = reset) */

	bsp_lcd_sendCommand(SSD1306_PAGEADDR);
	bsp_lcd_sendCommand(0); /* Page start address (0 = reset) */
	bsp_lcd_sendCommand(3); /* Page end address for Height in 32pxl */

	bsp_lcd_sendData((uint8_t *) g_ui8LogoDisplay, DISPLAY_BUFFER_SIZE);
}

/**
 * @brief  Zero all the data in the display buffer to turn off all the pixels.
 * @retval None
 */
void display_clearRenderBuffer(void)
{
	memset(g_pui8DisplayDataBuffer, 0, DISPLAY_DATA_SIZE);
}

/**
 * @brief  Invert the display pixel role.
 * 			@arg bit_0: on
 * 			@arg bit_1: off
 * @retval None
 */
void display_setInvertMode(void)
{
	bsp_lcd_sendCommand(SSD1306_INVERTDISPLAY);
}

/**
 * @brief  Reset the display pixel role.
 * 			@arg bit_0: off
 * 			@arg bit_1: on
 * @retval None
 */
void display_setNormalMode(void)
{
	bsp_lcd_sendCommand(SSD1306_NORMALDISPLAY);
}

/**
 * @} DRV_DISPLAY
 */
/********************** (TM) PnL - Programming and Leverage ****END OF FILE****/
