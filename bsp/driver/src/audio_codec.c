/**
 ****************************************************************************
 * @file        audio_codec.c
 * @author      Long Dang
 * @version     V0.1
 * @date        14-April-2017
 * @copyright   LGPLv3
 * @brief       This file implement the VS1003 device driver.
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
/** @addtogroup BSP_DRV_ACODEC
 * @{
 */
/** @defgroup BBSP_DRV_ACODEC_PRIVATE Audio CODEC (Private)
 * @{
 */
/* Includes ------------------------------------------------------------------*/
#include "audio_codec.h"
#include "vs10xx_uc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define VOLUME_ANALOG_SHUTDOWN	(0xFFFF)
#define VOLUME_MINUS_3DB		(0x0606)
#define VOLUME_MINUS_6DB		(0x0C0C)
#define VOLUME_MINUS_18DB		(0x2424)
#define VOLUME_SILENCE			(0xFEFE)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint16_t g_ui16PreviousPlayingVolume = 0x0C0C;
static uint16_t g_ui16PreviousBassTrebleSetting = 0x0000;

/* Private functions declaration ---------------------------------------------*/
static void acodec_setDecodingTime(uint16_t ui16TimeInSecond);

/* Private function prototypes -----------------------------------------------*/
/**
 * @brief  Update current decoding time in the VS1003 device.
 * @param  ui16TimeInSecond: Time in second unit.
 * @note   Need to write twice to change the value.
 * @retval None
 */
static void acodec_setDecodingTime(uint16_t ui16TimeInSecond)
{
	bsp_acodec_writeRegsiter(SCI_DECODE_TIME, ui16TimeInSecond);
	bsp_acodec_writeRegsiter(SCI_DECODE_TIME, ui16TimeInSecond);
}

/* Exported functions prototype ----------------------------------------------*/
/**
 * @brief  Initialize the audio CODEC driver to control the VS1003 device.
 * @retval bool: Initializing status
 *			@arg true: succeeded
 *			@arg false: failed
 */
bool acodec_init(void)
{
	if (!bsp_acodec_init())
	{
		return false;
	}

	bsp_acodec_reset();

	/* De-click: Immediately switch analog off */
	bsp_acodec_writeRegsiter(SCI_VOL, VOLUME_ANALOG_SHUTDOWN);
	bsp_acodec_delay_ms(100);
	/* Switch on the analog parts - volume = 0db */
	bsp_acodec_writeRegsiter(SCI_VOL, VOLUME_SILENCE);

	/* Sanity Check */
	{
		uint16_t ui16ReadValue = 0xFFFF;

		/* Start initialization with a dummy read, which makes sure our
		 MCU chips selects and everything are where they
		 are supposed to be and that VS10xx's SCI bus is in a known state. */
		bsp_acodec_readRegsiter(SCI_MODE, &ui16ReadValue);

		/* First real operation is a software reset. After the software
		 reset we know what the status of the IC is. You need, depending
		 on your application, either set or not set SM_SDISHARE. See the
		 Data-sheet for details. */
		bsp_acodec_writeRegsiter(SCI_MODE, SM_SDINEW | SM_SDISHARE | SM_RESET);

		/* A quick sanity check: write to two registers, then test if we
		 get the same results. Note that if you use a too high SPI
		 speed, the MSB is the most likely to fail when read again. */
		bsp_acodec_writeRegsiter(SCI_AICTRL1, 0xABAD);
		bsp_acodec_writeRegsiter(SCI_AICTRL2, 0x7E57);
		bsp_acodec_readRegsiter(SCI_AICTRL1, &ui16ReadValue);
		if (0xABAD != ui16ReadValue)
		{
			/* There is something wrong with VS10xx SCI registers */
			return false;
		}
		bsp_acodec_readRegsiter(SCI_AICTRL2, &ui16ReadValue);
		if (0x7E57 != ui16ReadValue)
		{
			/* There is something wrong with VS10xx SCI registers */
			return false;
		}
		bsp_acodec_writeRegsiter(SCI_AICTRL1, 0);
		bsp_acodec_writeRegsiter(SCI_AICTRL2, 0);

		/* Check VS10xx type */
		/* Note: code SS_VER=2 is used for both VS1002 and VS1011e */
		const uint16_t chipNumber[16] =
				{ 1001, 1011, 1011, 1003, 1053, 1033, 1063, 1103, 0, 0, 0, 0, 0,
						0, 0, 0 };

		uint16_t ssVer;
		bsp_acodec_readRegsiter(SCI_STATUS, &ui16ReadValue);
		ssVer = ((ui16ReadValue >> 4) & 15);
		if (chipNumber[ssVer])
		{
			if (chipNumber[ssVer] != 1003)
			{
				/* Incorrect chip */
				return false;
			}
		}
		else
		{
			/* Unknown VS10xx SCI_MODE field SS_VES */
			return false;
		}

		/* Set the clock. Until this point we need to run SPI slow so that
		 we do not exceed the maximum speeds mentioned in
		 Chapter SPI Timing Diagram in the Data-sheet. */

		/* Now when we have increase the VS10xx clock speed, the MCU
		 SPI bus can run faster. Do that before you start playing or
		 recording files. */

		/* Experimenting with higher clock settings: 12.288MHz x 3.0 = 36.864MHz */
		bsp_acodec_writeRegsiter(SCI_CLOCKF, SC_MULT_03_30X);
	}
	return true;
}

/**
 * @brief  Generate software reset command to the VS1003 device.
 * @retval None
 */
void acodec_reset(void)
{
	/* It's recommended to feed 2048 zeros (honoring DREQ) to the SDI bus
	 after the file and before the reset */
	bsp_acodec_sendDataRepeatedly(0x00, 2048);

	/* Trigger software reset */
	uint16_t ui16ReadValue;
	bsp_acodec_readRegsiter(SCI_MODE, &ui16ReadValue);
	bsp_acodec_writeRegsiter(SCI_MODE, ui16ReadValue | SM_RESET);

	/* Experimenting with higher clock settings: 12.288MHz x 3.0 = 36.864MHz */
	bsp_acodec_writeRegsiter(SCI_CLOCKF, SC_MULT_03_30X);

	/* Set volume level at -18 dB */
	bsp_acodec_writeRegsiter(SCI_VOL, VOLUME_MINUS_18DB);

	/* Reset decode time */
	acodec_setDecodingTime(0);
}

/**
 * @brief  Shutdown the chip without audio glitches.
 * @retval None
 */
void acodec_shutdown(void)
{
	uint16_t ui16ReadValue;
	bsp_acodec_readRegsiter(SCI_STATUS, &ui16ReadValue);
	bsp_acodec_writeRegsiter(SCI_STATUS, ui16ReadValue | SS_APDOWN2);

	/* Need to delay few milliseconds before activating reset */
	bsp_acodec_delay_ms(10);
}

/**
 * @brief  Setting the left and right channel volume.
 * @param  ui8Volume: Target volume value.
 * 			@arg Maximum volume is 0x00.
 * 			@arg Total silence is 0xFE.
 * @note   For each channel, a value in the range of 0..254
 * 		   may be defined to set its attenuation from
 * 		   the maximum volume level (in 0.5 dB steps).
 * @retval None
 */
void acodec_setVolume(uint8_t ui8Volume)
{
	uint16_t ui16Volume = ui8Volume;
	ui16Volume <<= 8;
	ui16Volume |= ui8Volume;

	bsp_acodec_writeRegsiter(SCI_VOL, ui16Volume);
}

/**
 * @brief  Setting the Bass Enhancement of the VS1003 device.
 * @param  bEnable:
 * 			@arg true: 15dB bass enhancement below 60Hz
 * 			@arg false: disable Bass enhancement
 * @note   SB_AMPLITUDE should be set to the user’s preferences,
 * 		   and SB_FREQLIMIT to roughly 1.5 times the lowest frequency
 * 		   the user’s audio system can reproduce.
 * @retval None
 */
void acodec_setBassEnhancement(bool bEnable)
{
	uint16_t ui16WriteValue;
	bsp_acodec_readRegsiter(SCI_BASS, &ui16WriteValue);

	ui16WriteValue &= 0xFF00; /* Disable Bass */
	if (bEnable)
	{
		/* 15dB bass enhancement below 60Hz */
		ui16WriteValue |= 0x00F6;
	}

	bsp_acodec_writeRegsiter(SCI_BASS, ui16WriteValue);
}

/**
 * @brief  Setting the Treble Control of the VS1003 device.
 * @param  bEnable:
 * 			@arg true: 10.5dB treble enhancement at and above 10kHz
 * 			@arg false: disable Treble Control
 * @retval None
 */
void acodec_setTrebleControl(bool bEnable)
{
	uint16_t ui16WriteValue;
	bsp_acodec_readRegsiter(SCI_BASS, &ui16WriteValue);

	ui16WriteValue &= 0x00FF; /* Disable Treble */
	if (bEnable)
	{
		/* 10.5dB treble enhancement at and above 10kHz */
		ui16WriteValue |= 0x7A00;
	}

	bsp_acodec_writeRegsiter(SCI_BASS, ui16WriteValue);
}

/**
 * @brief  Send at least 4 zeros between each stream data - 10 for safe and reset decoding time.
 * @retval None
 */
void acodec_initPlaying(void)
{
	bsp_acodec_sendDataRepeatedly(0x00, 10);
	acodec_setDecodingTime(0);
}

/**
 * @brief  Padding 2048 zeros bytes for EOF. It's recommended for VS1003 device.
 * @retval None
 */
void acodec_endFilePadding(void)
{
	bsp_acodec_sendDataRepeatedly(0x00, 2048);
}

/**
 * @brief  Get the current processing audio format in the VS1003 device.
 * @param  pAudioFormat: Data pointer to audio format:
 * 			@arg afRiff: RIFF
 * 			@arg afMp3: MP3
 * 			@arg afMidi: MIDI
 * 			@arg afUnknown: Unknown
 * @retval None
 */
void acodec_getFormat(audio_format_t *pAudioFormat)
{
	uint16_t ui16ReadValue;
	bsp_acodec_readRegsiter(SCI_HDAT1, &ui16ReadValue);

	if (0x7665 == ui16ReadValue)
	{
		*pAudioFormat = afRiff;
	}
	else if (0x4d54 == ui16ReadValue)
	{
		*pAudioFormat = afMidi;
	}
	else if (0xffe2 == (ui16ReadValue & 0xffe6))
	{
		*pAudioFormat = afMp3;
	}
	else
	{
		*pAudioFormat = afUnknown;
	}
}

/**
 * @brief  Get the current processing audio sample rate and stereo/mono mode in the VS1003 device.
 * @param  pui16SampleRate: Data pointer to sample rate in Hz unit.
 * @param  pbStereo: Data pointer to Stereo/Mono flag
 * 			@arg true: Stereo
 * 			@arg false: Mono
 * @retval None
 */
void acodec_getSamplerate(uint16_t *pui16SampleRate, bool *pbStereo)
{
	/* Get sample rate and Stereo/Mono flag */
	bsp_acodec_readRegsiter(SCI_AUDATA, pui16SampleRate);
	*pbStereo = (*pui16SampleRate & 0x0001) ? (true) : (false);
	*pui16SampleRate &= 0xFFFE;
}

/**
 * @brief  Get the current decoding time in the VS1003 device.
 * @param  pui16DecodingTimeInSecond: Data pointer to decoding time in second unit.
 * @retval None
 */
void acodec_getDecodingTime(uint16_t *pui16DecodingTimeInSecond)
{
	bsp_acodec_readRegsiter(SCI_DECODE_TIME, pui16DecodingTimeInSecond);
}

/**
 * @brief  Activate ADPCM recording mode and save the current playing volume and bass-treble configuration.
 * @retval None
 */
void acodec_initRecordAPCM(bool bFastSampleRate)
{
	uint16_t ui16ReadValue;

	/* Save the current playing setup */
	bsp_acodec_readRegsiter(SCI_VOL, &g_ui16PreviousPlayingVolume);
	bsp_acodec_readRegsiter(SCI_BASS, &g_ui16PreviousPlayingVolume);

	/* Recording monitor volume - silence to disable echo */
	bsp_acodec_writeRegsiter(SCI_VOL, VOLUME_SILENCE);

	/* Disable Treble, Bass Enhancement and Lower limit frequency */
	bsp_acodec_writeRegsiter(SCI_BASS, 0x0000);
	bsp_acodec_delay_ms(10);

	/* Experimenting with higher clock settings: 12.288MHz x 3.0 = 36.864MHz */
	bsp_acodec_writeRegsiter(SCI_CLOCKF, SC_MULT_03_30X);

	if (bFastSampleRate)
	{
		/* Set sample rate 16kHz for VS1003: 36.864MHz / (256 * 9) */
		bsp_acodec_writeRegsiter(SCI_AICTRL0, 0x0009);
	}
	else
	{
		/* Set sample rate 8kHz for VS1003: 36.864MHz / (256 * 18) */
		bsp_acodec_writeRegsiter(SCI_AICTRL0, 0x0012);
	}

	/* AutoGain OFF, recored level x4 (1024-0x400=x1; 512-0x200=x0.5; 0-0x0000=AutoGain)
	 * Typical speed applications usually are better off using AGC, as this task care
	 * of relatively uniform speech loudness in recordings.
	 */
	bsp_acodec_writeRegsiter(SCI_AICTRL1, 0x1000);

	/* RECORD,NEWMODE,SHARESPI,RESET
	 * Enable SM_ADPCM_HP: (high-pass filter at 8kHz)
	 * 		to intelligibility of speech when there is lots of background noise.
	 * Otherwise, audio will be fuller and closer to original if SM_ADPCM_HP is not used.
	 */
	bsp_acodec_readRegsiter(SCI_MODE, &ui16ReadValue);
	bsp_acodec_writeRegsiter(SCI_MODE, ui16ReadValue | SM_ADPCM | SM_RESET);
	bsp_acodec_delay_ms(10);

	/* Experimenting with higher clock settings: 12.288MHz x 3.0 = 36.864MHz */
	bsp_acodec_writeRegsiter(SCI_CLOCKF, SC_MULT_03_30X);
}

/**
 * @brief  Sync to in coming audio frame, lots of data in buffer, wait until buffer level restarts from 0.
 * @retval None
 */
void acodec_syncToIncomingAudioFrame(void)
{
	uint16_t ui16ReadValue;
	do
	{
		bsp_acodec_readRegsiter(SCI_RECWORDS, &ui16ReadValue);
	}
	while (ui16ReadValue >> 8);
}

/**
 * @brief  De-activate ADPCM recording mode and recover last playing volume and bass-treble configuration.
 * @retval None
 */
void acodec_deInitRecordAPCM(void)
{
	uint16_t ui16ReadValue;

	/* Deactivate ADPCM recording */
	bsp_acodec_readRegsiter(SCI_MODE, &ui16ReadValue);
	bsp_acodec_writeRegsiter(SCI_MODE, ui16ReadValue & ~SM_ADPCM);

	/* Software reset */
	acodec_reset();

	/* Recover the last play back setup */
	bsp_acodec_writeRegsiter(SCI_VOL, g_ui16PreviousPlayingVolume);
	bsp_acodec_writeRegsiter(SCI_BASS, g_ui16PreviousBassTrebleSetting);
}

/**
 * @brief  Read record data in 256 bytes / block.
 * @param  pui8OutputBuffer: Output data storage pointer.
 * @retval None
 */
void acodec_readRecordBlock(uint8_t **ppui8OutputBuffer)
{
	uint8_t *pui8Buffer = *ppui8OutputBuffer;
	uint16_t ui16ReadValue;

	/* Wait until 256 bytes available or buffer overflow */
	do
	{
		bsp_acodec_readRegsiter(SCI_RECWORDS, &ui16ReadValue);
	} while ((ui16ReadValue < 128) || (ui16ReadValue >= 896));

	/* Read 256 bytes */
	int32_t i;
	for (i = 0; i < 128; i++)
	{
		bsp_acodec_readRegsiter(SCI_RECDATA, &ui16ReadValue);
		*pui8Buffer++ = ((ui16ReadValue >> 8) & 0xff);
		*pui8Buffer++ = (ui16ReadValue & 0xff);
	}
	*ppui8OutputBuffer = pui8Buffer;
}

/**@}BBSP_DRV_ACODEC_PRIVATE*/
/**@}BSP_DRV_ACODEC*/
/********************** (TM) PnL - Programming and Leverage ****END OF FILE****/
