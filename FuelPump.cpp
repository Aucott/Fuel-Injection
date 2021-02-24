/*
 * FuelPump.cpp
 *
 *  Created on: 1 Sep. 2019
 *      Author: apaml
 */

#include "FuelPump.h"

_FuelPump::_FuelPump()
{
	// Set up defaults
	//_pumpOn = false;
	FPumpSettings.FuelPump_mode = FUELPUMPENABLED;
	FPumpSettings.FuelPump_primeTime = 3000;
	_sampleTime = 500;
}

void _FuelPump::reset_runtime(void)
{
	_pumpRunTime = 0;
}
//------------------------------------------------------------------
bool _FuelPump::isRunning(void)
{
	return _pumpOn;
}
//------------------------------------------------------------------

void _FuelPump::setMode(uint16_t mode)
{

	noInterrupts();
	FPumpSettings.FuelPump_mode = mode;
	interrupts();
}
//------------------------------------------------------------------

uint16_t _FuelPump::getMode(void)
{
	return FPumpSettings.FuelPump_mode;
}
//------------------------------------------------------------------
void _FuelPump::setPrimeTime(uint16_t primeTime)
{

	noInterrupts();
	FPumpSettings.FuelPump_primeTime = primeTime;
	interrupts();
}
//------------------------------------------------------------------
uint16_t _FuelPump::getPrimeTime(void)
{
	return FPumpSettings.FuelPump_primeTime;
}
//------------------------------------------------------------------

void _FuelPump::checkRunningTime(void)
{

	if (_clock >= _sampleTime)
	{ // do once every sample time
		_clock = _clock - _sampleTime;

		switch (FPumpSettings.FuelPump_mode)
		{

		case FUELPUMPOFF:
			digitalWriteFast(FUEL_PUMP_PIN, 0);
			break;

		case FUELPUMPENABLED:
			_pumpOn = (_pumpRunTime < FPumpSettings.FuelPump_primeTime);
			digitalWriteFast(FUEL_PUMP_PIN, _pumpOn);
			break;

		case FUELPUMPINVERTED:
			_pumpOn = (_pumpRunTime < FPumpSettings.FuelPump_primeTime);
			digitalWriteFast(FUEL_PUMP_PIN, !_pumpOn);
			break;

		case FUELPUMPALWAYSON:
			digitalWriteFast(FUEL_PUMP_PIN, 1); //todo Create idle  class with thermofan idle up
			break;
		}
	}
}

//----------------------------------------------------------------------------------------

int16_t _FuelPump::Load(void)
{

	int16_t err;
	HWSERIAL.print("Loading FPump Settings -> ");
	err = eRAM.f_open(_filePtr, _fName, SPIFFS_RDONLY);
	if (err < 0)
	{

		HWSERIAL.println("err open");
		return err;
	}
	err = eRAM.f_read(_filePtr, reinterpret_cast<const char *>(&FPumpSettings),
					  sizeof(FPumpSettings));
	if (err < 0)
	{
		HWSERIAL.println("err Read");
		return err;
	}
	eRAM.f_close(_filePtr);

	if (!crcCheckSettings())
	{
		HWSERIAL.println("FPump crc error ");
		
	}
	else
	{

		HWSERIAL.println("FPump Load Ok");
	}

	return 1; // all is ok
}
// ---------------------------------------------------------------------------
int16_t _FuelPump::Save(void)
{

	int16_t err;

	HWSERIAL.print("Saving FPump Settings -> ");

	FPumpSettings.Crc = CRC16.ccitt(reinterpret_cast<uint8_t *>(&FPumpSettings), //Calculate CRC
									sizeof(FPumpSettings) - WORDSIZE);

	err = eRAM.f_open(_filePtr, _fName,
					  SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR);
	if (err < 0)
	{
		HWSERIAL.println("err Open");
		return err;
	}
	err = eRAM.f_write(_filePtr, reinterpret_cast<const char *>(&FPumpSettings),
					   sizeof(FPumpSettings));
	if (err < 0)
	{
		HWSERIAL.println("err Write");
		return err;
	}
	eRAM.f_close(_filePtr);

	HWSERIAL.println("FPump Save OK");

	return 1; // all is ok
}
// ---------------------------------------------------------------------------
bool _FuelPump::crcCheckSettings(void)
{

	if (FPumpSettings.Crc != CRC16.ccitt(reinterpret_cast<uint8_t *>(&FPumpSettings), //Calculate CRC
										 sizeof(FPumpSettings) - WORDSIZE))
	{
		return false;
	}

	return true; // all is ok
}

// ---------------------------------------------------------------------------
