/*
 * ThermalFan.h
 *
 *  Created on: 1 Sep. 2019
 *      Author: apaml
 */

#ifndef THERMALFAN_H_
#define THERMALFAN_H_

#include "Arduino.h"
#include "ICosworth.h"
#include "Settings.h"

class _ThermalFan
{

public:
	_ThermalFan();
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
			HWSERIAL.print("Uploading TFan Settings ->");
			buf[1] = getOnTempertaure();
			buf[2] = getHystheresis();
			buf[3] = getAdditionIdleEffort();
			buf[4] = getAdditionIdleRpm();
			buf[5] = getIdleUpFanOnDelay();

			buf[0] = CRC16.ccitt(
				reinterpret_cast<uint8_t *>(&(buf[1])),
				(WORD_PACKET_SIZE * sizeof(uint16_t)) - sizeof(uint16_t));

			int n = hid_sendWithAck(reinterpret_cast<uint8_t *>(buf));

			if (n)
			{
				HWSERIAL.println("Error Uploading TFan Settings");
			}
			HWSERIAL.println("Ok");

			break;
		}

		case 0XA2:

			HWSERIAL.println("Downloading TFan Settings");
			crc =
				CRC16.ccitt(
					reinterpret_cast<uint8_t *>((uint16_t *)(&(Rxdata.dataArray[0]))),
					dataCount * sizeof(uint16_t));
			if (Crc == crc)
			{
				setOnTempertaure(Rxdata.dataArray[0]);
				setHystheresis(Rxdata.dataArray[1]);
				setAdditionIdleEffort(Rxdata.dataArray[2]);
				setAdditionIdleRpm(Rxdata.dataArray[3]);
				setIdleUpFanOnDelay(Rxdata.dataArray[4]);
				TFanSettings.Crc = Crc;
				Save();
			}
			else
			{
				HWSERIAL.print("TFan settings load error ");
				Errors.setError(ERR_SETTINGS_LOAD_CRC);
			}
			break;
		}
	}
	//----------------------------------------------------------------------------------------
	uint16_t isRunning(void);
	void check_Temperature(float);
	int16_t getOnTempertaure(void);
	uint16_t getHystheresis(void);
	uint16_t getIdleUpFanOnDelay(void);
	uint16_t getAddedIdleRpm(void);
	uint16_t getAddedIdleEffort(void);
	uint16_t getAdditionIdleEffort(void);
	uint16_t getAdditionIdleRpm(void);
	int16_t Load(void);

private:
	void setOnTempertaure(uint16_t);
	void setHystheresis(uint16_t);
	void setIdleUpFanOnDelay(uint16_t);
	void setAdditionIdleEffort(uint16_t);
	void setAdditionIdleRpm(uint16_t);
	bool crcCheckSettings(void);
	int16_t Save(void);
	char _fName[6] = "sTFan";
	spiffs_file _filePtr;

	struct t_SETTINGS
	{

		int16_t ThermalFan_OnTemperature;
		uint16_t ThermalFan_Hystheresis;
		uint16_t ThermalFan_AdditionIdleEffort;
		uint16_t ThermalFan_AdditionIdleRpm;
		uint16_t ThermalFan_IdleUpFanDelay;
		uint16_t Crc;
	};
	t_SETTINGS TFanSettings;

	bool _fanRunning;

	uint16_t _sampleTime;
	elapsedMillis _fanRunTime;
	elapsedMillis _clock;
};

extern _ThermalFan ThermalFan;

#endif /* THERMALFAN_H_ */
