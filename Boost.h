/*
 * Boost.h
 *
 *  Created on: 20 Aug. 2019
 *      Author: apaml
 */

#ifndef BOOST_H_
#define BOOST_H_

#include "Arduino.h"
#include "Array3D.h"
#include "PID_v1.h"
#include "Settings.h"

class _BoostControl
{
public:
	_BoostControl();
	_BoostControl(_Array3D *, _Array3D *, _Array3D *);

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
			HWSERIAL.print("Uploading Boost Settings ->");
			buf[1] = getMinDutyCycle();
			buf[2] = getMaxDutyCycle();
			buf[3] = getKp();
			buf[4] = getKi();
			buf[5] = getKd();
			buf[6] = getClosedLoopEnable();
			buf[7] = getUseDualBoostMaps();

			buf[0] = CRC16.ccitt(
				reinterpret_cast<uint8_t *>(&(buf[1])),
				(WORD_PACKET_SIZE * sizeof(uint16_t)) - sizeof(uint16_t));

			int n = hid_sendWithAck(reinterpret_cast<uint8_t *>(buf));

			if (n)
			{
				HWSERIAL.println("Error Uploading Boost Settings");
			}
			HWSERIAL.println("OK");
			break;
		}

		case 0XA2:
			HWSERIAL.println("Downloading Boost Settings");
			crc =
				CRC16.ccitt(
					reinterpret_cast<uint8_t *>((uint16_t *)(&(Rxdata.dataArray[0]))),
					dataCount * sizeof(uint16_t));
			if (Crc == crc)
			{
				setMinDutyCycle(Rxdata.dataArray[0]);
				setMaxDutyCycle(Rxdata.dataArray[1]);
				setKp(Rxdata.dataArray[2]);
				setKi(Rxdata.dataArray[3]);
				setKd(Rxdata.dataArray[4]);
				setClosedLoopEnable(Rxdata.dataArray[5]);
				setUseDualBoostMaps(Rxdata.dataArray[6]);
				BoostSettings.Crc = Crc;
				Save();
			}
			else
			{
				HWSERIAL.println("TPS Settings Crc error");
				Errors.setError(ERR_SETTINGS_LOAD_CRC);
			}
			break;
		}
	}
	//----------------------------------------------------------------------------------------

	void begin(void);
	void calcBoostDuty(uint16_t Rpm, float Tps, uint16_t boost);
	void setClosedLoopEnable(uint16_t);
	uint16_t getClosedLoopEnable(void);
	void setUseDualBoostMaps(uint16_t);
	uint16_t getUseDualBoostMaps(void);
	uint16_t getTargetBoost(void);
	uint16_t getWasteGateDuty(void);
	int16_t getPidDuty(void);
	void setMinDutyCycle(uint16_t);
	uint16_t getMinDutyCycle();
	void setMaxDutyCycle(uint16_t);
	uint16_t getMaxDutyCycle();
	uint16_t getRawBaseDuty(void);
	uint16_t getActiveMap(void);
	int16_t getBoostPressureError(void);

	void setKp(uint16_t);
	uint16_t getKp(void);
	void setKi(uint16_t);
	uint16_t getKi(void);
	void setKd(uint16_t);
	uint16_t getKd(void);
	int16_t Load(void);

private:
	int16_t Save(void);
	bool crcCheckSettings(void);
	char _fName[7] = "sBoost";
	spiffs_file _filePtr;

	struct t_SETTINGS
	{
		uint16_t Boost__minPidDuty;
		uint16_t Boost_maxPidDuty;
		uint16_t Boost_Kp;
		uint16_t Boost_Ki;
		uint16_t Boost_Kd;
		uint16_t Boost_closedLoopEnable;
		uint16_t Boost_useDualBoostMaps;
		uint16_t Crc;
	};

	t_SETTINGS BoostSettings;

	float _rawBaseDuty = 0;
	uint16_t _mapActive = 0;
	int16_t _boostPressureError = 0; //
	float _wasteGateDuty = 0;
	double _targetBoost = 0;
	double _currentBoost;
	double _PidDuty;

	_Array3D *Target1MapPtr;
	_Array3D *Target2MapPtr;
	_Array3D *DutyCycleMapPtr;
	PID BoostControlPID;
};

extern _BoostControl BoostControl;

#endif /* BOOST_H_ */
