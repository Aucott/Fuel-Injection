/*
 * TriggerTimers.cpp
 *
 *  Created on: 13 Jul. 2019
 *      Author: apaml
 */

#include "TriggerTimers.h"

_TriggerTimer::_TriggerTimer() {
	// TODO Auto-generated constructor stub
	_SyncFlag = false;
	_TriggerFlag = false;
	_triggerCounter = 0;
}
//----------------------------------------------------------------------------------------
void _TriggerTimer::ClockTriggerCounter(void) {
	_triggerCounter++;
	if (_triggerCounter > 3)
		_TriggerFlag = true;
}
//----------------------------------------------------------------------------------------
void _TriggerTimer::CheckTimeOuts(void) {

	if (_SyncMillis > SYNC_TRIGGER_TIMEOUT) {
		_SyncFlag = false;
		_SyncMillis = 0;
	}

	if (_TriggerMillis > CRANK_TRIGGER_TIMEOUT) {
		_TriggerFlag = false;
		_triggerCounter = 0;
		_TriggerMillis = 0;
	}
}
//----------------------------------------------------------------------------------------
