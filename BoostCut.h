/*
 * Limits.h
 *
 *  Created on: 27 Apr. 2020
 *      Author: apaml
 */

#ifndef BOOST_CUT_H_
#define BOOST_CUT_H_

#define BOOST_CUT_DISABLED 0
#define CUT_FUEL 1
#define CUT_IGN 2
#define CUT_FUEL_AND_IGN 3

#include "Arduino.h"
#include "Array3D.h"
#include "Settings.h"

class _BoostCut
{
public:
	_BoostCut();

	//----------------------------------------------------------------------------------------
	void processCommand(const rxData Rxdata)
	{
		uint16_t subCommand = Rxdata.subCommand;
		uint16_t dataCount = Rxdata.dataX;
		uint16_t Crc = Rxdata.dataY;
		uint16_t crc;

		switch (subCommand)
		{case 0XA1:
		{
			uint16_t buf[WORD_PACKET_SIZE];
			memset(buf, 0, sizeof(buf));
			HWSERIAL.print("Uploading BoostCut Settings ->");
			buf[1] = getOverBoostDelayed();
			buf[2] = getOverBoostInstant();
			buf[3] = getUseFuelCut();
			buf[4] = getUseIgnCut();			

			buf[0] = CRC16.ccitt(
				reinterpret_cast<uint8_t *>(&(buf[1])),
				(WORD_PACKET_SIZE * sizeof(uint16_t)) - sizeof(uint16_t));

			int n = hid_sendWithAck(reinterpret_cast<uint8_t *>(buf));

			if (n)
			{
				HWSERIAL.println("Error Uploading BoostCut Settings");
			}
			HWSERIAL.println("OK");
			break;
		}

		case 0XA2:
			HWSERIAL.println("Downloading BoostCut Settings");
			crc =
				CRC16.ccitt(
					reinterpret_cast<uint8_t *>((uint16_t *)(&(Rxdata.dataArray[0]))),
					dataCount * sizeof(uint16_t));
			if (Crc == crc)
			{

				setOverBoostDelayed(Rxdata.dataArray[0]);
				setOverBoostInstant(Rxdata.dataArray[1]);
				setUseFuelCut(Rxdata.dataArray[2]);
				setUseIgnitionCut(Rxdata.dataArray[3]);
				
				BoostCutSettings.Crc = Crc;
				Save();
			}
			else
			{
				HWSERIAL.print("BoostCut settings load error ");
				Errors.setError(ERR_SETTINGS_LOAD_CRC);
			}
			break;
		}
	}

	//----------------------------------------------------------------------------------------

	uint16_t getUseIgnCut(void)
	{
		return BoostCutSettings.useIgnitionCut;
	}

	//----------------------------------------------------------------------------------------

	uint16_t getUseFuelCut(void)
	{
		return BoostCutSettings.useFuelCut;
	}

	//----------------------------------------------------------------------------------------
	void setOverBoostDelayed(uint16_t overBoostDelayed)
	{
		noInterrupts();
		BoostCutSettings.overBoostDelayed = overBoostDelayed;
		interrupts();
	}

	uint16_t getOverBoostDelayed(void)
	{
		return BoostCutSettings.overBoostDelayed;
	}

	//----------------------------------------------------------------------------------------
	void setOverBoostInstant(uint16_t overBoostInstant)
	{
		noInterrupts();
		BoostCutSettings.overBoostInstant = overBoostInstant;
		interrupts();
	}

	uint16_t getOverBoostInstant(void)
	{
		return BoostCutSettings.overBoostInstant;
	}

	//----------------------------------------------------------------------------------

	void setUseIgnitionCut(uint16_t IgnitionCut)
	{
		noInterrupts();
		BoostCutSettings.useIgnitionCut = IgnitionCut;
		interrupts();
	}

	uint16_t getUseIgnitionCut(void)
	{
		return BoostCutSettings.useIgnitionCut;
	}

	//----------------------------------------------------------------------------------

	void setUseFuelCut(uint16_t FuelCut)
	{
		noInterrupts();
		BoostCutSettings.useFuelCut = FuelCut;
		interrupts();
	}

	
	//----------------------------------------------------------------------------------
	bool isOverBoosted(void)
	{
		return _overBoostFlag;
	}
	//----------------------------------------------------------------------------------
	bool isFuelCut(void)
	{ // returns true to cut fuel
		return (0 && (bool)BoostCutSettings.useFuelCut);
	}
	//----------------------------------------------------------------------------------
	bool isIgnCut(void)
	{ // returns true to cut ignition
		return (0 && (bool)BoostCutSettings.useIgnitionCut);
	}
	//----------------------------------------------------------------------------------

	void checkBoostLimits(uint16_t);
	uint16_t getBoostCutOption(void);
	int16_t Load(void);

private:
	int16_t Save(void);
	bool crcCheckSettings(void);
	char _fName[8] = "sBstCut";
	spiffs_file _filePtr;

	struct t_SETTINGS
	{

		uint16_t overBoostDelayed; //kPa x 10
		uint16_t overBoostInstant;
		uint16_t useFuelCut;
		uint16_t useIgnitionCut;
		uint16_t Crc;
	};
	t_SETTINGS BoostCutSettings;


	//Calculated Values Boost
	const uint16_t _overboostLimitTime = 1000; // 1 Second
	bool _delayedOverBoostFlag = false;
	bool _instantOverBoostFlag = false;
	bool _overBoostFlag = false;

	elapsedMillis _overboostTimer;
};
extern _BoostCut BoostCut;

#endif /* BOOST_CUT_H_ */
