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
#include "lcd.h"

/* Exported types ------------------------------------------------------------*/
/**
 * @typedef display_page_t
 * This type define the available page of the OLED LCD 128x32.
 */
typedef enum
{
	DISP_PAGE_0 = 0x00, /*!< PAGE0 from row 0 to 7 */
	DISP_PAGE_1 = 0x01, /*!< PAGE1 from row 8 to 15 */
	DISP_PAGE_2 = 0x02, /*!< PAGE2 from row 16 to 23 */
	DISP_PAGE_3 = 0x03 /*!< PAGE3 from row 24 to 31 */
} display_page_t;

/**
 * @typedef scroll_interval_t
 * This type define the time interval between each scroll step in terms of frame frequency (175FPS ~ 5.714ms).
 */
typedef enum
{
	SCROLL_BY_2FPS = 0x07, /*!< 11.428ms interval */
	SCROLL_BY_3FPS = 0x04, /*!< 17.143ms interval */
	SCROLL_BY_4FPS = 0x05, /*!< 22.857ms interval */
	SCROLL_BY_5FPS = 0x00, /*!< 28.571ms interval */
	SCROLL_BY_25FPS = 0x06, /*!< 142.857ms interval */
	SCROLL_BY_64FPS = 0x01, /*!< 365.714ms interval */
	SCROLL_BY_128FPS = 0x02, /*!< 731.429ms interval */
	SCROLL_BY_256FPS = 0x03 /*!< 1462.857ms interval */
} scroll_interval_t;

/* Exported constants --------------------------------------------------------*/
#define DISPLAY_WIDTH		(128) /*!< LCD width in pixel unit */
#define DISPLAY_HEIGHT		(32) /*!< LCD height in pixel unit */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
bool display_init(void);
void display_turnOn(void);
void display_turnOff(void);
void display_setDim(bool bIsDim);
uint8_t* display_getRenderBufferPointer(void);
void display_render(void);
void display_render_logo(void);
void display_clearRenderBuffer(void);
void display_setInvertMode(void);
void display_setNormalMode(void);
void display_scrollUp(scroll_interval_t scrollInterval);
void display_scrollRight(display_page_t pageStart, display_page_t pageStop);
void display_scrollLeft(display_page_t pageStart, display_page_t pageStop);
void display_scrollDiagRight(display_page_t pageStart, display_page_t pageStop);
void display_scrollDiagLeft(display_page_t pageStart, display_page_t pageStop);
void display_scrollStop(void);

/**
 * @} DRV_DISPLAY
 */
/**
 * @} DRV
 */
#endif /* DISPLAY_H_ */

/********************** (TM) PnL - Programming and Leverage ****END OF FILE****/
