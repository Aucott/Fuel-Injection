/*
 * Battery.cpp
 *
 *  Created on: 26 Aug. 2019
 *      Author: apaml
 */

#include "Battery.h"

_Battery::_Battery() {
	_sampleTime = 300;
}

void _Battery::AddValue(uint16_t adcValue) {

	_rawBattVolt = ADC_FILTER(adcValue, ADCFILTER_BAT, _rawBattVolt);
}
//----------------------------------------------------------------------------------
void _Battery::CalcVoltage(void) {

	if (_clock >= _sampleTime) {		// do once every sample time
		_clock = _clock - _sampleTime;

		_BAT_VOLT = fastMap12bitToX(_rawBattVolt,365) + 7;		//37 volts
	}
}
//----------------------------------------------------------------------------------
uint16_t _Battery::GetVoltage(void) {
	return 140;//TODO
	return _BAT_VOLT;
}
//----------------------------------------------------------------------------------
