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
	display_delay_ms(1000);
	display_setDim(true);
	display_delay_ms(1000);
	display_turnOff();
	display_delay_ms(1000);
	display_turnOn();
	display_delay_ms(1000);
	display_setInvertMode();
	display_delay_ms(1000);
	display_setNormalMode();
	display_scrollLeft(DISP_PAGE_0, DISP_PAGE_3);
	display_delay_ms(1000);
	display_scrollRight(DISP_PAGE_0, DISP_PAGE_3);
	display_delay_ms(1000);
	display_scrollDiagLeft(DISP_PAGE_0, DISP_PAGE_3);
	display_delay_ms(1000);
	display_scrollDiagRight(DISP_PAGE_0, DISP_PAGE_3);
	display_delay_ms(1000);
	display_scrollStop();
	display_delay_ms(1000);
	display_render_logo();
	display_scrollLeft(DISP_PAGE_1, DISP_PAGE_2);
	display_delay_ms(1000);
	display_render_logo();
	display_scrollRight(DISP_PAGE_3, DISP_PAGE_3);
	display_delay_ms(1000);
	display_render_logo();
	display_scrollDiagLeft(DISP_PAGE_2, DISP_PAGE_3);
	display_delay_ms(3000);
	display_render_logo();
	display_scrollDiagRight(DISP_PAGE_0, DISP_PAGE_1);
	display_delay_ms(3000);
	display_render_logo();
	display_scrollUp(SCROLL_BY_2FPS);
	display_delay_ms(3000);
	display_scrollUp(SCROLL_BY_3FPS);
	display_delay_ms(3000);
	display_scrollUp(SCROLL_BY_4FPS);
	display_delay_ms(3000);
	display_scrollUp(SCROLL_BY_5FPS);
	display_delay_ms(3000);
	display_scrollUp(SCROLL_BY_25FPS);
	display_delay_ms(3000);
	display_scrollUp(SCROLL_BY_64FPS);
	display_delay_ms(3000);
	display_scrollUp(SCROLL_BY_128FPS);
	display_delay_ms(3000);
	display_scrollUp(SCROLL_BY_256FPS);
	display_delay_ms(3000);
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
	graphic_clearRenderBuffer();

	graphic_drawPixel(0, 0, WHITE);
	graphic_render();
	graphic_delay_ms(1000);

	graphic_drawPixel(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2, WHITE);
	graphic_render();
	graphic_delay_ms(1000);

	graphic_drawPixel(DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1, WHITE);
	graphic_render();
	graphic_delay_ms(1000);

	graphic_drawPixel(0, DISPLAY_HEIGHT - 1, WHITE);
	graphic_render();
	graphic_delay_ms(1000);

	graphic_drawPixel(DISPLAY_WIDTH - 1, 0, WHITE);
	graphic_render();
	graphic_delay_ms(1000);

	/* Normal-Boundary-Abnormal */
	graphic_drawFastHLine(5, 5, DISPLAY_WIDTH / 2, WHITE);
	graphic_render();
	graphic_delay_ms(1000);
	graphic_drawFastHLine(-1, -4, DISPLAY_WIDTH / 2, WHITE);
	graphic_render();
	graphic_delay_ms(1000);
	graphic_drawFastHLine(-5, 7, DISPLAY_WIDTH / 2, WHITE);
	graphic_render();
	graphic_delay_ms(1000);

	/* Normal-Boundary-Abnormal */
	graphic_drawFastVLine(DISPLAY_WIDTH / 2, 2, DISPLAY_HEIGHT * 2 / 3,
			INVERSE);
	graphic_render();
	graphic_delay_ms(1000);
	graphic_drawFastVLine(DISPLAY_WIDTH / 2 + 1, 2, DISPLAY_HEIGHT, INVERSE);
	graphic_render();
	graphic_delay_ms(1000);
	graphic_drawFastVLine(DISPLAY_WIDTH / 2 + 2, -10, DISPLAY_HEIGHT, INVERSE);
	graphic_render();
	graphic_delay_ms(1000);
	graphic_drawFastVLine(DISPLAY_WIDTH / 2 + 3, -10, -3, INVERSE);
	graphic_render();
	graphic_delay_ms(1000);

	graphic_drawRect(2, 2, DISPLAY_WIDTH - 4, DISPLAY_HEIGHT - 4, WHITE);
	graphic_render();
	graphic_delay_ms(1000);

	graphic_fillRect(DISPLAY_WIDTH / 4, DISPLAY_HEIGHT / 4, DISPLAY_WIDTH / 2,
	DISPLAY_HEIGHT / 2, INVERSE);
	graphic_render();
	graphic_delay_ms(1000);
}

/**
 * @brief  Test the Text library.
 * @note   Expectation:
 * 			@arg LCD display the small character 'a' in one second.
 * 			@arg LCD display the big character '1' in one second.
 * 			@arg LCD display part of the big test string in one second.
 * 			@arg LCD display the test string in middle screen in one second.
 * 			@arg LCD display the test string in right right corner in one second.
 * 			@arg LCD display clear in one second.
 * 			@arg LCD display alphabet in medium size in one second.
 * 			@arg LCD display string number order in big size in one second.
 * 			@arg LCD display a test number in big size in one second.
 * 			@arg LCD display clear in one second.
 * 			@arg LCD display character by character of the test string in one second.
 * @retval None
 */
void test_TextLibrary(void)
{
	char *pcTestString = "Project Moon\n";
	graphic_clearRenderBuffer();

	text_drawChar('a', 2, 2, 1);
	graphic_render();
	graphic_delay_ms(1000);

	text_drawChar('1', 10, 2, 2);
	graphic_render();
	graphic_delay_ms(1000);

	text_drawString(pcTestString, DISPLAY_WIDTH / 3, 8, 2, LEFT);
	graphic_render();
	graphic_delay_ms(1000);

	text_drawString(pcTestString, DISPLAY_WIDTH / 2, 16, 1, CENTER);
	graphic_render();
	graphic_delay_ms(1000);

	text_drawString(pcTestString, DISPLAY_WIDTH, 24, 1, RIGHT);
	graphic_render();
	graphic_delay_ms(1000);

	graphic_clearRenderBuffer();
	graphic_render();
	graphic_delay_ms(1000);

	text_setWrapText(true);
	text_setCursor(1, 1);
	text_setTextSize(2);
	text_printString("abcdefghijklmnopqrstuvxyw\n");
	graphic_render();
	graphic_delay_ms(1000);

	text_setCursor(3, 3);
	text_printString("0123456789\n");
	graphic_render();
	graphic_delay_ms(1000);

	text_setCursor(0, 5);
	text_setTextSize(3);
	text_printNumber(97531);
	graphic_render();
	graphic_delay_ms(1000);

	graphic_clearRenderBuffer();
	graphic_render();
	graphic_delay_ms(1000);

	text_setCursor(0, 2);
	text_setTextSize(1);
	text_putString(pcTestString, SLOW);
	text_putString(pcTestString, NORMAL);
	text_putString(pcTestString, FAST);
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
	if (!bsp_init())
	{
		goto error;
	}

	/* Enable the LED0 */
	bsp_led_init(LED_RED);

	if (!graphic_init())
	{
		goto error;
	}

#ifdef TESTING
	test_DisplayDriver();
	test_GraphicLibrary();
	test_TextLibrary();
#endif
	while (true)
	{
		bsp_led_toggle(LED_RED);
		bsp_delay_ms(1000);
	}
	error: bsp_led_on(LED_RED);
	while (true);
}

/********************** (TM) PnL - Programming and Leverage ****END OF FILE****/
