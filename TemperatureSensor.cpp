/*
 * TemperatureSensor.cpp
 *
 *  Created on: 26 Aug. 2019
 *      Author: apaml
 */

#include "TemperatureSensor.h"

//----------------------------------------------------------------------------------
Temperature_Sensor::Temperature_Sensor(_Array3D *MapPtr) {
	_sampleTime = 305;
	_MapPtr = MapPtr;
	//_temperatureFilter.SetKalmanFilter(100, 2, 0.1);

}
//----------------------------------------------------------------------------------
void Temperature_Sensor::CalcTemperature(void) {

	if (_clock >= _sampleTime) {		// do once every sample time
		_clock = _clock - _sampleTime;
		uint16_t R2 = (_rawAdc * R1) / (Vin - _rawAdc);	// Calculate resistance given 5V supply and 2490R pull-up resistor

		_Temperature = _MapPtr->GetFrom2dMap(R2);

	}
}
//----------------------------------------------------------------------------------
void Temperature_Sensor::AddValue(uint16_t adcValue) {

	if (adcValue < SHORT_GND) {
		Errors.setError(ERR_CLT_SHORT);//Coolant sensor shorted
	}
	if (adcValue > SHORT_PWR) {
		Errors.setError(ERR_CLT_GND);
	}


	_rawAdc = ADC_FILTER(adcValue, ADCFILTER_TEMP, _rawAdc);//      _temperatureFilter.updateEstimate(adcValue);

	//_rawAdc =   _temperatureFilter.updateEstimate(adcValue);
}
//----------------------------------------------------------------------------------
int16_t Temperature_Sensor::GetTemperature(void) {


	return _returnVal;
		//return _Temperature;

}
//----------------------------------------------------------------------------------
uint16_t Temperature_Sensor::GetRawADC(void) {

	return _rawAdc;

}

//----------------------------------------------------------------------------------
