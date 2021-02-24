/*
 * Battery.h
 *
 *  Created on: 26 Aug. 2019
 *      Author: apaml
 */

#ifndef BATTERY_H_
#define BATTERY_H_

#include "Arduino.h"
#include "defines.h"
#include <FastCRC.h>


#define MAX_ADC_VALUE 	0XFFF
#define ADCFILTER_BAT	128
 #define fastMap12bitToX(x, out_max) ( ((unsigned long)x * out_max) >> 12)

class _Battery {

public:
	_Battery();

	void AddValue(uint16_t);
	uint16_t GetVoltage(void);
	void CalcVoltage(void);

private:
	uint16_t _BAT_VOLT;
	uint16_t _rawBattVolt;
	uint16_t  _crcCheck;

	uint16_t _sampleTime;
	elapsedMillis _clock;

};

extern _Battery Battery;


#endif /* BATTERY_H_ */
