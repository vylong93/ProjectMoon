/**
 ****************************************************************************
 * @file        audio.c
 * @author      Long Dang
 * @version     V0.1
 * @date        16-April-2017
 * @copyright   LGPLv3
 * @brief       This file implement the simple audio library.
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
/** @addtogroup LIB_AUDIO
 * @{
 */
/** @defgroup LIB_AUDIO_PRIVATE Audio (Private)
 * @{
 */
/* Includes ------------------------------------------------------------------*/
#include "audio.h"
#include "ff.h"
#include "text.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define REPORT_ON_SCREEN
#define AUDIO_BUFFER_SIZE	(32) /*!< Number of byte in the local audio buffer */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t g_pui8AudioBuffer[AUDIO_BUFFER_SIZE];
static bool g_bNeedData = true;

/* Private functions declaration ---------------------------------------------*/
static bool audio_readSongData(FIL *pFile, uint8_t *pui8DestBuffer,
		uint32_t ui32ReadSize);

/* Private function prototypes -----------------------------------------------*/
/**
 * @brief  Read a block data of the file to RAM.
 * @param  pFile: File pointer to current opened song.
 * @param  pui8DestBuffer: Destination buffer data pointer.
 * @param  ui32ReadSize: Size of the data block need to read.
 * @retval bool: process status
 *			@arg true: read completed
 *			@arg false: failed to read - EOF
 */
static bool audio_readSongData(FIL *pFile, uint8_t *pui8DestBuffer,
		uint32_t ui32ReadSize)
{
	FRESULT res;
	uint32_t ui32BytesRead = 0;
	res = f_read(pFile, pui8DestBuffer, ui32ReadSize, (UINT*) &ui32BytesRead);
	if ((0 == ui32BytesRead) || (FR_OK != res))
	{
		return false;
	}
	return true;
}

/* Exported functions prototype ----------------------------------------------*/
/**
 * @brief  Play the audio file in file system. Current supported *.mp3 and *.wav.
 * @param  pcFileName: string of the audio file to play.
 * @retval bool: process status
 *			@arg true: play file completed
 *			@arg false: failed to play
 */
bool audio_playFileBlocking(const char *pcFileName)
{
#ifdef REPORT_ON_SCREEN
#define REPORT_INTERVAL 		(4096)
#define REPORT_INTERVAL_MIDI	(512)
	const char *psNameTable[] =
	{ "unknown", "RIFF", "MP3", "MIDI", };
	uint32_t ui32CurrentPos = 0;
	uint32_t ui32NextReportPos = 0;
	uint16_t ui16DecodedTime = 0;
	uint16_t ui16SampleRate = 0;
	bool bIsStereoMode = false;
	audio_format_t audioFormat = afUnknown;
#endif

	graphic_clearRenderBuffer();
	text_setCursor(0, 0);
	text_setWrapText(false);
	text_putLine(pcFileName, FAST);

	FIL file;
	if (f_open(&file, pcFileName, FA_READ) != FR_OK)
	{
		return false;
	}

	acodec_initPlaying();
	while (true)
	{
		while (acodec_isDeviceBusy())
		{
			/* DREQ is low while the receive buffer is full.
			 You can do something else here, the buffer of the MP3 is full and happy.
			 Maybe set the volume or test to see how much we can delay before we hear audible glitches */

			/* If the MP3 IC is happy, but we need to read new data from the file system,
			 now is a great time to do so */
			if (true == g_bNeedData)
			{
				if (!audio_readSongData(&file, g_pui8AudioBuffer,
				AUDIO_BUFFER_SIZE))
				{
					/* Time to exit ! There is no data left to read! */
					text_putLine("EOF", FAST);
					break;
				}
				g_bNeedData = false;
			}

			/* Test to see just how much we can do before the audio starts to glitch */
//			bsp_acodec_delay_ms(150); /* audible glitches */
//			bsp_acodec_delay_ms(135); /* audible glitches */
//			bsp_acodec_delay_ms(120); /* barely audible glitches */
//			bsp_acodec_delay_ms(100); /* sound fine ~ 128kbps */
//			bsp_acodec_delay_ms(20); /* sound fine ~ 320kbps */
#ifdef REPORT_ON_SCREEN
			ui32CurrentPos += AUDIO_BUFFER_SIZE;
			if (ui32CurrentPos >= ui32NextReportPos)
			{
				/* Update report on screen
				 2nd line: Audio format
				 3rd line: sample rate - mono/stereo
				 4th line: decode time
				 */
				acodec_getFormat(&audioFormat);
				acodec_getSamplerate(&ui16SampleRate, &bIsStereoMode);
				acodec_getDecodingTime(&ui16DecodedTime);

				text_setCursor(0, 8);
				text_printString("Format: ");
				text_printString(psNameTable[audioFormat]);
				text_printString("                   \n");

				text_printNumber(ui16SampleRate);
				text_printString("Hz - ");
				text_printString((bIsStereoMode) ? "stereo\n" : "mono\n");

				int ss = ui16DecodedTime % 60;
				int mm = ui16DecodedTime / 60;
				text_printNumber(mm / 10);
				text_printNumber(mm % 10);
				text_printString(":");
				text_printNumber(ss / 10);
				text_printNumber(ss % 10);
				text_printString(" ");

				text_printNumber(ui32CurrentPos / 1024);
				text_printString("KiB");
				text_printString("             \n");

				graphic_render();
				ui32NextReportPos +=
						(audioFormat == afMidi || audioFormat == afUnknown) ?
								REPORT_INTERVAL_MIDI : REPORT_INTERVAL;
			}
#endif
		}

		/* This is here in case we haven't had any free time to load new data */
		if (true == g_bNeedData)
		{
			/* Goto current file system and try reading 32 new bytes of the song */
			if (!audio_readSongData(&file, g_pui8AudioBuffer,
			AUDIO_BUFFER_SIZE))
			{
				/* Time to exit ! There is no data left to read! */
				text_putString("EOF-", FAST);
				break;
			}
			g_bNeedData = false;
		}

		/* Once DREQ is released (high) we now feed 32 bytes of data to the VS1003 from our SD read buffer */
		acodec_sendData(g_pui8AudioBuffer, AUDIO_BUFFER_SIZE);

		/* We've just dumped 32 bytes into VS1003 so our SD read buffer is empty.
		 Set flag so we go get more data */
		g_bNeedData = true;
	}

	return true;
}

/**@}LIB_AUDIO_PRIVATE*/
/**@}LIB_AUDIO*/
/********************** (TM) PnL - Programming and Leverage ****END OF FILE****/
