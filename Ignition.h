/*
 * Ignition.h
 *
 *  Created on: 3 Apr. 2019
 *      Author: apaml
 */

#ifndef IGNITION_H_
#define IGNITION_H_

#include "Arduino.h"
#include "InjFuel.h"
#include "defines.h"

#define MIN_IGN_ADVANCE -500
#define MAX_IGN_ADVANCE 500

#define DISTRIBUTOR 0
#define WASTED_SPARK 1
#define DIRECT_FIRE 2

class _Ignition
{
public:
	_Ignition(_Array3D *, _Array3D *, _Array3D *, _Array3D *, _Array3D *,
			  _Array3D *);

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
			HWSERIAL.print("Uploading IGN Settings ->");
			buf[1] = GetIgnMasterTrim();
			buf[2] = GetIgnitionMode();
			buf[3] = GetIgnitionFireEdge();
			buf[4] = GetBaseIgnAngle();
			buf[5] = isTimingLocked();
			buf[6] = Get_Locked_Timing_Angle();

			buf[0] = CRC16.ccitt(
				reinterpret_cast<uint8_t *>(&(buf[1])),
				(WORD_PACKET_SIZE * sizeof(uint16_t)) - sizeof(uint16_t));

			int n = hid_sendWithAck(reinterpret_cast<uint8_t *>(buf));

			if (n)
			{
				HWSERIAL.println("Error Uploading IGN Settings");
			}
			HWSERIAL.println("OK");
			break;
		}

		case 0XA2:
		{
			HWSERIAL.println("Downloading IGN Settings");
			crc =
				CRC16.ccitt(
					reinterpret_cast<uint8_t *>((uint16_t *)(&(Rxdata.dataArray[0]))),
					dataCount * sizeof(uint16_t));
			if (Crc == crc)
			{

				SetIgnMasterTrim(Rxdata.dataArray[0]);
				SetIgnitionMode(Rxdata.dataArray[1]);
				SetIgnitionFireEdge(Rxdata.dataArray[2]);
				SetBaseIgnAngle(Rxdata.dataArray[3]);
				Set_Locked_Timing_Enable(Rxdata.dataArray[4]);
				Set_Locked_Timing_Angle(Rxdata.dataArray[5]);
				IgnSettings.Crc = Crc;
				Save();
			}
			else
			{
				Errors.setError(ERR_SETTINGS_LOAD_CRC);
				HWSERIAL.print("IGN settings load error ");
			}
		}

		break;
		}
	}

	//----------------------------------------------------------------------------------------
	void SetCalcFlag(void)
	{
		_calcNow = true;
		return;
	}

	//----------------------------------------------------------------------------------------

	int16_t GetIgnAngle(void)
	{
		if (RunningData.IsEngineRunning())
		{

			return _correctedIgnAngle;
		}
		else
		{
			return _crankingIgnAngle;
		}
	}
	//----------------------------------------------------------------------------------------
	int16_t GetRawIgnAngle(void)
	{

		return _rawIgnAngle;
	}
	//----------------------------------------------------------------------------------------
	uint16_t GetCrankingIgnAngle(void)
	{

		return _crankingIgnAngle;
	}
	//----------------------------------------------------------------------------------------
	int16_t GetCltCorrection(void)
	{

		return _cltCorrection;
	}
	//----------------------------------------------------------------------------------------
	int16_t GetMatCorrection(void)
	{

		return _matCorrection;
	}

	//----------------------------------------------------------------------------------------
	//uint16_t GetNumberOfIgnitors(void) {
	//	return IgnSettings._NumberOfIgnitors;

	//}
	//----------------------------------------------------------------------------------------
	void SetIgnitionFireEdge(uint16_t IgnitionFireEdge)
	{
		IgnSettings.FireEdge = IgnitionFireEdge;
	}
	//----------------------------------------------------------------------------------------
	uint16_t GetIgnitionFireEdge(void)
	{

		return IgnSettings.FireEdge;
	}
	//----------------------------------------------------------------------------------------
	void RecordActualDwell(uint16_t dwellTime)
	{
		_ActualDwell = dwellTime;
	}
	//----------------------------------------------------------------------------------------
	uint16_t GetActualDwell(void)
	{
		return _ActualDwell;
	}

	//----------------------------------------------------------------------------------------
	uint16_t GetDwellTime(void)
	{
		return _IgnDwellMicros;
	}
	//----------------------------------------------------------------------------------------

	int16_t GetIdleIgnTrim(void)
	{
		return _idleIgnTrim;
	}
	//----------------------------------------------------------------------------------------

	void SetIgnMasterTrim(uint16_t trim)
	{

		noInterrupts();
		IgnSettings.MasterTrim = trim;
		interrupts();
	}

	uint16_t GetIgnMasterTrim(void)
	{
		return IgnSettings.MasterTrim;
	}
	//----------------------------------------------------------------------------------------

	void SetIgnitionMode(uint16_t mode)
	{

		noInterrupts();
		IgnSettings.Mode = mode;
		interrupts();
	}

	uint16_t GetIgnitionMode(void)
	{

		//IgnSettings.Mode = WASTED_SPARK;
		return IgnSettings.Mode;
	}

	//----------------------------------------------------------------------------------------

	void SetBaseIgnAngle(uint16_t BaseIgnAngle)
	{

		noInterrupts();
		IgnSettings.BaseAngle = BaseIgnAngle;
		interrupts();
	}

	uint16_t GetBaseIgnAngle(void)
	{
		return IgnSettings.BaseAngle;
	}

	//----------------------------------------------------------------------------------------

	void Set_Locked_Timing_Angle(uint16_t Locked_Timing_Angle)
	{

		noInterrupts();
		IgnSettings.Locked_Timing_Angle = Locked_Timing_Angle;
		interrupts();
	}

	uint16_t Get_Locked_Timing_Angle(void)
	{
		return IgnSettings.Locked_Timing_Angle;
	}

	//----------------------------------------------------------------------------------------
	void Set_Locked_Timing_Enable(uint16_t Locked_Timing_Enable)
	{
		noInterrupts();
		IgnSettings.Locked_Timing_Enable = Locked_Timing_Enable;
		interrupts();
	}

	uint16_t isTimingLocked(void)
	{
		return IgnSettings.Locked_Timing_Enable;
	}

	//----------------------------------------------------------------------------------------

	void CalcIgnAngle(void);
	void DoIgnCalcs(const int16_t, const int16_t, const uint16_t,
					const uint16_t);
	int16_t Load(void);

private:
	void CalcCrankingIgnAngle(const int16_t);
	void CalcCltCorrection(const int16_t);
	void CalcMatCorrection(const int16_t);
	void CalcIdleIgnTrim(const uint16_t);
	void CalcDwell(const uint16_t);
	bool crcCheckSettings(void);
	int16_t Save(void);
	spiffs_file _filePtr;

	char _fName[5] = "sIGN";
	_Array3D *_ignAngleMap;
	_Array3D *_crankAngleMap;
	_Array3D *_ignCltCorrectionMap;
	_Array3D *_ignMatCorrectionMap;
	_Array3D *_ignDwellMap;
	_Array3D *_ignIdleTrimMap;

	bool _calcNow = false;

	//Important strcture to be the same as terminal program
	struct t_SETTINGS
	{
		int16_t MasterTrim;
		uint16_t Mode;
		uint16_t FireEdge; // 1-Falling 0-Rising
		int16_t BaseAngle;
		uint16_t Locked_Timing_Enable;
		int16_t Locked_Timing_Angle;
		uint16_t Crc;
	};
	t_SETTINGS IgnSettings;
	const int SETTINGSIZE = sizeof(t_SETTINGS);

	//Calculated Values
	uint16_t _ActualDwell; //uS
	int16_t _matCorrection;
	int16_t _cltCorrection;
	int16_t _rawIgnAngle;
	int16_t _correctedIgnAngle;
	int16_t _crankingIgnAngle;
	int16_t _idleIgnTrim;
	uint16_t _IgnDwellMicros; //uS
	uint16_t _sampleTime;
	elapsedMillis _clock;
};

extern _Ignition Ignition;

#endif /* IGNITION_H_ */
