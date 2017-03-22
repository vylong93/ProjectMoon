/**
 ****************************************************************************
 * @file        main.c
 * @author      Long Dang
 * @version     V0.1
 * @date        18-December-2016
 * @copyright   LGPLv3
 * @brief       This file provides main application.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define TESTING /*!< Enable this flag to active the testing functions. */
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
#ifdef TESTING
/**
 * @brief  Test the display driver.
 * @note   Expectation:
 * 			@arg LCD display project LOGO in one second at dim mode.
 * 			@arg LCD display project LOGO in one second at normal contrast.
 * 			@arg LCD turn off in one second.
 * 			@arg LCD turn on and the project LOGO show back in one second.
 * 			@arg LCD invert the pixel in one second.
 * 			@arg LCD return normal display mode and scroll Left hold screen in one second
 * 			@arg scroll right hold screen in one second
 * 			@arg scroll diagleft hold screen in one second
 * 			@arg scroll diagRight hold screen in one second
 * 			@arg stop scroll in one second
 * 			@arg scroll left page 1 and 2 in one second
 * 			@arg scroll right page 3 in one second
 * 			@arg scroll diagleft page 2 and 3 in three second
 * 			@arg scroll diagright page 0 and 1 in three second
 * 			@arg scroll up 11.428ms interval in three second
 * 			@arg scroll up 17.143ms interval in three second
 * 			@arg scroll up 22.857ms interval in three second
 * 			@arg scroll up 28.571ms interval in three second
 * 			@arg scroll up 142.857ms interval in three second
 * 			@arg scroll up 365.714ms interval in three second
 * 			@arg scroll up 731.429ms interval in three second
 * 			@arg scroll up 1462.857ms interval in three second
 * 			@arg stop scroll
 * @retval None
 */
void test_DisplayDriver(void)
{
	display_render_logo();
	display_setDim(false);
	bsp_delay_ms(1000);
	display_setDim(true);
	bsp_delay_ms(1000);
	display_turnOff();
	bsp_delay_ms(1000);
	display_turnOn();
	bsp_delay_ms(1000);
	display_setInvertMode();
	bsp_delay_ms(1000);
	display_setNormalMode();
	display_scrollLeft(DISP_PAGE_0, DISP_PAGE_3);
	bsp_delay_ms(1000);
	display_scrollRight(DISP_PAGE_0, DISP_PAGE_3);
	bsp_delay_ms(1000);
	display_scrollDiagLeft(DISP_PAGE_0, DISP_PAGE_3);
	bsp_delay_ms(1000);
	display_scrollDiagRight(DISP_PAGE_0, DISP_PAGE_3);
	bsp_delay_ms(1000);
	display_scrollStop();
	bsp_delay_ms(1000);
	display_render_logo();
	display_scrollLeft(DISP_PAGE_1, DISP_PAGE_2);
	bsp_delay_ms(1000);
	display_render_logo();
	display_scrollRight(DISP_PAGE_3, DISP_PAGE_3);
	bsp_delay_ms(1000);
	display_render_logo();
	display_scrollDiagLeft(DISP_PAGE_2, DISP_PAGE_3);
	bsp_delay_ms(3000);
	display_render_logo();
	display_scrollDiagRight(DISP_PAGE_0, DISP_PAGE_1);
	bsp_delay_ms(3000);
	display_render_logo();
	display_scrollUp(SCROLL_BY_2FPS);
	bsp_delay_ms(3000);
	display_scrollUp(SCROLL_BY_3FPS);
	bsp_delay_ms(3000);
	display_scrollUp(SCROLL_BY_4FPS);
	bsp_delay_ms(3000);
	display_scrollUp(SCROLL_BY_5FPS);
	bsp_delay_ms(3000);
	display_scrollUp(SCROLL_BY_25FPS);
	bsp_delay_ms(3000);
	display_scrollUp(SCROLL_BY_64FPS);
	bsp_delay_ms(3000);
	display_scrollUp(SCROLL_BY_128FPS);
	bsp_delay_ms(3000);
	display_scrollUp(SCROLL_BY_256FPS);
	bsp_delay_ms(3000);
	display_render_logo();
	display_scrollStop();
}

/**
 * @brief  Test the Graphic library.
 * @note   Expectation:
 * 			@arg LCD display the top left pixel in one second.
 * 			@arg LCD display the middle pixel in one second.
 * 			@arg LCD display the bottom right pixel in one second.
 * 			@arg LCD display the bottom left pixel in one second.
 * 			@arg LCD display the top right pixel in one second.
 * 			@arg LCD display the 1st horizontal line in one second.
 * 			@arg LCD display not draw anything in one second.
 * 			@arg LCD display the 2nd horizontal line in one second.
 * 			@arg LCD display the 1st vertical line in one second.
* 			@arg LCD display the 2nd vertical line in one second.
* 			@arg LCD display the 3dr vertical line in one second.
* 			@arg LCD display not draw anything in one second.
 * @retval None
 */
void test_GraphicLibrary(void)
{
	display_clearRenderBuffer();

	graphic_drawPixel(0, 0, WHITE);
	display_render();
	bsp_delay_ms(1000);

	graphic_drawPixel(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2, WHITE);
	display_render();
	bsp_delay_ms(1000);

	graphic_drawPixel(DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1, WHITE);
	display_render();
	bsp_delay_ms(1000);

	graphic_drawPixel(0, DISPLAY_HEIGHT - 1, WHITE);
	display_render();
	bsp_delay_ms(1000);

	graphic_drawPixel(DISPLAY_WIDTH - 1, 0, WHITE);
	display_render();
	bsp_delay_ms(1000);

	/* Normal-Boundary-Abnormal */
	graphic_drawFastHLine(5, 5, DISPLAY_WIDTH / 2, WHITE);
	display_render();
	bsp_delay_ms(1000);
	graphic_drawFastHLine(-1, -4, DISPLAY_WIDTH / 2, WHITE);
	display_render();
	bsp_delay_ms(1000);
	graphic_drawFastHLine(-5, 7, DISPLAY_WIDTH / 2, WHITE);
	display_render();
	bsp_delay_ms(1000);

	/* Normal-Boundary-Abnormal */
	graphic_drawFastVLine(DISPLAY_WIDTH / 2, 2, DISPLAY_HEIGHT * 2 / 3, INVERSE);
	display_render();
	bsp_delay_ms(1000);
	graphic_drawFastVLine(DISPLAY_WIDTH / 2 + 1, 2, DISPLAY_HEIGHT, INVERSE);
	display_render();
	bsp_delay_ms(1000);
	graphic_drawFastVLine(DISPLAY_WIDTH / 2 + 2, -10, DISPLAY_HEIGHT, INVERSE);
	display_render();
	bsp_delay_ms(1000);
	graphic_drawFastVLine(DISPLAY_WIDTH / 2 + 3, -10, -3, INVERSE);
	display_render();
	bsp_delay_ms(1000);
}
#endif
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Main program                                                    */
/******************************************************************************/
/**
 * @brief  Main program
 * @retval None
 */
int main(void)
{
	bsp_init();
#ifdef TESTING
	test_DisplayDriver();
	test_GraphicLibrary();
#endif
	while (true)
	{
		bsp_led_toggle(LED_RED);
		bsp_delay_ms(1000);
	}
}

/********************** (TM) PnL - Programming and Leverage ****END OF FILE****/
