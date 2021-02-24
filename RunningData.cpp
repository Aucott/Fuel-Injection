/*
 * RunningData.cpp
 *
 *  Created on: 13 Jul. 2019
 *      Author: apaml
 */

#include "RunningData.h"

#define uS_IN_MINUTE 60000000.0L
#define SECONDSTIME   1000L

_RunningData::_RunningData()
{	
	_RPM = 0; // 0-16,00
	_calculate_RPM_Flag = false;
	_engine_running_time = 0;
	_engineIsRunning = ENGINE_STALLED;
}

bool _RunningData::CalcRpm(const uint32_t RPM_Count)
{

	bool returnVal = false;

	if (!Triggers.getTriggerFlag())
	{
		_RPM = 0;
		_engineIsRunning = ENGINE_STALLED;
		return returnVal;
	}

	if (_calculate_RPM_Flag)
	{

		_calculate_RPM_Flag = false;

		float tempVal = (uS_IN_MINUTE / RPM_Count); //Calc RPM based on last full revolution time

		noInterrupts();

		if (tempVal < STALL_RPM)
		{
			_engineIsRunning = ENGINE_STALLED;
			tempVal = 0;
		}
		else if (tempVal >= STALL_RPM && tempVal <= Triggers.getCrankingRpm())
		{
			_engineIsRunning = ENGINE_CRANKING;
		}
		else
		{
			_engineIsRunning = ENGINE_RUNNING;
			returnVal = true;
		}
	
		_RPM = tempVal;
		interrupts();
	}
	return returnVal;
}

//----------------------------------------------------------------------------------------
uint16_t _RunningData::GetRPM(void)
{

	return _RPM;
}
//----------------------------------------------------------------------------------------
void _RunningData::SetCalcRpmFlag()
{

	_calculate_RPM_Flag = true;
}

//----------------------------------------------------------------------------------------

uint32_t _RunningData::GetEngineRunningTime()
{
	return _engine_running_time;
}

//----------------------------------------------------------------------------------------
int16_t _RunningData::GetEngineState(void)
{

	return _engineIsRunning;
}
//----------------------------------------------------------------------------------------
void _RunningData::ClockEngineRunningTime(void)
{

	if (_loopClock >= SECONDSTIME)
	{

		_loopClock = _loopClock - SECONDSTIME; // every 100 ms

		if (_engineIsRunning == ENGINE_RUNNING)
		{

			_engine_running_time++;
		}

		else
		{
			_engine_running_time = 0;
		}
	}
}
//----------------------------------------------------------------------------------------
void _RunningData::ClockMainLoopCounter(void)
{

	_mainLoopCounter++;

	if (_loopCounter >= SECONDSTIME)
	{ // once every second
		_loopCounter = _loopCounter - SECONDSTIME;

		_loopsPerSecond = _mainLoopCounter;
		_mainLoopCounter = 0;
	}
}
//----------------------------------------------------------------------------------------

uint16_t _RunningData::GetMainLoopCounter(void)
{

	return _loopsPerSecond / 1000;
}
//----------------------------------------------------------------------------------------
