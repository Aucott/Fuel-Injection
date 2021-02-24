/*
 * Ignition.cpp
 *
 *  Created on: 3 Apr. 2019
 *      Author: apaml
 */

#include "Ignition.h"

_Ignition::_Ignition(_Array3D *ignAngleMap, _Array3D *crankAngleMap,
		_Array3D *ignCltCorrectionMap, _Array3D *ignMatCorrectionMap,
		_Array3D *ignDwellMap, _Array3D *ignIdleTrimMap) {

	_ignAngleMap = ignAngleMap;
	_crankAngleMap = crankAngleMap;
	_ignCltCorrectionMap = ignCltCorrectionMap;
	_ignMatCorrectionMap = ignMatCorrectionMap;
	_ignDwellMap = ignDwellMap;
	_ignIdleTrimMap = ignIdleTrimMap;
	_sampleTime = 306;

	//_IgnMasterTrim = 0; //  >>>>>>>>>> degrees x 100
	//_UseWastedSpark = false;
	//_BaseIgnAngle = 0;
	//_Locked_Timing_Angle = 0;
	//_Locked_Timing_Enable = 0;

}

//----------------------------------------------------------------------------------------

void _Ignition::CalcIgnAngle(void) {

	if (_calcNow) { // not ready yet return
		_calcNow = false;

		_rawIgnAngle = _ignAngleMap->GetFrom3dMap(RunningData.GetRPM(),
				MapSensor.getMap());
		// Angles are x 10
		// check for correction from the soft rev limiter
		int16_t tempAngle = RpmLimiter.ignAngleCorrection(
				_rawIgnAngle + _cltCorrection + _matCorrection + _idleIgnTrim);

		tempAngle = constrain(tempAngle, MIN_IGN_ADVANCE, MAX_IGN_ADVANCE);	//Sanity check

		noInterrupts()
		;
		//_correctedIgnAngle = 1800 - tempAngle;
		_correctedIgnAngle =  3600 - tempAngle;
		interrupts()
		;
	}

	return;
}
//----------------------------------------------------------------------------------------
void _Ignition::CalcCrankingIgnAngle(const int16_t cltValue) {

	_crankingIgnAngle = 1800 - _crankAngleMap->GetFrom2dMapX(cltValue, 1);

	return;
}

//----------------------------------------------------------------------------------------
void _Ignition::CalcCltCorrection(const int16_t cltValue) {

	_cltCorrection = _ignCltCorrectionMap->GetFrom2dMapX(cltValue, 1);
}

//----------------------------------------------------------------------------------------
void _Ignition::CalcMatCorrection(const int16_t matValue) {

	_matCorrection = _ignMatCorrectionMap->GetFrom2dMapX(matValue, 1);
}
//----------------------------------------------------------------------------------------

void _Ignition::CalcDwell(const uint16_t battVolt) {

	float	maxDwell = 0;

	if (IgnSettings.Mode == DISTRIBUTOR) {

			maxDwell = (Cyl_360_degree_Time / 2) - 200;
	}
	if (IgnSettings.Mode == WASTED_SPARK) {

		maxDwell = (Cyl_360_degree_Time )-200;
	}

	float dwell = _ignDwellMap->GetFrom2dMapX(battVolt, 1);

	if (dwell >= maxDwell) {
		dwell = maxDwell;
	}

	cli()
	;
	_IgnDwellMicros = dwell;
	sei()
	;

}

//----------------------------------------------------------------------------------------

void _Ignition::CalcIdleIgnTrim(const uint16_t idleError) {

	_idleIgnTrim = _ignIdleTrimMap->GetFrom2dMapX(idleError, 1);
}

//----------------------------------------------------------------------------------------
void _Ignition::DoIgnCalcs(const int16_t cltValue, const int16_t matValue,
		const uint16_t battVolt, const uint16_t idleError) {

	if (_clock >= _sampleTime) {	// do once every sample time
		_clock = _clock - _sampleTime;

		CalcCrankingIgnAngle(cltValue);
		CalcCltCorrection(cltValue);
		CalcMatCorrection(matValue);
		CalcDwell(battVolt);
		CalcIdleIgnTrim(idleError);
	}
}
//----------------------------------------------------------------------------------------
int16_t _Ignition::Load(void) {

	int16_t err;
	HWSERIAL.print("Loading IGN Settings ->");
	err = eRAM.f_open(_filePtr, _fName, SPIFFS_RDONLY);
	if (err < 0) {

		HWSERIAL.println("err open");
		return err;

	}
	err = eRAM.f_read(_filePtr, reinterpret_cast<const char *>(&IgnSettings),
			SETTINGSIZE);
	if (err < 0) {
		HWSERIAL.println("err Read");
		return err;

	}
	eRAM.f_close(_filePtr);

	if (!crcCheckSettings()) {
		HWSERIAL.print("crc error ");
		HWSERIAL.println(_fName);
	} else {

		HWSERIAL.println("IGN Load Ok");
	}

	return 1; // all is ok
}
// ---------------------------------------------------------------------------
int16_t _Ignition::Save(void) {

	int16_t err;

	HWSERIAL.print("Saving Ign Settings -> ");
	IgnSettings.Crc = CRC16.ccitt(reinterpret_cast<uint8_t *>(&IgnSettings), //Calculate CRC
							   sizeof(IgnSettings) - WORDSIZE);

	err = eRAM.f_open(_filePtr, _fName,
	SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR);
	if (err < 0) {
		HWSERIAL.println("err Open");
		return err;
	}
	err = eRAM.f_write(_filePtr, reinterpret_cast<const char *>(&IgnSettings),
			SETTINGSIZE);
	if (err < 0) {
		HWSERIAL.println("err Write");
		return err;
	}
	eRAM.f_close(_filePtr);
	HWSERIAL.println("IGN Save OK");
	return 1; // all is ok
}
// ---------------------------------------------------------------------------
bool _Ignition::crcCheckSettings(void) {

	if (IgnSettings.Crc
			!= CRC16.ccitt(reinterpret_cast<uint8_t*>(&IgnSettings), //Calculate CRC
					SETTINGSIZE - WORDSIZE)) {
		return false;
	}

	return true; // all is ok
}

// ---------------------------------------------------------------------------

