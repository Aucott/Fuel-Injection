/*
 * Idle.h
 *
 *  Created on: 17 Aug. 2019
 *      Author: apaml
 */

#ifndef IDLE_H_
#define IDLE_H_

#include "Arduino.h"
#include "PID_v1.h"
#include "Array3D.h"
#include "ThermalFan.h"

class _Idle
{
public:
	_Idle();
	_Idle(_Array3D *, _Array3D *, _Array3D *);

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
			HWSERIAL.print("Uploading Idle Settings ->");
			buf[1] = getMinDutycycle();
			buf[2] = getMaxDutycycle();
			buf[3] = getKp();
			buf[4] = getKi();
			buf[5] = getKd();
			buf[6] = getCloseIdleOnBoost();
			buf[7] = getClosedLoopEnable();
			
			buf[0] = CRC16.ccitt(
				reinterpret_cast<uint8_t *>(&(buf[1])),
				(WORD_PACKET_SIZE * sizeof(uint16_t)) - sizeof(uint16_t));

			int n = hid_sendWithAck(reinterpret_cast<uint8_t *>(buf));

			if (n)
			{
				HWSERIAL.println("Error Uploading Idle Settings");
			}
			HWSERIAL.println("OK");
			break;
		}

		case 0XA2:
			HWSERIAL.println("Downloading Idle Settings");
			crc =
				CRC16.ccitt(
					reinterpret_cast<uint8_t *>((uint16_t *)(&(Rxdata.dataArray[0]))),
					dataCount * sizeof(uint16_t));
			if (Crc == crc)
			{
				setMinDutycycle(Rxdata.dataArray[0]);
				setMaxDutycycle(Rxdata.dataArray[1]);
				setKp(Rxdata.dataArray[2]);
				setKi(Rxdata.dataArray[3]);
				setKd(Rxdata.dataArray[4]);
				setCloseIdleOnBoost(Rxdata.dataArray[5]);
				setClosedLoopEnable(Rxdata.dataArray[6]);				
				Save();
			}
			else
			{
				HWSERIAL.print("Idle settings load error ");
				Errors.setError(ERR_SETTINGS_LOAD_CRC);
			}
			break;
		}
	}

	//----------------------------------------------------------------------------------------

	void setKp(uint16_t);
	uint16_t getKp(void);
	void setKi(uint16_t);
	uint16_t getKi(void);
	void setKd(uint16_t);
	uint16_t getKd(void);

	void setClosedLoopEnable(uint16_t);
	uint16_t getClosedLoopEnable(void);
	void setCloseIdleOnBoost(uint16_t);
	uint16_t getCloseIdleOnBoost(void);

	void calcIdleDuty(uint16_t, int16_t, float, bool, uint32_t, bool);

	void setStartIdlingRpm(uint16_t);
	void setMinDutycycle(uint16_t);
	uint16_t getMinDutycycle(void);
	void setMaxDutycycle(uint16_t);
	uint16_t getMaxDutycycle(void);
	int16_t Load(void);

	//Functions that return data
	uint16_t getRawTargetIdleRpm(void);
	uint16_t getTargetIdleRpm(void);
	uint16_t getRawBaseDuty(void);
	uint16_t getIdleEffortPostCrank(void);
	uint16_t getIdleEffortThermalFan(void);
	int16_t getIdleEffortPID(void); //  sent back 1 order of magnitude greater
	uint16_t getIdleEffortOpenLoop(void);
	uint16_t getIdleEffortUnclipped(void);
	int16_t getIdleRpmError(void);
	uint16_t getIdleEffortDuty(void);
	uint16_t closedLoopIsActive(void);

private:
	void Begin(void);
	int16_t Save(void);
	bool crcCheckSettings(void);
	char _fName[6] = "sIdle";
	spiffs_file _filePtr;
	uint16_t _sampleTime;
	elapsedMicros _clock;

	struct t_SETTINGS
	{

		uint16_t Idle_minPidDuty;
		uint16_t Idle_maxPidDuty;
		int16_t Idle_Kp;
		int16_t Idle_Ki;
		int16_t Idle_Kd;
		uint16_t Idle_closeIdleOnBoost;
		uint16_t Idle_closedLoopEnable;
		uint16_t Crc;
	};

	t_SETTINGS IdleSettings;

	_Array3D *_targetRpmMap;
	_Array3D *_baseIdleDutyMap;
	_Array3D *_postCrankIdleMap;

	double _currentRpm = 0; //cast to float for PID;

	uint16_t _idleEffortDuty = 0; //Final idle effort, between the range of the minimum idle effort setting and 100%
	double _rawTargetIdleRpm = 0; //Target idle speed, from the target idle speed table (vs coolant temp) TODO change PID to accept uint16_t
	uint16_t _targetIdleRpm = 0;  //Target idle speed, with correction
	uint16_t _rawBaseDuty = 0;	  //Base idle duty cycle, from the base idle duty cycle table (vs coolant temp)
	uint16_t _idleEffortPostCrank = 0;
	uint16_t _idleEffortThermalFan = 0; //Calculated Additional idle effort from Thermal Fan
	uint16_t _idleEffortAirCon = 0;		//Additional idle effort from air conditioner
	uint16_t _idleEffortElectrical = 0; //Additional idle effort from electrical loads
	uint16_t _idleEffortOpenLoop = 0;	//Idle open loop value, ie the base duty plus all the open loop corrections but no closed loop corrections
	double _idleEffortPID = 0;			//Additional idle effort from closed loop idle control
	uint16_t _idleEffortUnclipped = 0;	//Idle valves added together (could be less than the minimum value or greater than 100)
	int16_t _idleRpmError = 0;			//Idle RPM error, ie actual RPM minus target idle RPM

	PID IdlePID;
};

extern _Idle Idle;

#endif /* IDLE_H_ */
