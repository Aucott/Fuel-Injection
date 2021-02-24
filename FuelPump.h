/*
 * FuelPump.h
 *
 *  Created on: 1 Sep. 2019
 *      Author: apaml
 */

#ifndef FUELPUMP_H_
#define FUELPUMP_H_

#define FUELPUMPOFF 0
#define FUELPUMPENABLED 1
#define FUELPUMPINVERTED 2
#define FUELPUMPALWAYSON 3

#include "Arduino.h"
#include "ICosworth.h"

class _FuelPump
{
public:
	_FuelPump();
	//----------------------------------------------------------------------------------------
	void processCommand(const rxData Rxdata)
	{
		uint16_t subCommand = Rxdata.subCommand;
		uint16_t dataCount = Rxdata.dataX;
		uint16_t Crc = Rxdata.dataY;
		uint16_t crc;

		switch (subCommand)
		{
		case 0XA1:
		{
			uint16_t buf[WORD_PACKET_SIZE];
			memset(buf, 0, sizeof(buf));
			HWSERIAL.print("Uploading FPump Settings ->");
			buf[1] = getMode();
			buf[2] = getPrimeTime();
		
			buf[0] = CRC16.ccitt(
				reinterpret_cast<uint8_t *>(&(buf[1])),
				(WORD_PACKET_SIZE * sizeof(uint16_t)) - sizeof(uint16_t));

			int n = hid_sendWithAck(reinterpret_cast<uint8_t *>(buf));

			if (n)
			{
				HWSERIAL.println("Error Uploading FPump Settings");
			}

			HWSERIAL.println("OK");

			break;
		}
		case 0XA2:
			HWSERIAL.println("Downloading FPump Settings");
			crc =
				CRC16.ccitt(
					reinterpret_cast<uint8_t *>((uint16_t *)(&(Rxdata.dataArray[0]))),
					dataCount * sizeof(uint16_t));
			if (Crc == crc)
			{
				setMode(Rxdata.dataArray[0]);
				setPrimeTime(Rxdata.dataArray[1]);

				FPumpSettings.Crc = Crc;
				Save();
			}
			else
			{
				HWSERIAL.print("FPump settings load error ");
				Errors.setError(ERR_SETTINGS_LOAD_CRC);
			}
			break;
		}
	}
	//----------------------------------------------------------------------------------------

	void reset_runtime(void);
	bool isRunning(void);
	void checkRunningTime(void);
	uint16_t getMode(void);
	uint16_t getPrimeTime(void);
	int16_t Load(void);

private:
	void setPrimeTime(uint16_t);
	void setMode(uint16_t);
	int16_t Save(void);
	bool crcCheckSettings(void);
	char _fName[6] = "sFPmp";
	spiffs_file _filePtr;

	struct t_SETTINGS
	{
		uint16_t FuelPump_mode;
		uint16_t FuelPump_primeTime;
		uint16_t Crc;
	};

	t_SETTINGS FPumpSettings;

	bool _pumpOn;

	uint16_t _sampleTime;
	elapsedMillis _clock;
	elapsedMillis _pumpRunTime;
};

extern _FuelPump FuelPump;

#endif /* FUELPUMP_H_ */
