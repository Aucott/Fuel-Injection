/*
 * Limits.cpp
 *
 *  Created on: 27 Apr. 2020
 *      Author: apaml
 */

#include "RpmLimiter.h"

_RpmLimiter::_RpmLimiter(_Array3D *rpm_Limit_Map)
{

	_rpm_Limit_Map = rpm_Limit_Map;

	//_softLimitIgnAngle = 0; // Angle x 10
	//_additionalRpmToCut = 500; // rpm below hard rev cut
	//_limpHomeRpmLimit = 3000;
	//_useIgnitionCut = true;
	//_useFuelCut = true;
}

void _RpmLimiter::getRpmLimits(int16_t cltValue)
{

	_softRpmLimit = _rpm_Limit_Map->GetFrom2dMapX(cltValue, 1);
	_hardRpmLimit = _softRpmLimit + RpmLimiterSettings.additionalRpmToCut;
}

//----------------------------------------------------------------------------------

void _RpmLimiter::checkRpmLimits(uint16_t Rpm, int16_t cltValue)
{

	getRpmLimits(cltValue);

	if (Rpm >= _hardRpmLimit)
	{							  // check if we have reached the hard rev limit
		_hardRpmLimitFlag = true; // Set Hard cut
	
		return;
	}

	if (Rpm < _softRpmLimit)
	{

		_hardRpmLimitFlag = false; // No hard cut
		_softRpmLimitFlag = false; // No soft cut

		return;
	}

	if (!_softRpmLimitFlag)
	{ // check if we have reached the soft rpm-limit
		_softRpmLimitFlag = true;
		_softRpmLimitTimer = 0; // reset timer to zero >> retard for only n milliseconds
	}

	if (_softRpmLimitFlag && (_softRpmLimitTimer > _softRpmLimitTime))
	{
		_hardRpmLimitFlag = true; // reached n milliseconds so hard-cut
		
	}
}

//----------------------------------------------------------------------------------

int16_t _RpmLimiter::ignAngleCorrection(int16_t ignAdvance)
{

	uint16_t ignSoftRevValue = ignAdvance;
	if (_softRpmLimitFlag)
		ignSoftRevValue = RpmLimiterSettings.softLimitIgnAngle; // correct timing for soft rev limit

	return ignSoftRevValue;
}

//----------------------------------------------------------------------------------------

int16_t _RpmLimiter::Load(void)
{

	int16_t err;
	HWSERIAL.print("Loading RpmL Settings -> ");
	err = eRAM.f_open(_filePtr, _fName, SPIFFS_RDONLY);
	if (err < 0)
	{

		HWSERIAL.println("err open");
		return err;
	}
	err = eRAM.f_read(_filePtr, reinterpret_cast<const char *>(&RpmLimiterSettings),
					  sizeof(RpmLimiterSettings));
	if (err < 0)
	{
		HWSERIAL.println("err Read");
		return err;
	}
	eRAM.f_close(_filePtr);

	if (!crcCheckSettings())
	{
		HWSERIAL.print("RpmL crc error ");
		
	}
	else
	{

		HWSERIAL.println("RpmL Load Ok");
	}

	return 1; // all is ok
}
// ---------------------------------------------------------------------------
int16_t _RpmLimiter::Save(void)
{

	int16_t err;

	HWSERIAL.print("Saving RpmL Settings -> ");

	RpmLimiterSettings.Crc = CRC16.ccitt(reinterpret_cast<uint8_t *>(&RpmLimiterSettings), //Calculate CRC
										 sizeof(RpmLimiterSettings) - WORDSIZE);

	err = eRAM.f_open(_filePtr, _fName,
					  SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR);
	if (err < 0)
	{
		HWSERIAL.println("err Open");
		return err;
	}
	err = eRAM.f_write(_filePtr, reinterpret_cast<const char *>(&RpmLimiterSettings),
					   sizeof(RpmLimiterSettings));
	if (err < 0)
	{
		HWSERIAL.println("err Write");
		return err;
	}
	eRAM.f_close(_filePtr);

	HWSERIAL.println("RpmL Save OK");

	return 1; // all is ok
}
// ---------------------------------------------------------------------------
bool _RpmLimiter::crcCheckSettings(void)
{

	if (RpmLimiterSettings.Crc != CRC16.ccitt(reinterpret_cast<uint8_t *>(&RpmLimiterSettings), //Calculate CRC
											  sizeof(RpmLimiterSettings) - WORDSIZE))
	{
		return false;
	}

	return true; // all is ok
}

// ---------------------------------------------------------------------------
