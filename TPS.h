/*
 * TPS.h
 *
 *  Created on: 3 Jun. 2019
 *      Author: apaml
 */

#ifndef TPS_H_
#define TPS_H_

#include "Arduino.h"
#include "MapSensor.h"
#include <FastCRC.h>

#define ADCFILTER_TPS 128
#define DELTA_FILTER 20
class _TPS
{
public:
	_TPS();
	explicit _TPS(_Array3D *);

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
			HWSERIAL.print("Uploading TPS Settings");
			buf[1] = GetTpsClosedValue();
			buf[2] = GetTpsOpenValue();
			buf[3] = GetClosedThottleThreshold();
			buf[4] = GetOpenThottleThreshold();

			buf[0] = CRC16.ccitt(
				reinterpret_cast<uint8_t *>(&(buf[1])),
				(WORD_PACKET_SIZE * sizeof(uint16_t)) - sizeof(uint16_t));

			int n = hid_sendWithAck(reinterpret_cast<uint8_t *>(buf));

			if (n)
			{
				HWSERIAL.println("Error Uploading TPS Settings");
			}
			HWSERIAL.println("OK");
			break;
		}

		case 0XA2:
		{

			HWSERIAL.println("Downloading TPS Settings");
			crc =
				CRC16.ccitt(
					reinterpret_cast<uint8_t *>((uint16_t *)(&(Rxdata.dataArray[0]))),
					dataCount * sizeof(uint16_t));
			if (Crc == crc)
			{
				SetTpsClosedValue(Rxdata.dataArray[0]);
				SetTpsOpenValue(Rxdata.dataArray[1]);
				SetClosedThottleThreshold(Rxdata.dataArray[2]);
				SetOpenThottleThreshold(Rxdata.dataArray[3]);
				TPSSettings._Crc = Crc;
				Save();
			}
			else
			{
				Errors.setError(ERR_SETTINGS_LOAD_CRC);
				HWSERIAL.println("TPS Settings Crc error");
			}

			break;
		}

		case LEARN_CLOSED_THROTTLE:
			learnClosedThrottle();
			break;
		case LEARN_OPEN_THROTTLE:
			learnOpenThrottle();
			break;
		}
	}

	//----------------------------------------------------------------------------------------

	uint16_t
	GetTpsClosedValue(void);
	uint16_t GetTpsOpenValue(void);
	uint16_t GetClosedThottleThreshold(void);
	uint16_t GetOpenThottleThreshold(void);
	bool IsClosedThrottle(void);
	bool IsWOT(void);
	uint16_t GetRawADC(void);
	void CheckTpsTrigger(void);
	uint16_t GetTpsTriggerValue(void);
	uint16_t GetTPS();
	int16_t GetTpsDelta(void);
	void AddValue(const uint16_t adc_Value);
	int16_t Load(void);
	int16_t Save(void);

private:
	void learnClosedThrottle(void);
	void learnOpenThrottle(void);
	void checkTpsTrigger(void);
	void SetTpsClosedValue(uint16_t);
	void SetTpsOpenValue(uint16_t);
	void SetClosedThottleThreshold(uint16_t);
	void SetOpenThottleThreshold(uint16_t);
	bool crcCheckSettings(void);
	char _fName[5] = "sTPS";
	spiffs_file _filePtr;

	_Array3D *_MapPtr;
	uint16_t _RawAdc;
	uint16_t _tps_Percentage;
	int16_t _tps_Delta;

	struct t_SETTINGS
	{
		uint16_t Tps_ClosedValue;
		uint16_t Tps_OpenValue;
		uint16_t Tps_ClosedThrottleThreshold;
		uint16_t Tps_OpenThrottleThreshold;
		uint16_t _Crc;
	};
	t_SETTINGS TPSSettings;

	uint16_t _triggerValue;
	uint32_t TPSlast_time;
};

extern _TPS TpsSensor;

#endif /* TPS_H_ */
