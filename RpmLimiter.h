/*
 * Limits.h
 *
 *  Created on: 27 Apr. 2020
 *      Author: apaml
 */

#ifndef RPMLIMITER_H_
#define RPMLIMITER_H_

#include "Arduino.h"
#include "Array3D.h"
#include "Settings.h"

#define RPM_LIMITER_DISABLED 0
#define CUT_FUEL 1
#define CUT_IGN 2

class _RpmLimiter
{
public:
	_RpmLimiter(_Array3D *);

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
			HWSERIAL.print("Uploading RpmLimiter Settings ->");
			buf[1] = getSoftLimitIgnAngle();
			buf[2] = getAdditionalRpmToCut();
			buf[3] = getLimpHomeRpmLimit();
			buf[4] = getUseFuelCut();
			buf[5] = getUseIgnitionCut();
			

			buf[0] = CRC16.ccitt(
				reinterpret_cast<uint8_t *>(&(buf[1])),
				(WORD_PACKET_SIZE * sizeof(uint16_t)) - sizeof(uint16_t));

			int n = hid_sendWithAck(reinterpret_cast<uint8_t *>(buf));

			if (n)
			{
				HWSERIAL.println("Error Uploading RpmLimiter Settings");
			}
			HWSERIAL.println("OK");
			break;
		}

		case 0XA2:
			HWSERIAL.println("Downloading RpmLimiter Settings");
			crc =
				CRC16.ccitt(
					reinterpret_cast<uint8_t *>((uint16_t *)(&(Rxdata.dataArray[0]))),
					dataCount * sizeof(uint16_t));
			if (Crc == crc)
			{
				setSoftLimitIgnAngle(Rxdata.dataArray[0]);
				setAdditionalRpmToCut(Rxdata.dataArray[1]);
				setLimpHomeRpmLimit(Rxdata.dataArray[2]);
				setUseFuelCut(Rxdata.dataArray[3]);
				setUseIgnitionCut(Rxdata.dataArray[4]);
				RpmLimiterSettings.Crc = Crc;
				Save();
			}
			else
			{
				HWSERIAL.print("RpmLimiter settings load error ");
				Errors.setError(ERR_SETTINGS_LOAD_CRC);
			}
			break;
		}
	}

	//----------------------------------------------------------------------------------------
	void setSoftLimitIgnAngle(uint16_t softLimitIgnAngle)
	{

		noInterrupts();
		RpmLimiterSettings.softLimitIgnAngle = softLimitIgnAngle;
		interrupts();
	}

	uint16_t getSoftLimitIgnAngle(void)
	{

		return RpmLimiterSettings.softLimitIgnAngle;
	}

	//----------------------------------------------------------------------------------------
	void setAdditionalRpmToCut(uint16_t additionalRpmToCut)
	{

		noInterrupts();
		RpmLimiterSettings.additionalRpmToCut = additionalRpmToCut;
		interrupts();
	}

	uint16_t getAdditionalRpmToCut(void)
	{

		return RpmLimiterSettings.additionalRpmToCut;
	}

	//----------------------------------------------------------------------------------------

	void setLimpHomeRpmLimit(uint16_t limpHomeRpmLimit)
	{

		noInterrupts();
		RpmLimiterSettings.limpHomeLimit = limpHomeRpmLimit;
		interrupts();
	}

	uint16_t getLimpHomeRpmLimit(void)
	{
		return RpmLimiterSettings.limpHomeLimit;
	}
	//----------------------------------------------------------------------------------

	void setUseIgnitionCut(uint16_t IgnitionCut)
	{
		noInterrupts();
		RpmLimiterSettings.useIgnitionCut = IgnitionCut;
		interrupts();
	}

	uint16_t getUseIgnitionCut(void)
	{
		return RpmLimiterSettings.useIgnitionCut;
	}
	//----------------------------------------------------------------------------------------

	void setUseFuelCut(uint16_t FuelCut)
	{
		noInterrupts();
		RpmLimiterSettings.useFuelCut = FuelCut;
		interrupts();
	}
	uint16_t getUseFuelCut(void)
	{
		return RpmLimiterSettings.useFuelCut;
	}

	//----------------------------------------------------------------------------------------

	uint16_t getHardRPM(void)
	{
		return _hardRpmLimit;
	}
	//----------------------------------------------------------------------------------
	uint16_t getSoftRPM(void)
	{
		return _softRpmLimit;
	}
	//----------------------------------------------------------------------------------
	bool isHardRevLimited(void)
	{
		return _hardRpmLimitFlag;
	}
	//----------------------------------------------------------------------------------
	bool isSoftRevLimited(void)
	{
		return _softRpmLimitFlag;
	}
	//----------------------------------------------------------------------------------
	bool isOverBoosted(void)
	{
		return _overBoostFlag;
	}
	//----------------------------------------------------------------------------------
	bool isFuelCut(void)
	{ // returns true to cut fuel

		return (_hardRpmLimitFlag && (RpmLimiterSettings.useFuelCut == CUT_FUEL));
	}
	//----------------------------------------------------------------------------------
	bool isIgnCut(void)
	{ // returns true to cut ignition

		return (_hardRpmLimitFlag && (RpmLimiterSettings.useIgnitionCut == CUT_IGN));
	}
	//----------------------------------------------------------------------------------

	void checkRpmLimits(uint16_t, int16_t);
	int16_t ignAngleCorrection(int16_t);
	uint16_t getRpmLimitOption(void);
	int16_t Load(void);

private:
	void setRpmLimiterOption(uint16_t);
	void getRpmLimits(int16_t);
	int16_t Save(void);
	bool crcCheckSettings(void);
	char _fName[6] = "sRpmL";
	spiffs_file _filePtr;

	struct t_SETTINGS
	{
		int16_t softLimitIgnAngle;	 // Angle x 10
		uint16_t additionalRpmToCut; // rpm below hard rev cut
		uint16_t limpHomeLimit;
		uint16_t useFuelCut;
		uint16_t useIgnitionCut;
		uint16_t Crc;
	};

	t_SETTINGS RpmLimiterSettings;

	_Array3D *_rpm_Limit_Map;

	//Calculated Values Rpm
	uint16_t _softRpmLimit;
	uint16_t _hardRpmLimit;
	bool _softRpmLimitFlag = false;
	bool _hardRpmLimitFlag = false;

	//Calculated Values Boost
	const uint16_t _overboostLimitTime = 1000; // 1 Second
	bool _delayedOverBoostFlag = false;
	bool _instantOverBoostFlag = false;
	bool _overBoostFlag = false;

	uint16_t _softRpmLimitTime = 2000; // Milliseconds time
	elapsedMillis _softRpmLimitTimer;
};
extern _RpmLimiter RpmLimiter;

#endif /* RPMLIMITER_H_ */
