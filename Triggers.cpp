/*
 * Triggers.cpp
 *
 *  Created on: 22 Mar. 2020
 *      Author: apaml
 */

#include "Triggers.h"

_Triggers::_Triggers()
{
	//Default Settings
	TriggerSettings.crankFilterTime = 40;
	TriggerSettings.syncFilterTime = 40;
	TriggerSettings.triggerEdge = RELUCTOR_FALLING;
	TriggerSettings.syncEdge = RELUCTOR_FALLING;
	TriggerSettings.crankingCycleCount = 8;
	TriggerSettings.crankingRpm = 450;
}

//----------------------------------------------------------------------------------
void _Triggers::setCrankFilterTime(uint16_t Crank_Filter_Time)
{

	noInterrupts();
	TriggerSettings.crankFilterTime = Crank_Filter_Time;
	interrupts();
}

uint16_t _Triggers::getCrankFilterTime(void)
{

	return TriggerSettings.crankFilterTime;
}
//----------------------------------------------------------------------------------
void _Triggers::setSyncFilterTime(uint16_t Sync_Filter_Time)
{

	noInterrupts();
	TriggerSettings.syncFilterTime = Sync_Filter_Time;
	interrupts();
}
uint16_t _Triggers::getSyncFilterTime(void)
{
	return TriggerSettings.syncFilterTime;
}
//----------------------------------------------------------------------------------
void _Triggers::setSyncTriggerEdge(uint16_t Sync_Trigger_Edge)
{

	noInterrupts();
	TriggerSettings.syncEdge = Sync_Trigger_Edge;
	interrupts();
}
uint16_t _Triggers::getSyncTriggerEdge(void)
{
	return TriggerSettings.syncEdge;
}
//----------------------------------------------------------------------------------
void _Triggers::setCrankTriggerEdge(uint16_t Crank_Trigger_Edge)
{

	noInterrupts();
	TriggerSettings.triggerEdge = Crank_Trigger_Edge;
	interrupts();
}
uint16_t _Triggers::getCrankTriggerEdge(void)
{
	return TriggerSettings.syncEdge;
}
//----------------------------------------------------------------------------------

void _Triggers::setCrankingCycleCount(uint16_t CrankingCycleCount)
{

	noInterrupts();
	TriggerSettings.crankingCycleCount = CrankingCycleCount;
	interrupts();
}
uint16_t _Triggers::getCrankingCycleCount(void)
{
	return TriggerSettings.crankingCycleCount;
}

//----------------------------------------------------------------------------------

bool _Triggers::isCrankNoise(void)
{

	uint32_t Current_Micros = micros();
	bool returnVal = false;

	uint32_t Trigger_Difference = Current_Micros - _Last_Crank_Micros;
	if (Trigger_Difference <= TriggerSettings.crankFilterTime)
	{
		returnVal = true;
	}
	_Last_Crank_Micros = Current_Micros;
	return returnVal;
}
//----------------------------------------------------------------------------------

bool _Triggers::isSyncNoise(void)
{

	uint32_t Current_Micros = micros();
	bool returnVal = false;

	uint32_t Trigger_Difference = Current_Micros - _Last_SyncMicros;
	if (Trigger_Difference <= TriggerSettings.syncFilterTime)
	{
		returnVal = true;
	}
	_Last_SyncMicros = Current_Micros;
	return returnVal;
}

//----------------------------------------------------------------------------------------
bool _Triggers::clockCrankTrigger(void)
{

	if (isCrankNoise())
	{
		return false;
		HWSERIAL.println("Noise");
	}
	_TriggerMillis = 0; //reset timeout timer

	if (_SyncFlag)
	{
		_numOfTriggerCycles++;
	}
	else
	{
		_numOfTriggerCycles = 0;
	}
	_TriggerIndex++;

	if (_numOfTriggerCycles > TriggerSettings.crankingCycleCount)
	{
		_TriggerFlag = true;
	}
	return true;
}
//----------------------------------------------------------------------------------------

bool _Triggers::clockSyncTrigger(void)
{

	if (isSyncNoise())
	{ // if trigger is noise return false
		return false;
	}
	if (_TriggerIndex == 2)
	{
		return false; // second sync trigger skip
	}

	_SyncMillis = 0; //reset timeout timer
	_SyncFlag = true;
	_TriggerIndex = 0;

	return true;
}
//----------------------------------------------------------------------------------------

void _Triggers::checkTimeOuts(void)
{

	if (_SyncMillis > SYNC_TRIGGER_TIMEOUT)
	{
		_SyncFlag = false;
	}

	if (_TriggerMillis > CRANK_TRIGGER_TIMEOUT)
	{
		_TriggerFlag = false;
		_TriggerIndex = 0;
		_numOfTriggerCycles = 0;
	}
}

//----------------------------------------------------------------------------------------

int16_t _Triggers::Load(void)
{

	int16_t err;
	HWSERIAL.print("Loading Trigger Settings -> ");
	err = eRAM.f_open(_filePtr, _fName, SPIFFS_RDONLY);
	if (err < 0)
	{

		HWSERIAL.println("err open");
		return err;
	}
	err = eRAM.f_read(_filePtr, reinterpret_cast<const char *>(&TriggerSettings),
					  sizeof(TriggerSettings));
	if (err < 0)
	{
		HWSERIAL.println("err Read");
		return err;
	}
	eRAM.f_close(_filePtr);

	if (!crcCheckSettings())
	{
		HWSERIAL.print("crc error ");
	}
	else
	{
		HWSERIAL.println("Triggers Load Ok");
	}

	return 1; // all is ok
}

//----------------------------------------------------------------------------------------
void _Triggers::setCrankingRpm(uint16_t Cranking_Rpm)
{

	//framLow.writeWord(EEPROM_CRANKING_RPM_FRAM, Cranking_Rpm);

	noInterrupts();
	TriggerSettings.crankingRpm = Cranking_Rpm;
	interrupts();
}
//----------------------------------------------------------------------------------------
uint16_t _Triggers::getCrankingRpm(void)
{
	return TriggerSettings.crankingRpm;
}

// ---------------------------------------------------------------------------
int16_t _Triggers::Save(void)
{

	int16_t err;

	HWSERIAL.print("Saving Trigger Settings -> ");
	TriggerSettings.Crc = CRC16.ccitt(reinterpret_cast<uint8_t *>(&TriggerSettings), //Calculate CRC
									  sizeof(TriggerSettings) - WORDSIZE);

	err = eRAM.f_open(_filePtr, _fName,
					  SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR);
	if (err < 0)
	{
		HWSERIAL.println("err Open");
		return err;
	}
	err = eRAM.f_write(_filePtr, reinterpret_cast<const char *>(&TriggerSettings),
					   sizeof(TriggerSettings));
	if (err < 0)
	{
		HWSERIAL.println("err Write");
		return err;
	}
	eRAM.f_close(_filePtr);

	HWSERIAL.println("Trigger Save OK");

	return 1; // all is ok
}
// ---------------------------------------------------------------------------
bool _Triggers::crcCheckSettings(void)
{

	if (TriggerSettings.Crc != CRC16.ccitt(reinterpret_cast<uint8_t *>(&TriggerSettings), //Calculate CRC
										   sizeof(TriggerSettings) - WORDSIZE))
	{
		return false;
	}

	return true; // all is ok
}

// ---------------------------------------------------------------------------
