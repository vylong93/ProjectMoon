/**
 ****************************************************************************
 * @file        button.c
 * @author      Long Dang
 * @version     V0.1
 * @date        01-May-2017
 * @copyright   LGPLv3
 * @brief       This file implement the button IO handlers.
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
/** @addtogroup BSP_DEVICE_BUTTON
 * @{
 */
/** @defgroup BSP_DEVICE_BUTTON_PRIVATE Button (Private)
 * @{
 */
/* Includes ------------------------------------------------------------------*/
#include "button.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @addtogroup BSP_INT_PRIORITY
 * @{
 */
#define BUTTONx_EXTI_IRQ_PRIORITY 		(15)
/**@}BSP_INT_PRIORITY*/

#define BUTTON0_PIN						GPIO_PIN_0
#define BUTTON0_PORT					GPIOB
#define BUTTON0_EXTI_IRQn				EXTI0_IRQn

#define BUTTON1_PIN						GPIO_PIN_1
#define BUTTON1_PORT					GPIOA
#define BUTTON1_EXTI_IRQn				EXTI1_IRQn

#define BUTTON2_PIN						GPIO_PIN_3
#define BUTTON2_PORT					GPIOB
#define BUTTON2_EXTI_IRQn				EXTI3_IRQn

#define BUTTON3_PIN						GPIO_PIN_4
#define BUTTON3_PORT					GPIOB
#define BUTTON3_EXTI_IRQn				EXTI4_IRQn

#define BUTTON4_PIN						GPIO_PIN_5
#define BUTTON4_PORT					GPIOB
#define BUTTON4_EXTI_IRQn				EXTI9_5_IRQn /* Please double check your datasheet */

#define BUTTON5_PIN						GPIO_PIN_9
#define BUTTON5_PORT					GPIOB
#define BUTTON5_EXTI_IRQn				EXTI9_5_IRQn /* Please double check your datasheet */

#define BUTTON6_PIN						GPIO_PIN_10
#define BUTTON6_PORT					GPIOB
#define BUTTON6_EXTI_IRQn				EXTI15_10_IRQn /* Please double check your datasheet */

#define BUTTON7_PIN						GPIO_PIN_11
#define BUTTON7_PORT					GPIOB
#define BUTTON7_EXTI_IRQn				EXTI15_10_IRQn /* Please double check your datasheet */

#define BUTTON8_PIN						GPIO_PIN_15
#define BUTTON8_PORT					GPIOA
#define BUTTON8_EXTI_IRQn				EXTI15_10_IRQn /* Please double check your datasheet */

//#define LEDx_GPIO_CLK_ENABLE(__INDEX__)   do { if((__INDEX__) == 0) LED0_GPIO_CLK_ENABLE();} while(0)

#define BUTTONx_GPIO_CLK_ENABLE(__INDEX__)	{ if((__INDEX__) < BUTTONn) {\
												if(GPIOA == BUTTON_PORT[__INDEX__]) __HAL_RCC_GPIOA_CLK_ENABLE();\
												else if(GPIOB == BUTTON_PORT[__INDEX__]) __HAL_RCC_GPIOB_CLK_ENABLE();\
												else if(GPIOC == BUTTON_PORT[__INDEX__]) __HAL_RCC_GPIOC_CLK_ENABLE();\
												else if(GPIOD == BUTTON_PORT[__INDEX__]) __HAL_RCC_GPIOD_CLK_ENABLE();} }
#define BUTTONx_GPIO_CLK_DISABLE(__INDEX__)	{ if((__INDEX__) < BUTTONn) {\
												if(GPIOA == BUTTON_PORT[__INDEX__]) __HAL_RCC_GPIOA_CLK_DISABLE();\
												else if(GPIOB == BUTTON_PORT[__INDEX__]) __HAL_RCC_GPIOB_CLK_DISABLE();\
												else if(GPIOC == BUTTON_PORT[__INDEX__]) __HAL_RCC_GPIOC_CLK_DISABLE();\
												else if(GPIOD == BUTTON_PORT[__INDEX__]) __HAL_RCC_GPIOD_CLK_DISABLE();} }

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static button_callback_t g_pButtonCallbackArray[BUTTONn] =
{
{ 0 },
{ 0 },
{ 0 },
{ 0 },
{ 0 },
{ 0 },
{ 0 },
{ 0 },
{ 0 } };

const uint16_t BUTTON_PIN[BUTTONn] =
{
BUTTON0_PIN, BUTTON1_PIN, BUTTON2_PIN,
BUTTON3_PIN, BUTTON4_PIN, BUTTON5_PIN,
BUTTON6_PIN, BUTTON7_PIN, BUTTON8_PIN }; /*!< 's pin array */

GPIO_TypeDef* BUTTON_PORT[BUTTONn] =
{
BUTTON0_PORT, BUTTON1_PORT, BUTTON2_PORT,
BUTTON3_PORT, BUTTON4_PORT, BUTTON5_PORT,
BUTTON6_PORT, BUTTON7_PORT, BUTTON8_PORT }; /*!< BUTTON's port array */

IRQn_Type BUTTON_IRQ[BUTTONn] =
{
BUTTON0_EXTI_IRQn, BUTTON1_EXTI_IRQn, BUTTON2_EXTI_IRQn,
BUTTON3_EXTI_IRQn, BUTTON4_EXTI_IRQn, BUTTON5_EXTI_IRQn,
BUTTON6_EXTI_IRQn, BUTTON7_EXTI_IRQn, BUTTON8_EXTI_IRQn }; /*!< BUTTON's IRQ array */

/* Private functions declaration ---------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/**
 * @brief  This function handles external line 0 interrupt request.
 * @retval None
 */
void EXTI0_IRQHandler(void)
{
	/* EXTI line interrupt detected */
	if (RESET != __HAL_GPIO_EXTI_GET_IT(BUTTON0_PIN))
	{
		__HAL_GPIO_EXTI_CLEAR_IT(BUTTON0_PIN);
		if (0 != g_pButtonCallbackArray[0].action)
		{
			g_pButtonCallbackArray[0].action();
		}
	}
}

/**
 * @brief  This function handles external line 1 interrupt request.
 * @retval None
 */
void EXTI1_IRQHandler(void)
{
	/* EXTI line interrupt detected */
	if (RESET != __HAL_GPIO_EXTI_GET_IT(BUTTON1_PIN))
	{
		__HAL_GPIO_EXTI_CLEAR_IT(BUTTON1_PIN);
		if (0 != g_pButtonCallbackArray[1].action)
		{
			g_pButtonCallbackArray[1].action();
		}
	}
}

/**
 * @brief  This function handles external line 3 interrupt request.
 * @param  None
 * @retval None
 */
void EXTI3_IRQHandler(void)
{
	/* EXTI line interrupt detected */
	if (RESET != __HAL_GPIO_EXTI_GET_IT(BUTTON2_PIN))
	{
		__HAL_GPIO_EXTI_CLEAR_IT(BUTTON2_PIN);
		if (0 != g_pButtonCallbackArray[2].action)
		{
			g_pButtonCallbackArray[2].action();
		}
	}
}

/**
 * @brief  This function handles external line 4 interrupt request.
 * @param  None
 * @retval None
 */
void EXTI4_IRQHandler(void)
{
	/* EXTI line interrupt detected */
	if (RESET != __HAL_GPIO_EXTI_GET_IT(BUTTON3_PIN))
	{
		__HAL_GPIO_EXTI_CLEAR_IT(BUTTON3_PIN);
		if (0 != g_pButtonCallbackArray[3].action)
		{
			g_pButtonCallbackArray[3].action();
		}
	}
}

/**
 * @brief  This function handles external line 5-9 interrupt request.
 * @param  None
 * @retval None
 */
void EXTI9_5_IRQHandler(void)
{
	/* EXTI line interrupt detected */
	if (RESET != __HAL_GPIO_EXTI_GET_IT(BUTTON4_PIN))
	{
		__HAL_GPIO_EXTI_CLEAR_IT(BUTTON4_PIN);
		if (0 != g_pButtonCallbackArray[4].action)
		{
			g_pButtonCallbackArray[4].action();
		}
	}
	else if (RESET != __HAL_GPIO_EXTI_GET_IT(BUTTON5_PIN))
	{
		__HAL_GPIO_EXTI_CLEAR_IT(BUTTON5_PIN);
		if (0 != g_pButtonCallbackArray[5].action)
		{
			g_pButtonCallbackArray[5].action();
		}
	}
}

/**
 * @brief  This function handles external line 10_15 interrupt request.
 * @param  None
 * @retval None
 */
void EXTI15_10_IRQHandler(void)
{
	/* EXTI line interrupt detected */
	if (RESET != __HAL_GPIO_EXTI_GET_IT(BUTTON6_PIN))
	{
		__HAL_GPIO_EXTI_CLEAR_IT(BUTTON6_PIN);
		if (0 != g_pButtonCallbackArray[6].action)
		{
			g_pButtonCallbackArray[6].action();
		}
	}
	else if (RESET != __HAL_GPIO_EXTI_GET_IT(BUTTON7_PIN))
	{
		__HAL_GPIO_EXTI_CLEAR_IT(BUTTON7_PIN);
		if (0 != g_pButtonCallbackArray[7].action)
		{
			g_pButtonCallbackArray[7].action();
		}
	}
	else if (RESET != __HAL_GPIO_EXTI_GET_IT(BUTTON8_PIN))
	{
		__HAL_GPIO_EXTI_CLEAR_IT(BUTTON8_PIN);
		if (0 != g_pButtonCallbackArray[8].action)
		{
			g_pButtonCallbackArray[8].action();
		}
	}
}

/* Exported functions prototype ----------------------------------------------*/
/**
 * @brief  Configures EXTI button lines in interrupt mode
 * @param  pbuttonCallbackArray: list of button callback function pointers.
 * @retval bool: Initializing status
 *			@arg true: succeeded
 *			@arg false: failed
 */
bool button_configIRQHandler(button_callback_t *pbuttonCallbackArray)
{
	/* Note: Enable GPIOA and GPIOB clock for short.
	 __HAL_RCC_GPIOA_CLK_ENABLE();
	 __HAL_RCC_GPIOB_CLK_ENABLE();
	 For portability, the macro should be used:
	 */
	int32_t i;
	for (i = 0; i < BUTTONn; i++)
	{
		BUTTONx_GPIO_CLK_ENABLE(i);
	}

	/* Configure pin as input floating */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	for (i = 0; i < BUTTONn; i++)
	{
		GPIO_InitStructure.Pin = BUTTON_PIN[i];
		HAL_GPIO_Init(BUTTON_PORT[i], &GPIO_InitStructure);

		/* Clear pending flag for safe */
		__HAL_GPIO_EXTI_CLEAR_IT(BUTTON_PIN[i]);

		/* Enable and set EXTI lines Interrupt to the lowest priority */
		HAL_NVIC_SetPriority(BUTTON_IRQ[i], BUTTONx_EXTI_IRQ_PRIORITY,
				0 /* UNUSED */);
		HAL_NVIC_EnableIRQ(BUTTON_IRQ[i]);

		/* Mapping callback function */
		if (0 != (pbuttonCallbackArray + i)->action)
		{
			g_pButtonCallbackArray[i].action =
					(pbuttonCallbackArray + i)->action;
		}
		else
		{
			return false;
		}
	}

	/* NOTE: The following setting MUST be done AFTER the Pin configuration above,
	 Otherwise, the PB3 will NOT behave like GPIO in input mode. */
	CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_TRACE_IOEN); /* For safe, this must be default setting after reset */
	__HAL_AFIO_REMAP_SWJ_NOJTAG(); /* Release the PB3 pin */
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_3); /* Clear the rising pending immediately after the PB3 pin release */

	return true;
}

/**@}BSP_DEVICE_BUTTON_PRIVATE*/
/**@}BSP_DEVICE_BUTTON*/
/********************** (TM) PnL - Programming and Leverage ****END OF FILE****/
