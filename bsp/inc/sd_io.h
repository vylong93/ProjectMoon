/**
 ****************************************************************************
 * @file        sd_io.h
 * @author      Long Dang
 * @version     V0.1
 * @date        25-March-2017
 * @copyright   LGPLv3
 * @brief       This is the header of the SD I/O controler.
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
#ifndef SDIO_H_
#define SDIO_H_

/** @defgroup BSP Board Support Package
 * @{
 */
/** @defgroup BSP_SDIO SD I/O
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_bsp.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
bool bsp_sd_init(void);
bool bsp_sd_isDetected(void);
bool bsp_sd_sendData(uint8_t * pui8Buffer, uint16_t ui16Size);
bool bsp_sd_readData(uint8_t * pui8Buffer, uint16_t ui16Size);
bool bsp_sd_sendCommand(uint8_t ui8Cmd, uint32_t ui32Arg, uint8_t ui8CRC,
		uint8_t ui8ExpectedResponse);
bool bsp_sd_sendSpecialCommand(uint8_t ui8Cmd, uint32_t ui32Arg,
		uint8_t ui8CRC, uint8_t ui8ExpectedResponse,
		uint32_t *pui32TrailingResponse);
bool bsp_sd_waitResponse(const uint8_t ui8ExpectedResponse);
void bsp_sd_sendDummy(void);

/**
 * @} BSP_SDIO
 */
/**
 * @} LIB
 */
#endif /* SDIO_H_ */

/********************** (TM) PnL - Programming and Leverage ****END OF FILE****/
