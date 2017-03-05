/**
 ****************************************************************************
 * @file	main.c
 * @author	Long Dang
 * @version	V1.0.0
 * @date	18 Dec 2016
 * @brief	This file provides main application.
 ****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2017 PnL </center></h2>
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
#include <stm32f1xx_bsp.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Main program                                                    */
/******************************************************************************/
/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
int main(void)
{
	bsp_init();

	while (true)
	{
		bsp_delay_ms(1000);
	}
}

/***************************** (C) COPYRIGHT 2017 PnL **********END OF FILE****/
