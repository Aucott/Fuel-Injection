/*
 * TemperatureSensor.h
 *
 *  Created on: 26 Aug. 2019
 *      Author: apaml
 */

#ifndef TEMPERATURESENSOR_H_
#define TEMPERATURESENSOR_H_

#include "Arduino.h"
#include "ICosworth.h"



#define R1  2490
#define Vin  0X0FFF
#define ADCFILTER_TEMP 180


class Temperature_Sensor {
public:

	explicit Temperature_Sensor(_Array3D *);

	void SetReturnVal(uint16_t returnVal){
		_returnVal = returnVal;
		return;

	}

	uint16_t GetRawADC(void);
	int16_t GetTemperature(void);
	void AddValue(uint16_t adcValue);
	void CalcTemperature(void);

private:
	_Array3D *_MapPtr;
	int16_t _Temperature;
	uint16_t _rawAdc;
	//SimpleKalmanFilter _temperatureFilter;

	uint16_t _sampleTime;
	elapsedMillis _clock;

	uint16_t _returnVal;

};

extern Temperature_Sensor CLT_Sensor;
extern Temperature_Sensor MAT_Sensor;

#endif /* TEMPERATURESENSOR_H_ */
