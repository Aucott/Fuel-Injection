/*
 * Boost.cpp
 *
 *  Created on: 20 Aug. 2019
 *      Author: apaml
 */

#include "Boost.h"

#define MINDUTY 0
#define MAXDUTY 100

_BoostControl::_BoostControl()
{

	//_minPidDuty = 50;
	//_maxPidDuty = 50;
	//_Kp = 50;
	//_Ki = 0;
	//_Kd = 0;
	//_closedLoopEnable = false;
	//_useDualBoostMaps = false;
}

//----------------------------------------------------------------------------------

_BoostControl::_BoostControl(_Array3D *Target1Map, _Array3D *Target2Map,
							 _Array3D *DutyMap)
{

	Target1MapPtr = Target1Map;
	Target2MapPtr = Target2Map;
	DutyCycleMapPtr = DutyMap;
	return;
}
//----------------------------------------------------------------------------------
void _BoostControl::begin(void)
{

	BoostControlPID.SetUp(&_currentBoost, &_PidDuty, &_targetBoost, BoostSettings.Boost_Kp / 100.0, BoostSettings.Boost_Ki / 100.0,
						  BoostSettings.Boost_Kd / 100.0, P_ON_E, DIRECT);
	BoostControlPID.SetSampleTime(200); //FREQUENCY 10 HERTZ
	BoostControlPID.SetOutputLimits(0 - (BoostSettings.Boost__minPidDuty / 10.0),
									BoostSettings.Boost_maxPidDuty / 10.0);
	BoostControlPID.SetMode(BoostSettings.Boost_closedLoopEnable);
}
//----------------------------------------------------------------------------------
void _BoostControl::calcBoostDuty(uint16_t Rpm, float Tps, uint16_t boost)
{

	_Array3D *TargetMapPtr;
	float unClippedDuty;

	_currentBoost = boost; //

	if (BoostSettings.Boost_useDualBoostMaps && true)
	{ //TODO assign to a pin external switch
		TargetMapPtr = Target2MapPtr;
		_mapActive = 1;
	}
	else
	{
		TargetMapPtr = Target1MapPtr;
		_mapActive = 0;
	}

	_targetBoost = TargetMapPtr->GetFrom3dMap(Rpm, Tps);

	_boostPressureError = _currentBoost - _targetBoost;
	_rawBaseDuty = DutyCycleMapPtr->GetAxisFrom3dMap(Rpm, _targetBoost);

	if (BoostControlPID.GetMode())
	{
		BoostControlPID.Compute();

		unClippedDuty = _rawBaseDuty + _PidDuty; //offset PID to add or subtract around _baseDutyCycle
	}
	else
	{

		unClippedDuty = _rawBaseDuty;
	}

	if (unClippedDuty < MINDUTY)
	{
		_wasteGateDuty = MINDUTY;
	}
	else if (unClippedDuty > MAXDUTY)
	{
		_wasteGateDuty = MAXDUTY;
	}
	else
	{
		_wasteGateDuty = unClippedDuty;
	}
	return;
}
//----------------------------------------------------------------------------------

void _BoostControl::setMinDutyCycle(uint16_t minPidDuty)
{
	noInterrupts();
	BoostSettings.Boost__minPidDuty = minPidDuty;
	interrupts();

	return;
}

uint16_t _BoostControl::getMinDutyCycle()
{
	return BoostSettings.Boost__minPidDuty;
}

//----------------------------------------------------------------------------------
void _BoostControl::setMaxDutyCycle(uint16_t maxPidDuty)
{

	noInterrupts();
	BoostSettings.Boost_maxPidDuty = maxPidDuty;
	interrupts();

	return;
}

uint16_t _BoostControl::getMaxDutyCycle()
{
	return BoostSettings.Boost_maxPidDuty;
}

//----------------------------------------------------------------------------------
void _BoostControl::setKp(uint16_t Kp)
{
	//framLow.writeWord(BOOST_KP_FRAM, Kp);
	noInterrupts();
	BoostSettings.Boost_Kp = Kp;
	interrupts();

	BoostControlPID.SetTunings(BoostSettings.Boost_Kp / 100.0, BoostSettings.Boost_Ki / 100.0, BoostSettings.Boost_Kd / 100.0);
	return;
}
uint16_t _BoostControl::getKp(void)
{

	return BoostSettings.Boost_Kp;
}
//----------------------------------------------------------------------------------
void _BoostControl::setKi(uint16_t Ki)
{

	//framLow.writeWord(BOOST_KI_FRAM, Ki);
	noInterrupts();
	BoostSettings.Boost_Ki = Ki;
	interrupts();

	BoostControlPID.SetTunings(BoostSettings.Boost_Kp / 100.0, BoostSettings.Boost_Ki / 100.0, BoostSettings.Boost_Kd / 100.0);
	return;
}
uint16_t _BoostControl::getKi(void)
{

	return BoostSettings.Boost_Ki;
}
//----------------------------------------------------------------------------------
void _BoostControl::setKd(uint16_t Kd)
{

	noInterrupts();
	BoostSettings.Boost_Kd = Kd;
	interrupts();

	BoostControlPID.SetTunings(BoostSettings.Boost_Kp / 100.0, BoostSettings.Boost_Ki / 100.0, BoostSettings.Boost_Kd / 100.0);
	return;
}
uint16_t _BoostControl::getKd(void)
{

	return BoostSettings.Boost_Kd;
}
//----------------------------------------------------------------------------------

void _BoostControl::setClosedLoopEnable(uint16_t closedLoopEnable)

{

	noInterrupts();
	BoostControlPID.SetMode(closedLoopEnable);
	interrupts();
}

uint16_t _BoostControl::getClosedLoopEnable(void)

{
	return BoostControlPID.GetMode();
}

//----------------------------------------------------------------------------------
void _BoostControl::setUseDualBoostMaps(uint16_t useDualBoostMaps)
{

	BoostSettings.Boost_useDualBoostMaps = useDualBoostMaps;
}

uint16_t _BoostControl::getUseDualBoostMaps(void)
{

	return BoostSettings.Boost_useDualBoostMaps;
}

//----------------------------------------------------------------------------------
uint16_t _BoostControl::getRawBaseDuty(void)
{
	return _rawBaseDuty * 10;
}

//----------------------------------------------------------------------------------
uint16_t _BoostControl::getActiveMap(void)
{
	return _mapActive;
}
//----------------------------------------------------------------------------------

int16_t _BoostControl::getBoostPressureError(void)
{
	return _boostPressureError;
}
//----------------------------------------------------------------------------------

uint16_t _BoostControl::getTargetBoost()
{
	return _targetBoost;
}

//----------------------------------------------------------------------------------
uint16_t _BoostControl::getWasteGateDuty()
{
	return _wasteGateDuty * 10;
}

//----------------------------------------------------------------------------------
int16_t _BoostControl::getPidDuty()
{
	return _PidDuty * 10;
}

//----------------------------------------------------------------------------------------

int16_t _BoostControl::Load(void)
{

	int16_t err;
	HWSERIAL.print("Loading Boost Settings -> ");
	err = eRAM.f_open(_filePtr, _fName, SPIFFS_RDONLY);
	if (err < 0)
	{

		HWSERIAL.println("err open");
		return err;
	}
	err = eRAM.f_read(_filePtr, reinterpret_cast<const char *>(&BoostSettings),
					  sizeof(BoostSettings));
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

		HWSERIAL.println("Boost Load Ok");
	}

	return 1; // all is ok
}
// ---------------------------------------------------------------------------
int16_t _BoostControl::Save(void)
{

	int16_t err;

	HWSERIAL.print("Saving Boost Settings -> ");

	BoostSettings.Crc = CRC16.ccitt(reinterpret_cast<uint8_t *>(&BoostSettings), //Calculate CRC
									   sizeof(BoostSettings) - WORDSIZE);

	err = eRAM.f_open(_filePtr, _fName,
					  SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR);
	if (err < 0)
	{
		HWSERIAL.println("err Open");
		return err;
	}
	err = eRAM.f_write(_filePtr, reinterpret_cast<const char *>(&BoostSettings),
					   sizeof(BoostSettings));
	if (err < 0)
	{
		HWSERIAL.println("err Write");
		return err;
	}
	eRAM.f_close(_filePtr);

	HWSERIAL.println("Boost Save OK");

	return 1; // all is ok
}
// ---------------------------------------------------------------------------
bool _BoostControl::crcCheckSettings(void)
{

	if (BoostSettings.Crc != CRC16.ccitt(reinterpret_cast<uint8_t *>(&BoostSettings), //Calculate CRC
										  sizeof(BoostSettings) - WORDSIZE))
	{
		return false;
	}

	return true; // all is ok
}

// ---------------------------------------------------------------------------
