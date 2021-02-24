/*
 * Idle.cpp
 *
 *  Created on: 17 Aug. 2019
 *      Author: apaml
 */

#include "Idle.h"
#define MINDUTY 0
#define MAXDUTY 1000

_Idle::_Idle()
{
	_sampleTime = 1000;
}

_Idle::_Idle(_Array3D *TargetIdleMap, _Array3D *BaseIdleDutyMap,
			 _Array3D *PostCrankIdleMap)
{

	_targetRpmMap = TargetIdleMap;
	_baseIdleDutyMap = BaseIdleDutyMap;
	_postCrankIdleMap = PostCrankIdleMap;
}
void _Idle::Begin(void)
{



	IdlePID.SetUp(&_currentRpm, &_idleEffortPID, &_rawTargetIdleRpm,
				  IdleSettings.Idle_Kp / 100.0,  IdleSettings.Idle_Ki / 100.000,IdleSettings.Idle_Kd / 100.0, P_ON_E, DIRECT);
	IdlePID.SetSampleTime(200); //FREQUENCY 10 HERTZ
	IdlePID.SetOutputLimits(0 - IdleSettings.Idle_minPidDuty, IdleSettings.Idle_maxPidDuty);
	IdlePID.SetMode(MANUAL);


}
//----------------------------------------------------------------------------------
void _Idle::calcIdleDuty(uint16_t Rpm, int16_t EngineIsRunning, float CltTemp,
						 bool ClosedThrottle, uint32_t EngineRunningTime, bool isBoosting)
{
	if (_clock >= _sampleTime)
	{ // do once every sample time
		_clock = _clock - _sampleTime;

		_currentRpm = Rpm; // casts to float for PID

		_rawTargetIdleRpm = _targetRpmMap->GetFrom2dMapX(CltTemp, 1) + ThermalFan.getAddedIdleRpm();

		if (ClosedThrottle && Rpm <= (_rawTargetIdleRpm + 100) && IdleSettings.Idle_closedLoopEnable)
		{

			IdlePID.SetMode(AUTOMATIC);
		}

		if (!ClosedThrottle || (EngineIsRunning != ENGINE_RUNNING))
		{ // Throttle is open no need for closed loop

			IdlePID.SetMode(MANUAL);
		}

		_rawBaseDuty = _baseIdleDutyMap->GetFrom2dMapX(CltTemp, 1);

		_idleEffortThermalFan = (((float)_rawBaseDuty * (float)ThermalFan.getAddedIdleEffort()) / 100.0);

		_idleEffortPostCrank = _postCrankIdleMap->GetFrom3dMap(EngineRunningTime,
															   CltTemp); //additional percentage//TODO change map to have scalar

		_idleEffortOpenLoop = _rawBaseDuty + _idleEffortThermalFan + _idleEffortPostCrank + _idleEffortAirCon + _idleEffortElectrical;

		_targetIdleRpm = _rawTargetIdleRpm;

		_idleRpmError = Rpm - _rawTargetIdleRpm; //Idle RPM error, ie actual RPM minus target idle RPM

		if (IdleSettings.Idle_closedLoopEnable && IdlePID.GetMode())
		{
			IdlePID.Compute();

			_idleEffortUnclipped = _idleEffortOpenLoop + ((_idleEffortPID));
		}
		else
		{

			_idleEffortUnclipped = _idleEffortOpenLoop;
		}

		if (EngineIsRunning != ENGINE_RUNNING)
		{ //Engine is not running open idle valve for start up
			_idleEffortUnclipped = 1000;
		}

		if (isBoosting && IdleSettings.Idle_closeIdleOnBoost)
		{ //Close idle valve on boost
			_idleEffortUnclipped = 0;
		}

		if (_idleEffortUnclipped < MINDUTY)
		{ //Clip values between 0 and 100%
		}
		else if (_idleEffortUnclipped > MAXDUTY)
		{
			_idleEffortDuty = MAXDUTY;
		}
		else
		{
			_idleEffortDuty = _idleEffortUnclipped;
		}
	}

	return;
}
//----------------------------------------------------------------------------------
void _Idle::setMinDutycycle(uint16_t minPidDuty)
{
	//framLow.writeWord(IDLE_MIN_DUTY_FRAM, minPidDuty);
	noInterrupts();

	IdleSettings.Idle_minPidDuty = minPidDuty;

	interrupts();
}

uint16_t _Idle::getMinDutycycle()
{

	return IdleSettings.Idle_minPidDuty;
}
//----------------------------------------------------------------------------------

void _Idle::setMaxDutycycle(uint16_t maxPidDuty)
{
	//framLow.writeWord(IDLE_MAX_DUTY_FRAM, maxPidDuty);
	noInterrupts();

	IdleSettings.Idle_maxPidDuty = maxPidDuty;

	interrupts();
}

uint16_t _Idle::getMaxDutycycle()
{

	return IdleSettings.Idle_maxPidDuty;
}

//----------------------------------------------------------------------------------
void _Idle::setKp(uint16_t Kp)
{

	//framLow.writeWord(IDLE_KP_FRAM, Kp);
	noInterrupts();
	IdleSettings.Idle_Kp = Kp;
	interrupts();
	IdlePID.SetTunings(IdleSettings.Idle_Kp / 100.0, IdleSettings.Idle_Ki / 100.0, IdleSettings.Idle_Kd / 100.0);
	return;
}
uint16_t _Idle::getKp(void)
{

	return IdleSettings.Idle_Kp;
}

//----------------------------------------------------------------------------------
void _Idle::setKi(uint16_t Ki)
{
	//framLow.writeWord(IDLE_KI_FRAM, Ki);
	noInterrupts();
	IdleSettings.Idle_Ki = Ki;
	interrupts();
	IdlePID.SetTunings(IdleSettings.Idle_Kp / 100.0, IdleSettings.Idle_Ki / 100.0, IdleSettings.Idle_Kd / 100.0);
	return;
	
}

uint16_t _Idle::getKi(void)
{

	return IdleSettings.Idle_Ki;
}
//----------------------------------------------------------------------------------
void _Idle::setKd(uint16_t Kd)
{

	//framLow.writeWord(IDLE_KD_FRAM, Kd);
	noInterrupts();
	IdleSettings.Idle_Kd = Kd;
	interrupts();
	IdlePID.SetTunings(IdleSettings.Idle_Kp / 100.0, IdleSettings.Idle_Ki / 100.0, IdleSettings.Idle_Kd / 100.0);
	return;
}

uint16_t _Idle::getKd(void)
{

	return IdleSettings.Idle_Kd;
}
//----------------------------------------------------------------------------------
void _Idle::setClosedLoopEnable(uint16_t closedLoopIdleEnable)
{

	noInterrupts();

	IdleSettings.Idle_closedLoopEnable = closedLoopIdleEnable;
	interrupts();
}
uint16_t _Idle::getClosedLoopEnable(void)
{
	return IdleSettings.Idle_closedLoopEnable;
}
//----------------------------------------------------------------------------------
void _Idle::setCloseIdleOnBoost(uint16_t closeIdleOnBoost)
{

	noInterrupts();

	IdleSettings.Idle_closeIdleOnBoost = closeIdleOnBoost;
	interrupts();
}
uint16_t _Idle::getCloseIdleOnBoost(void)
{
	return IdleSettings.Idle_closeIdleOnBoost;
}

//----------------------------------------------------------------------------------
uint16_t _Idle::getRawTargetIdleRpm(void)
{
	return _rawTargetIdleRpm;
}
//----------------------------------------------------------------------------------
uint16_t _Idle::getTargetIdleRpm(void)
{
	return _targetIdleRpm;
}
//----------------------------------------------------------------------------------
uint16_t _Idle::getIdleEffortUnclipped(void)
{
	return _idleEffortUnclipped;
}
//----------------------------------------------------------------------------------
uint16_t _Idle::getIdleEffortOpenLoop(void)
{
	return _idleEffortOpenLoop;
}
//----------------------------------------------------------------------------------

int16_t _Idle::getIdleRpmError(void)
{
	return _idleRpmError;
}
//----------------------------------------------------------------------------------
uint16_t _Idle::getIdleEffortPostCrank(void)
{
	return _idleEffortPostCrank;
}
//----------------------------------------------------------------------------------
uint16_t _Idle::getIdleEffortThermalFan(void)
{
	return _idleEffortThermalFan;
}
//----------------------------------------------------------------------------------
int16_t _Idle::getIdleEffortPID(void)
{
	return _idleEffortPID; //  sent back 1 order of magnitude greater
}
//----------------------------------------------------------------------------------
uint16_t _Idle::getRawBaseDuty(void)
{
	return _rawBaseDuty;
}

//----------------------------------------------------------------------------------
uint16_t _Idle::getIdleEffortDuty(void)
{
	return _idleEffortDuty;
}
//----------------------------------------------------------------------------------
uint16_t _Idle::closedLoopIsActive(void)
{
	return IdlePID.GetMode();
}

//----------------------------------------------------------------------------------------

int16_t _Idle::Load(void)
{

	int16_t err;
	HWSERIAL.print("Loading Idle Settings -> ");
	err = eRAM.f_open(_filePtr, _fName, SPIFFS_RDONLY);
	if (err < 0)
	{

		HWSERIAL.println("err open");
		return err;
	}
	err = eRAM.f_read(_filePtr, reinterpret_cast<const char *>(&IdleSettings),
					  sizeof(IdleSettings));
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

		HWSERIAL.println("Idle Load Ok");
	}

Begin();


	return 1; // all is ok
}
// ---------------------------------------------------------------------------
int16_t _Idle::Save(void)
{

	int16_t err;

	HWSERIAL.print("Saving Idle Settings -> ");

	IdleSettings.Crc = CRC16.ccitt(reinterpret_cast<uint8_t *>(&IdleSettings), //Calculate CRC
								   sizeof(IdleSettings) - WORDSIZE);

	err = eRAM.f_open(_filePtr, _fName,
					  SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR);
	if (err < 0)
	{
		HWSERIAL.println("err Open");
		return err;
	}
	err = eRAM.f_write(_filePtr, reinterpret_cast<const char *>(&IdleSettings),
					   sizeof(IdleSettings));
	if (err < 0)
	{
		HWSERIAL.println("err Write");
		return err;
	}
	eRAM.f_close(_filePtr);

	HWSERIAL.println("Idle Save OK");

	return 1; // all is ok
}
// ---------------------------------------------------------------------------
bool _Idle::crcCheckSettings(void)
{

	if (IdleSettings.Crc != CRC16.ccitt(reinterpret_cast<uint8_t *>(&IdleSettings), //Calculate CRC
										sizeof(IdleSettings) - WORDSIZE))
	{
		return false;
	}

	return true; // all is ok
}

// ---------------------------------------------------------------------------
