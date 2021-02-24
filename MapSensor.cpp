/*
 * MapSensor.cpp
 *
 *  Created on: 31 May 2019
 *      Author: apaml
 */

#include "MapSensor.h"

_MapSensor::_MapSensor()
{
}
//----------------------------------------------------------------------------------------
_MapSensor::_MapSensor(_Array3D *MapPtr, _Array3D *PreMapPtr)
{

	_MapPtr = MapPtr;
	_PredictedMapPtr = PreMapPtr;

	MapSettings.Map_PredictionEnabled = false;
	MapSettings.Map_PredictionTransitionTime = 500;
	MapSettings.Map_AdaptiveMapPredictionEnabled = false;

	_predictedMapValue = 0;
	_atmosphericPressure = 1013; //x10
}

//----------------------------------------------------------------------------------------
void _MapSensor::addValue(uint16_t adcValue)
{

	if (adcValue < SHORT_GND)
	{
		Errors.setError(ERR_MAP_LOW);
	}
	if (adcValue > SHORT_PWR)
	{
		Errors.setError(ERR_MAP_HIGH);
	}

	if ((adcValue < VALID_MAP_MAX) && (adcValue > VALID_MAP_MIN))
	{
		_rawMapADC = ADC_FILTER(adcValue, ADCFILTER_MAP, _rawMapADC);

		if (_MAPcount >= 0xFFFF)
		{ //prevent _accumulator overflow
			_MAPrunningValue = adcValue;
			_MAPcount = 1;
		}
		_MAPrunningValue += _rawMapADC;
		_MAPcount++;
	}
}

//----------------------------------------------------------------------------------------

void _MapSensor::zero(void)
{

	_MAPrunningValue = 0;
	_MAPcount = 0;
}
//----------------------------------------------------------------------------------------
uint16_t _MapSensor::getMap(void)
{
	return _returnVal; //TODO
					   //return _MAP;
}
//----------------------------------------------------------------------------------------

uint16_t _MapSensor::getRawADC(void)
{

	return _rawMapADC;
}

//----------------------------------------------------------------------------------------
uint16_t _MapSensor::getAverageADC(void)
{

	return _averageMapADC;
}

//----------------------------------------------------------------------------------------
void _MapSensor::setCalcFlag(void)
{

	_calcNow = true;
}
//----------------------------------------------------------------------------------------
void _MapSensor::calcInstantaneousMAP(void)
{

	_MAP = _MapPtr->GetFrom2dMap(_rawMapADC);
}
//----------------------------------------------------------------------------------------

uint16_t _MapSensor::getUnfilteredMap(void)
{

	return _MapPtr->GetFrom2dMap(_rawMapADC);
}
//----------------------------------------------------------------------------------------

uint16_t _MapSensor::getPredictedMapValue(void)
{

	return _predictedMapValue;
}

//----------------------------------------------------------------------------------------

bool _MapSensor::calcMap(const uint16_t rpmValue, const uint16_t TpsValue,
						 const int16_t engineStatus)
{

	bool returnVal = false;

	if (engineStatus != ENGINE_RUNNING)
	{
		//Calc_InstantaneousMAP(); \\TODO
		return returnVal;
	}

	if (_calcNow)
	{ //Tdc trigger so average up 180deg of map values
		returnVal = true;
		if ((_MAPrunningValue != 0) && (_MAPcount != 0))
		{

			uint16_t aMap = 0;
			uint16_t pMap = 0;
			_calcNow = false;

			if (_mapPredictionTimer > MapSettings.Map_PredictionTransitionTime)
			{								//check time limit for predicted map hasn't expired
				_mapPredictionFlag = false; //has expired so set flag
			}

			if (_mapPredictionFlag && true)
			{ //_mapPredictionEnabled) {		TODO	//if predicted map flag is set

				pMap = _PredictedMapPtr->GetFrom3dMap(rpmValue, TpsValue); //set the predicted map value
			}

			_predictedMapValue = pMap;

			_averageMapADC = _MAPrunningValue / _MAPcount;
			aMap = _MapPtr->GetFrom2dMap(_averageMapADC); //average

			zero(); //reset map averaging

			_MAP = (aMap > pMap) ? aMap : pMap; // assign largest to map
		}
	}

	return returnVal;
}
//----------------------------------------------------------------------------------------
void _MapSensor::useMapPrediction(void)
{

	_mapPredictionTimer = 0;
	_mapPredictionFlag = true;
	return;
}
//----------------------------------------------------------------------------------------
bool _MapSensor::isMapPredictionRunning(void)
{

	return _mapPredictionFlag;
}
//----------------------------------------------------------------------------------------

//returns true if Map is over atmospheric pressure
bool _MapSensor::isBoosting(void)
{

	if (_MAP >= _atmosphericPressure)
	{
		_is_Boosting = true;
	}

	if ((_MAP < (_atmosphericPressure - _boostHystheresis)) && _is_Boosting)
	{
		_is_Boosting = false;
	}

	return _is_Boosting;
}

//----------------------------------------------------------------------------------------

void _MapSensor::setMapPredictionEnable(uint16_t mapPredictionEnabled)
{

	noInterrupts();
	MapSettings.Map_PredictionEnabled = mapPredictionEnabled;
	interrupts();
}
uint16_t _MapSensor::getMapPredictionEnable(void)
{
	return MapSettings.Map_PredictionEnabled;
}

//----------------------------------------------------------------------------------------
void _MapSensor::setAdaptiveMapPredictionEnable(
	uint16_t adaptiveMapPredictionEnabled)
{

	noInterrupts();
	MapSettings.Map_AdaptiveMapPredictionEnabled = adaptiveMapPredictionEnabled;
	interrupts();
}
uint16_t _MapSensor::getAdaptiveMapPredictionEnable(void)
{
	return MapSettings.Map_AdaptiveMapPredictionEnabled;
}
//----------------------------------------------------------------------------------------
void _MapSensor::setMapPredictionTransitionTime(
	uint16_t mapPredictionTransitionTime)
{

	noInterrupts();
	MapSettings.Map_PredictionTransitionTime = mapPredictionTransitionTime;
	interrupts();
}
uint16_t _MapSensor::getMapPredictionTransitionTime(void)
{
	return MapSettings.Map_PredictionTransitionTime;
}

//----------------------------------------------------------------------------------------
void _MapSensor::setAtmosphericPressure(uint16_t atmosphericPressure)
{

	noInterrupts();
	_atmosphericPressure = atmosphericPressure;
	interrupts();
}

uint16_t _MapSensor::getAtmosphericPressure(void)
{
	return _atmosphericPressure;
}

//----------------------------------------------------------------------------------------
/*
 * The highest sea-level pressure on Earth occurs in Siberia, where the Siberian High often attains a sea-level pressure above 105 kPa;
 * with record highs close to 108.5 kPa.
 * The lowest measurable sea-level pressure is found at the centers of tropical cyclones and tornadoes, with a record low of 87 kPa;
 */
void _MapSensor::setAtmosphericPressure(void)
{
	calcInstantaneousMAP();

	if ((_MAP >= BARO_MIN) && (_MAP <= BARO_MAX))
	{
		setAtmosphericPressure(_MAP);
	}
	else
	{
		setAtmosphericPressure(1013); //default value
	}
}

//----------------------------------------------------------------------------------------

int16_t _MapSensor::Load(void)
{

	int16_t err;
	HWSERIAL.print("Loading Map Settings -> ");


	err = eRAM.f_open(_filePtr, _fName, SPIFFS_RDONLY);
	if (err < 0)
	{

		HWSERIAL.println("err open");
		return err;
	}
	err = eRAM.f_read(_filePtr, reinterpret_cast<const char *>(&MapSettings),
					  sizeof(MapSettings));
	if (err < 0)
	{
		HWSERIAL.println("err Read");
		return err;
	}
	eRAM.f_close(_filePtr);

	if (!crcCheckSettings())
	{
		HWSERIAL.print("Map crc error ");
	
	}
	else
	{

		HWSERIAL.println("Map Load Ok");
	}

	return 1; // all is ok
}
// ---------------------------------------------------------------------------
int16_t _MapSensor::Save(void)
{

	int16_t err;

	HWSERIAL.print("Saving Map Settings -> ");

	MapSettings.Crc = CRC16.ccitt(reinterpret_cast<uint8_t *>(&MapSettings), //Calculate CRC
									   sizeof(MapSettings) - WORDSIZE);


	err = eRAM.f_open(_filePtr, _fName,
					  SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR);
	if (err < 0)
	{
		HWSERIAL.println("err Open");
		return err;
	}
	err = eRAM.f_write(_filePtr, reinterpret_cast<const char *>(&MapSettings),
					   sizeof(MapSettings));
	if (err < 0)
	{
		HWSERIAL.println("err Write");
		return err;
	}
	eRAM.f_close(_filePtr);

	HWSERIAL.println("Map Save OK");

	return 1; // all is ok
}
// ---------------------------------------------------------------------------
bool _MapSensor::crcCheckSettings(void)
{

	if (MapSettings.Crc != CRC16.ccitt(reinterpret_cast<uint8_t *>(&MapSettings), //Calculate CRC
										sizeof(MapSettings) - WORDSIZE))
	{
		return false;
	}

	return true; // all is ok
}

// ---------------------------------------------------------------------------
