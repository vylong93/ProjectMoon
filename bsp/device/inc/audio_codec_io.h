/**
 ****************************************************************************
 * @file        audio_codec_io.h
 * @author      Long Dang
 * @version     V0.1
 * @date        02-April-2017
 * @copyright   LGPLv3
 * @brief       This is the header of the VS1003 device IO controller.
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef AUDIO_CODEC_IO_H_
#define AUDIO_CODEC_IO_H_

/** @addtogroup BSP_DEVICE_ACODEC
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_bsp.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define bsp_acodec_delay_ms(x) bsp_delay_ms(x) /*!< Wrapper BSP API */

/* Exported functions --------------------------------------------------------*/
bool bsp_acodec_init(void);
void bsp_acodec_reset(void);
bool bsp_acodec_writeRegsiter(uint8_t ui8Address, uint16_t ui16Value);
bool bsp_acodec_readRegsiter(uint8_t ui8Address, uint16_t *pui16Value);
bool bsp_acodec_sendData(const uint8_t *pui8Buffer, uint32_t ui32Size);

/**@}BSP_DEVICE_ACODEC*/
#endif /* AUDIO_CODEC_IO_H_ */

/********************** (TM) PnL - Programming and Leverage ****END OF FILE****/
