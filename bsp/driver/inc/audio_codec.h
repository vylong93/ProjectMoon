/**
 ****************************************************************************
 * @file        audio_codec.h
 * @author      Long Dang
 * @version     V0.1
 * @date        14-April-2017
 * @copyright   LGPLv3
 * @brief       This is the header of the VS1003 device driver.
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
#ifndef AUDIO_CODEC_H_
#define AUDIO_CODEC_H_

/** @addtogroup BSP_DRV_ACODEC
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "audio_codec_io.h"

/* Exported types ------------------------------------------------------------*/
/**
 * @typedef audio_format_t
 * This type define the audio format supported by the VS1003 device.
 */
typedef enum
{
	afUnknown = 0, /*!< Unknown audio format */
	afRiff, /*!< RIFF audio format */
	afMp3, /*!< MP3 audio format */
	afMidi, /*!< MIDI audio format */
} audio_format_t;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define	acodec_delay_ms(x)		bsp_acodec_delay_ms(x) /*!< Wrapper Audio CODEC IO API */
#define acodec_isDeviceBusy()	bsp_acodec_isDeviceBusy() /*!< API wrapper: check if current device is busy or not */
#define acodec_sendData(pui8Buffer, ui32Size)	bsp_acodec_sendData(pui8Buffer, ui32Size) /*!< API wrapper: send a bunk of data to device */

/* Exported functions --------------------------------------------------------*/
bool acodec_init(void);
void acodec_reset(void);
void acodec_shutdown(void);
void acodec_setVolume(uint8_t ui8Volume);
void acodec_setBassEnhancement(bool bEnable);
void acodec_setTrebleControl(bool bEnable);

void acodec_initPlaying(void);
void acodec_endFilePadding(void);
void acodec_getFormat(audio_format_t *pAudioFormat);
void acodec_getSamplerate(uint16_t *pui16SampleRate, bool *pbStereo);
void acodec_getDecodingTime(uint16_t *pui16DecodingTimeInSecond);

void acodec_initRecordAPCM(bool bFastSampleRate);
void acodec_deInitRecordAPCM(void);
void acodec_readRecordBlock(uint8_t *pui8OutputBuffer);

/**@}BSP_DRV_ACODEC*/
#endif /* AUDIO_CODEC_H_ */

/********************** (TM) PnL - Programming and Leverage ****END OF FILE****/
