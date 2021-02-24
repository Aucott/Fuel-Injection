/*
 * TriggerTimers.h
 *
 *  Created on: 13 Jul. 2019
 *      Author: apaml
 */

#ifndef TRIGGERTIMERS_H_
#define TRIGGERTIMERS_H_

#include "Arduino.h"

#define SYNC_TRIGGER_TIMEOUT 	2000
#define CRANK_TRIGGER_TIMEOUT 	1500

#define TRIGGER_FILTER_TIME 50// microseconds

class _TriggerTimer {
public:
	_TriggerTimer();

//----------------------------------------------------------------------------------------
	void ResetTriggerClock(void) {
		_TriggerMillis = 0;
	}
//----------------------------------------------------------------------------------------
	void ResetTriggerCounter(void) {
		_triggerCounter = 0;
	}
//----------------------------------------------------------------------------------------
	void ResetSyncClock(void) {
		_SyncMillis = 0;
	}
//----------------------------------------------------------------------------------------

	void SetSyncFlag(void) {
		_SyncFlag = true;
	}
//----------------------------------------------------------------------------------------
	bool GetSyncFlag(void) {

		return _SyncFlag;
	}
//----------------------------------------------------------------------------------------

	bool GetTriggerFlag(void) {
		return _TriggerFlag;
	}
//---------------------------------------------------------------------------------------

	int16_t GetTriggerCounter(void) {

		return _triggerCounter;
	}
//----------------------------------------------------------------------------------------

	void ClockTriggerCounter(void);
	void CheckTimeOuts(void);

private:

	elapsedMillis _TriggerMillis;
	elapsedMillis _SyncMillis;
	elapsedMillis _Running_Time;
	uint8_t _triggerCounter = 0;
	bool _SyncFlag = false;
	bool _TriggerFlag = false;
};

extern _TriggerTimer TriggerTimer;
#endif /* TRIGGERTIMERS_H_ */
