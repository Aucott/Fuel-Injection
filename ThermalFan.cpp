/*
 * ThermalFan.cpp
 *
 *  Created on: 1 Sep. 2019
 *      Author: apaml
 */

#include "ThermalFan.h"

_ThermalFan::_ThermalFan()
{

	//Defaults
	//_OnTemperature = 90;
	//_Hystheresis = 2;
	_fanRunning = false;
	//_AdditionIdleEffort = 5; //percentage
	//_AdditionIdleRpm = 50;

	//_IdleUpFanDelay = 500; //ms

	_sampleTime = 300;
}

//------------------------------------------------------------------
uint16_t _ThermalFan::isRunning(void)
{
	return _fanRunning;
}
//------------------------------------------------------------------
void _ThermalFan::setOnTempertaure(uint16_t OnTemperature)
{

	//framLow.writeWord(ON_TEMPERATURE_FRAM, OnTemperature);
	noInterrupts();
	TFanSettings.ThermalFan_OnTemperature = OnTemperature;
	interrupts();
}
int16_t _ThermalFan::getOnTempertaure(void)
{
	return TFanSettings.ThermalFan_OnTemperature;
}
//------------------------------------------------------------------
void _ThermalFan::setHystheresis(uint16_t Hystheresis)
{

	noInterrupts();
	TFanSettings.ThermalFan_Hystheresis = Hystheresis;
	interrupts();
}

uint16_t _ThermalFan::getHystheresis(void)
{
	return TFanSettings.ThermalFan_Hystheresis;
}

//------------------------------------------------------------------

void _ThermalFan::setIdleUpFanOnDelay(uint16_t IdleUpFanDelay)
{

	noInterrupts();
	TFanSettings.ThermalFan_IdleUpFanDelay = IdleUpFanDelay;
	interrupts();
}

uint16_t _ThermalFan::getIdleUpFanOnDelay(void)
{
	return TFanSettings.ThermalFan_IdleUpFanDelay;
}
//------------------------------------------------------------------
void _ThermalFan::setAdditionIdleEffort(uint16_t AdditionIdleEffort)
{

	noInterrupts();
	TFanSettings.ThermalFan_AdditionIdleEffort = AdditionIdleEffort;
	interrupts();
}

uint16_t _ThermalFan::getAdditionIdleEffort(void)
{
	return TFanSettings.ThermalFan_AdditionIdleEffort;
}

//------------------------------------------------------------------
void _ThermalFan::setAdditionIdleRpm(uint16_t AdditionIdleRpm)
{
	//framLow.writeWord(IDLE_FAN_ON_DELAY_FRAM, AdditionIdleRpm);
	noInterrupts();
	TFanSettings.ThermalFan_AdditionIdleRpm = AdditionIdleRpm;
	interrupts();
}

uint16_t _ThermalFan::getAdditionIdleRpm(void)
{
	return TFanSettings.ThermalFan_AdditionIdleRpm;
}

//------------------------------------------------------------------
uint16_t _ThermalFan::getAddedIdleRpm(void)
{

	uint16_t returnVal = (_fanRunning) ? TFanSettings.ThermalFan_AdditionIdleRpm : 0;

	return returnVal;
}

//------------------------------------------------------------------
uint16_t _ThermalFan::getAddedIdleEffort(void)
{

	uint16_t returnVal = (_fanRunning) ? TFanSettings.ThermalFan_AdditionIdleEffort : 0;

	return returnVal;
}

//----------------------------------------------------------------------------------------

void _ThermalFan::check_Temperature(float CltTemp)
{

	if (_clock >= _sampleTime)
	{ // do once every sample time
		_clock = _clock - _sampleTime;

		if (CltTemp >= TFanSettings.ThermalFan_OnTemperature && _fanRunning == false)
		{ //Mark that fan is on starting timer
			_fanRunning = true;
			_fanRunTime = 0;
		}
		if (CltTemp <= TFanSettings.ThermalFan_OnTemperature - TFanSettings.ThermalFan_Hystheresis)
		{ //Turn fan off if below temp
			_fanRunning = false;
			digitalWriteFast(OUTPUT_1_PIN, _fanRunning);
		}
		if (_fanRunning && _fanRunTime > TFanSettings.ThermalFan_IdleUpFanDelay)
		{ //Turn fan on if time over time delay

			digitalWriteFast(OUTPUT_1_PIN, _fanRunning);
		}
	}
}

//----------------------------------------------------------------------------------------

int16_t _ThermalFan::Load(void)
{

	int16_t err;
	HWSERIAL.print("Loading TFan Settings ->");
	err = eRAM.f_open(_filePtr, _fName, SPIFFS_RDONLY);
	if (err < 0)
	{

		HWSERIAL.println("err open");
		return err;
	}
	err = eRAM.f_read(_filePtr, reinterpret_cast<const char *>(&TFanSettings),
					  sizeof(TFanSettings));
	if (err < 0)
	{
		HWSERIAL.println("err Read");
		return err;
	}
	eRAM.f_close(_filePtr);

	if (!crcCheckSettings())
	{
		HWSERIAL.print("crc error ");
		HWSERIAL.println(_fName);
	}
	else
	{
		HWSERIAL.println("TFan Load Ok");
	}

	return 1; // all is ok
}
// ---------------------------------------------------------------------------
int16_t _ThermalFan::Save(void)
{

	int16_t err;

	HWSERIAL.print("Saving TFan Settings -> ");

	TFanSettings.Crc = CRC16.ccitt(reinterpret_cast<uint8_t *>(&TFanSettings), //Calculate CRC
								   sizeof(TFanSettings) - WORDSIZE);

	err = eRAM.f_open(_filePtr, _fName,
					  SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR);
	if (err < 0)
	{
		HWSERIAL.println("err Open");
		return err;
	}
	err = eRAM.f_write(_filePtr, reinterpret_cast<const char *>(&TFanSettings),
					   sizeof(TFanSettings));
	if (err < 0)
	{
		HWSERIAL.println("err Write");
		return err;
	}
	eRAM.f_close(_filePtr);

	HWSERIAL.println("TFan Save OK");

	return 1; // all is ok
}
// ---------------------------------------------------------------------------
bool _ThermalFan::crcCheckSettings(void)
{

	if (TFanSettings.Crc != CRC16.ccitt(reinterpret_cast<uint8_t *>(&TFanSettings), //Calculate CRC
										sizeof(TFanSettings) - WORDSIZE))
	{
		return false;
	}

	return true; // all is ok
}

// ---------------------------------------------------------------------------
