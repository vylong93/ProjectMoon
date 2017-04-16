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
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
bool audio_init(void);
bool audio_playFileBlocking(const char *pcFileName);

/**@}LIB_AUDIO*/
#endif /* AUDIO_H_ */

/********************** (TM) PnL - Programming and Leverage ****END OF FILE****/
