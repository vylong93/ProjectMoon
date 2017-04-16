/**
 ****************************************************************************
 * @file        audio.h
 * @author      Long Dang
 * @version     V0.1
 * @date        16-April-2017
 * @copyright   LGPLv3
 * @brief       This is the header of the simple audio library.
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
#ifndef AUDIO_H_
#define AUDIO_H_

/** @addtogroup LIB_AUDIO
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "audio_codec.h"

/* Exported types ------------------------------------------------------------*/
/**
 * @typedef record_rate_t
 * This type define the target sample rate for recording.
 */
typedef enum
{
	REC_8KHz = 0, /*!< Sample Rate, 0x3e80 = 16.0kHz, Average Bytes Per Second, 0x1fae for 16.0kHz */
	REC_16KHz = 1, /*!< Sample Rate, 0x1f40 = 8.0kHz, Average Bytes Per Second, 0x0fd7 for 8.0kHz */
} record_rate_t;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
bool audio_init(void);
bool audio_playFileBlocking(const char *pcFileName);
bool audio_recordFileBlocking(const char *pcFileName, uint32_t ui32PeriodSecond,
		record_rate_t recordRate);

/**@}LIB_AUDIO*/
#endif /* AUDIO_H_ */

/********************** (TM) PnL - Programming and Leverage ****END OF FILE****/
