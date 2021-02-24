/*
 * Limits.cpp
 *
 *  Created on: 27 Apr. 2020
 *      Author: apaml
 */

#include "BoostCut.h"

_BoostCut::_BoostCut()
{

	//_useIgnitionCut = true;
	//_useFuelCut = true;
	//Boost
	//_overBoostDelayed = 2000; //kPa x 10
	//_overBoostInstant = 2500;
}

//----------------------------------------------------------------------------------
// There are two thresholds; one is the instantaneous TABLE,
//if you go above this then the protection is triggered immediately.
//The other is a one-second value, where the TABLE has to exceed this for 1 second to trigger the protection.
//This allows for short term spike
void _BoostCut::checkBoostLimits(uint16_t mapValue)
{

	//_overBoostDelayed =_overBoostInstant -200;

	if (mapValue < BoostCutSettings.overBoostDelayed)
	{
		_overBoostFlag = false;
		_delayedOverBoostFlag = false;
		return;
	}

	if (mapValue > BoostCutSettings.overBoostInstant)
	{
		_overBoostFlag = true;
		return;
	}

	// Here we are between _overBoostDelayed and _overBoostInstant
	if (!_delayedOverBoostFlag)
	{
		_delayedOverBoostFlag = true; // if timer hasn't been started start it
		_overboostTimer = 0;
	}

	if (_delayedOverBoostFlag && (_overboostTimer > _overboostLimitTime))
	{ // has been overboosting for a second so boost cut
		_overBoostFlag = true;
	}
}

//----------------------------------------------------------------------------------------

int16_t _BoostCut::Load(void)
{

	int16_t err;
	HWSERIAL.print("Loading BoostCut Settings -> ");
	err = eRAM.f_open(_filePtr, _fName, SPIFFS_RDONLY);
	if (err < 0)
	{

		HWSERIAL.println("err open");
		return err;
	}
	err = eRAM.f_read(_filePtr, reinterpret_cast<const char *>(&BoostCutSettings),
					  sizeof(BoostCutSettings));
	if (err < 0)
	{
		HWSERIAL.println("err Read");
		return err;
	}
	eRAM.f_close(_filePtr);

	if (!crcCheckSettings())
	{
		HWSERIAL.print("crc error ");
		HWSERIAL.println(_fName);
	}
	else
	{

		HWSERIAL.println("BoostCut Load Ok");
	}

	return 1; // all is ok
}
// ---------------------------------------------------------------------------
int16_t _BoostCut::Save(void)
{

	int16_t err;

	HWSERIAL.print("Saving BoostCut Settings -> ");

	BoostCutSettings.Crc = CRC16.ccitt(reinterpret_cast<uint8_t *>(&BoostCutSettings), //Calculate CRC
								   sizeof(BoostCutSettings) - WORDSIZE);

	err = eRAM.f_open(_filePtr, _fName,
					  SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR);
	if (err < 0)
	{
		HWSERIAL.println("err Open");
		return err;
	}
	err = eRAM.f_write(_filePtr, reinterpret_cast<const char *>(&BoostCutSettings),
					   sizeof(BoostCutSettings));
	if (err < 0)
	{
		HWSERIAL.println("err Write");
		return err;
	}
	eRAM.f_close(_filePtr);

	HWSERIAL.println("BoostCut Save OK");

	return 1; // all is ok
}
// ---------------------------------------------------------------------------
bool _BoostCut::crcCheckSettings(void)
{

	if (BoostCutSettings.Crc != CRC16.ccitt(reinterpret_cast<uint8_t *>(&BoostCutSettings), //Calculate CRC
											sizeof(BoostCutSettings) - WORDSIZE))
	{
		return false;
	}

	return true; // all is ok
}

// ---------------------------------------------------------------------------
