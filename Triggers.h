/*
 * Triggers.h
 *
 *  Created on: 22 Mar. 2020
 *      Author: apaml
 */

#ifndef TRIGGERS_H_
#define TRIGGERS_H_

#include "Arduino.h"
#include "defines.h"
#include "USB.h"
#include "Settings.h"
#include <FastCRC.h>
#include "errors.h"

#define RELUCTOR_FALLING 0
#define HALL_FALLING 1
#define HALL_RISING 3

#define SYNC_TRIGGER_TIMEOUT 6000 // 20 rpm
#define CRANK_TRIGGER_TIMEOUT 750

class _Triggers
{
public:
	_Triggers();

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
			HWSERIAL.print("Uploading TRIG Settings ->");
			buf[1] = getCrankTriggerEdge();
			buf[2] = getSyncTriggerEdge();
			buf[3] = getCrankFilterTime();
			buf[4] = getSyncFilterTime();
			buf[5] = getCrankingCycleCount();
			buf[6] = getCrankingRpm();

			buf[0] = CRC16.ccitt(
				reinterpret_cast<uint8_t *>(&(buf[1])),
				(WORD_PACKET_SIZE * sizeof(uint16_t)) - sizeof(uint16_t));

			int n = hid_sendWithAck(reinterpret_cast<uint8_t *>(buf));

			if (n)
			{
				HWSERIAL.println("Error Uploading TRIG Settings");
			}
			HWSERIAL.println("OK");
			break;
		}

		case 0XA2:
		{
			crc =
				CRC16.ccitt(
					reinterpret_cast<uint8_t *>((uint16_t *)(&(Rxdata.dataArray[0]))),
					dataCount * sizeof(uint16_t));
			if (Crc == crc)
			{
				setCrankTriggerEdge(Rxdata.dataArray[0]);
				setSyncTriggerEdge(Rxdata.dataArray[1]);
				setCrankFilterTime(Rxdata.dataArray[2]);
				setSyncFilterTime(Rxdata.dataArray[3]);
				setCrankingCycleCount(Rxdata.dataArray[4]);
				setCrankingRpm(Rxdata.dataArray[5]);
				TriggerSettings.Crc = crc;

				Save();
			}
			else
			{
				HWSERIAL.print("TRIG settings load error ");
				Errors.setError(ERR_SETTINGS_LOAD_CRC);
			}
		}
		break;
		}
	}
	//---------------------------------------------------------------------------------------
	bool getTriggerFlag(void)
	{
		return _TriggerFlag;
	}
	//---------------------------------------------------------------------------------------
	uint8_t getTriggerIndex(void)
	{
		return _TriggerIndex;
	}
	//---------------------------------------------------------------------------------------
	bool getSyncFlag(void)
	{
		return _SyncFlag;
	}
	//---------------------------------------------------------------------------------------
	void ResetTriggerIndex(void)
	{
		_TriggerIndex = 0;
	}
	//---------------------------------------------------------------------------------------

	void setCrankTriggerEdge(int count)
	{
		TriggerSettings.triggerEdge = count;
	}

	void setCrankFilterTime(uint16_t);
	uint16_t getCrankFilterTime(void);
	void setSyncFilterTime(uint16_t);
	uint16_t getSyncFilterTime(void);
	void setCrankingCycleCount(uint16_t);
	uint16_t getCrankingCycleCount(void);
	void setSyncTriggerEdge(uint16_t);
	uint16_t getSyncTriggerEdge(void);
	void setCrankTriggerEdge(uint16_t);
	uint16_t getCrankTriggerEdge(void);
	void checkTimeOuts(void);
	bool clockCrankTrigger(void);
	bool clockSyncTrigger(void);
	int16_t Load(void);
	uint16_t getCrankingRpm(void);

private:
	bool isCrankNoise(void);
	bool isSyncNoise(void);
	bool crcCheckSettings(void);
	void setCrankingRpm(uint16_t);
	int16_t Save(void);
	char _fName[6] = "sTrig";
	spiffs_file _filePtr;

	struct t_SETTINGS
	{
		uint16_t triggerEdge; // 2-Falling  3-Rising   4-Change
		uint16_t syncEdge;	  // 2-Falling  3-Rising   4-Change
		uint16_t crankFilterTime;
		uint16_t syncFilterTime;
		uint16_t crankingCycleCount;
		uint16_t crankingRpm;
		uint16_t Crc;
	};

	t_SETTINGS TriggerSettings;

	//Calculated Settings
	uint8_t _TriggerIndex = 0;
	uint16_t _numOfTriggerCycles = 0;

	bool _SyncFlag = false;
	bool _TriggerFlag = false;

	elapsedMillis _SyncMillis;
	elapsedMillis _TriggerMillis;

	uint32_t _Last_Crank_Micros;
	uint32_t _Last_SyncMicros;
};
extern _Triggers Triggers;
#endif /* TRIGGERS_H_ */
