/*
 * TPS.cpp
 *
 *  Created on: 3 Jun. 2019
 *      Author: apaml
 */

#include "TPS.h"

_TPS::_TPS()
{
}

//----------------------------------------------------------------------------------------

_TPS::_TPS(_Array3D *MapPtr)
{

	_MapPtr = MapPtr;

	//_TpsClosedValue = 500;
	//_TpsOpenValue = 3000;
	//_OpenThrottleThreshold = 900;
	//_ClosedThrottleThreshold = 10;
}
//----------------------------------------------------------------------------------------
void _TPS::AddValue(const uint16_t adcValue)
{

	if (adcValue < SHORT_GND)
	{
		Errors.setError(ERR_TPS_GND); //Coolant sensor shorted
	}
	if (adcValue > SHORT_PWR)
	{
		Errors.setError(ERR_TPS_SHORT); //TPS shorted (Is potentially valid)
	}

	uint16_t filtered_Value;
	uint16_t New_TPS_Percentage;

	uint32_t TPS_time = micros();

	_RawAdc = ADC_FILTER(adcValue, ADCFILTER_TPS, _RawAdc);

	filtered_Value = _RawAdc;

	if (filtered_Value < TPSSettings.Tps_ClosedValue)
		filtered_Value = TPSSettings.Tps_ClosedValue;
	if (filtered_Value > TPSSettings.Tps_OpenValue)
		filtered_Value = TPSSettings.Tps_OpenValue;

	New_TPS_Percentage = map(filtered_Value, TPSSettings.Tps_ClosedValue, //Scalar X 10
							 TPSSettings.Tps_OpenValue, 0, 1000);

	int32_t TPS_change = (New_TPS_Percentage - _tps_Percentage);

	int32_t rateOfChange =
		1000000 * TPS_change / (TPS_time - TPSlast_time); //This is the % per second that the TPS has moved

	TPSlast_time = TPS_time;

	rateOfChange = ADC_FILTER(rateOfChange, DELTA_FILTER, _tps_Delta);

	if (rateOfChange < 20 && rateOfChange > -20) //deadband
		rateOfChange = 0;

	_tps_Delta = rateOfChange; //TPS change per second
	_tps_Percentage = New_TPS_Percentage;
	checkTpsTrigger();
}

//----------------------------------------------------------------------------------------
void _TPS::checkTpsTrigger(void)
{

	_triggerValue = _MapPtr->GetFrom2dMapX(_tps_Percentage, 1);

	if (!MapSensor.isMapPredictionRunning())
	{
		if (_tps_Delta >= _triggerValue)
			MapSensor.useMapPrediction();
	}
}

//----------------------------------------------------------------------------------------
void _TPS::learnClosedThrottle(void)
{
	noInterrupts();
	TPSSettings.Tps_ClosedValue = _RawAdc;
	interrupts();

	sendPacketUSB(TPS_SETTINGS, SET_TPS_CLOSED_VALUE, 0,
				  TpsSensor.GetTpsClosedValue());
}
//----------------------------------------------------------------------------------------
void _TPS::learnOpenThrottle(void)
{
	noInterrupts();
	TPSSettings.Tps_OpenValue = _RawAdc;
	interrupts();

	sendPacketUSB(TPS_SETTINGS, SET_TPS_OPEN_VALUE, 0,
				  TpsSensor.GetTpsOpenValue());
}
//----------------------------------------------------------------------------------------
void _TPS::SetTpsOpenValue(uint16_t TpsOpenValue)
{

	noInterrupts();
	TPSSettings.Tps_OpenValue = TpsOpenValue;
	interrupts();
}

uint16_t _TPS::GetTpsOpenValue(void)
{
	return TPSSettings.Tps_OpenValue;
}
//----------------------------------------------------------------------------------------

void _TPS::SetTpsClosedValue(uint16_t TpsClosedValue)
{

	noInterrupts();
	TPSSettings.Tps_ClosedValue = TpsClosedValue;
	interrupts();
}
uint16_t _TPS::GetTpsClosedValue(void)
{
	return TPSSettings.Tps_ClosedValue;
}

//----------------------------------------------------------------------------------------

void _TPS::SetClosedThottleThreshold(uint16_t ClosedThottleThreshold)
{

	noInterrupts();
	TPSSettings.Tps_ClosedThrottleThreshold = ClosedThottleThreshold;
	interrupts();
}
uint16_t _TPS::GetClosedThottleThreshold(void)
{
	return TPSSettings.Tps_ClosedThrottleThreshold;
}

//----------------------------------------------------------------------------------------

void _TPS::SetOpenThottleThreshold(uint16_t OpenThrottleThreshold)
{

	noInterrupts();
	TPSSettings.Tps_OpenThrottleThreshold = OpenThrottleThreshold;
	interrupts();
}
uint16_t _TPS::GetOpenThottleThreshold(void)
{
	return TPSSettings.Tps_ClosedThrottleThreshold;
}

//----------------------------------------------------------------------------------------
bool _TPS::IsClosedThrottle(void)
{

	bool returnVal = false;
	if (_tps_Percentage <= TPSSettings.Tps_ClosedThrottleThreshold)
		returnVal = true;

	return returnVal;
}
//----------------------------------------------------------------------------------------
bool _TPS::IsWOT(void)
{

	bool returnVal = false;
	if (_tps_Percentage >= TPSSettings.Tps_OpenThrottleThreshold)
		returnVal = true;

	return returnVal;
}

//----------------------------------------------------------------------------------------

uint16_t _TPS::GetTPS(void)
{
	return _tps_Percentage;
}
//----------------------------------------------------------------------------------------
int16_t _TPS::GetTpsDelta(void)
{
	return _tps_Delta;
}
//----------------------------------------------------------------------------------------

uint16_t _TPS::GetTpsTriggerValue(void)
{
	return _triggerValue;
}
//----------------------------------------------------------------------------------------

void _TPS::CheckTpsTrigger(void)
{
	checkTpsTrigger();
}
//----------------------------------------------------------------------------------------
uint16_t _TPS::GetRawADC(void)
{
	return _RawAdc;
}
//----------------------------------------------------------------------------------------

int16_t _TPS::Load(void)
{

	int16_t err;
	HWSERIAL.print("Loading TPS Settings -> ");
	err = eRAM.f_open(_filePtr, _fName, SPIFFS_RDONLY);
	if (err < 0)
	{

		HWSERIAL.println("err open");
		return err;
	}
	err = eRAM.f_read(_filePtr, reinterpret_cast<const char *>(&TPSSettings),
					  sizeof(TPSSettings));
	if (err < 0)
	{
		HWSERIAL.println("err Read");
		return err;
	}
	eRAM.f_close(_filePtr);

	if (!crcCheckSettings())
	{
		HWSERIAL.print("TPS crc error ");
		
	}
	else
	{

		HWSERIAL.println("TPS Load Ok");
	}

	return 1; // all is ok
}
// ---------------------------------------------------------------------------
int16_t _TPS::Save(void)
{

	int16_t err;

	HWSERIAL.print("Saving TPS Settings -> ");

	TPSSettings._Crc = CRC16.ccitt(reinterpret_cast<uint8_t *>(&TPSSettings), //Calculate CRC
								   sizeof(TPSSettings) - WORDSIZE);

	err = eRAM.f_open(_filePtr, _fName,
					  SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR);
	if (err < 0)
	{
		HWSERIAL.println("err Open");
		return err;
	}
	err = eRAM.f_write(_filePtr, reinterpret_cast<const char *>(&TPSSettings),
					   sizeof(TPSSettings));
	if (err < 0)
	{
		HWSERIAL.println("err Write");
		return err;
	}
	eRAM.f_close(_filePtr);

	HWSERIAL.println("TPS Save OK");

	return 1; // all is ok
}
// ---------------------------------------------------------------------------
bool _TPS::crcCheckSettings(void)
{

	if (TPSSettings._Crc != CRC16.ccitt(reinterpret_cast<uint8_t *>(&TPSSettings), //Calculate CRC
										 sizeof(TPSSettings) - WORDSIZE))
	{
		return false;
	}

	return true; // all is ok
}

// ---------------------------------------------------------------------------
