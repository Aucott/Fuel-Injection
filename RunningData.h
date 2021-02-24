/*
 * RunningData.h
 *
 *  Created on: 23 Mar. 2019
 *      Author: apaml
 */

#ifndef RUNNINGDATA_H_
#define RUNNINGDATA_H_

#include "Arduino.h"
#include "Settings.h"
#include "Triggers.h"
#include "errors.h"
//#include <FastCRC.h>
#include "defines.h"

class _RunningData
{
public:
	_RunningData();

	//----------------------------------------------------------------------------------------
	void processCommand(const rxData Rxdata)
	{		
		
			HWSERIAL.println("Here in Running Data");		
		
	}

	//----------------------------------------------------------------------------------------

	bool IsEngineCranking(void)
	{

		return (_engineIsRunning == ENGINE_CRANKING);
	}
	//----------------------------------------------------------------------------------------
	bool IsEngineRunning(void)
	{

		return (_engineIsRunning == ENGINE_RUNNING);
	}
	//----------------------------------------------------------------------------------------
	bool IsEngineStalled(void)
	{

		return (_engineIsRunning == ENGINE_STALLED);
	}
	//----------------------------------------------------------------------------------------

	bool CalcRpm(const uint32_t);
	uint16_t GetRPM(void);

	void SetCalcRpmFlag();
	uint32_t GetEngineRunningTime();
	float GetAFR();
	int16_t GetEngineState(void);
	void ClockEngineRunningTime(void);
	void ClockMainLoopCounter(void);
	uint16_t GetMainLoopCounter(void);

private:	

	volatile uint16_t _RPM; // 0-16,00
	bool _calculate_RPM_Flag;
	uint32_t _engine_running_time;
	int16_t _engineIsRunning;
	volatile uint32_t _mainLoopCounter;
	volatile uint32_t _loopsPerSecond;

		
	elapsedMillis _loopCounter;
	elapsedMillis _loopClock;
};

extern _RunningData RunningData;
#endif /* RUNNINGDATA_H_ */
