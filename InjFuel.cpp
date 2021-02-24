/*
 * InjFuel.cpp
 *
 *  Created on: 29 Mar. 2019
 *      Author: apaml
 */

#include "InjFuel.h"

InjFuel::InjFuel(_Array3D *veMap, _Array3D *injFlowRateMap,
				 _Array3D *postCrankEnrichMap, _Array3D *heatSoakMap,
				 _Array3D *deadTimeMap, _Array3D *targetLambdaMap, _Array3D *poolingMap,
				 _Array3D *evapTimeMap, _Array3D *crankingVeMap,
				 _Array3D *injTimingMap)
{

	_veMap = veMap;
	_injFlowRateMap = injFlowRateMap;
	_postCrankEnrichMap = postCrankEnrichMap;
	_heatSoakMap = heatSoakMap;
	_deadTimeMap = deadTimeMap;
	_targetLambdaMap = targetLambdaMap;
	_poolingMap = poolingMap;
	_evapTimeMap = evapTimeMap;
	_crankingVeMap = crankingVeMap;
	_injTimingMap = injTimingMap;

	_sampleTime = 310;

	fuelSettings.Fuel_FuelDensity = 719;

	fuelSettings.Fuel_CylinderSize = fuelSettings.Fuel_EngineCapacity / fuelSettings.Fuel_NumberOfCyls;
	_fuelCorrection = (fuelSettings.Fuel_FuelDensity * 10) / CC2mg;
}

//----------------------------------------------------------------------------------------

void InjFuel::CalcMaxInjectorPeriod(void)
{

	uint32_t maxEffectivePW = (Cyl_360_degree_Time << 1) - 500; // Max duty - 250uS

	_MaxInjEffectivePW = maxEffectivePW;
	return;
}

//----------------------------------------------------------------------------------------

void InjFuel::CalcRequiredFuelVolume(void)
{
	float tempVal;

	float AbsChargeTemp = _chargeTemperature + TEMP2KELVIN;

	_AirMass = ((float)MapSensor.getMap() * (float)_VE * (float)fuelSettings.Fuel_CylinderSize) / (DRYAIRCONSTANT * AbsChargeTemp);

	tempVal =
		((float)_AirMass * 100000) / ((float)_targetLambda * (float)fuelSettings.Fuel_Stoichiometric); //result x10

	_fuelmg = tempVal + (((float)tempVal * ((float)fuelSettings.Fuel_MasterFuelTrim + (float)_post_crank_enrich) // TODO master fuel trim increased to x 10
						  / 1000.0));
	return;
}

//----------------------------------------------------------------------------------------

void InjFuel::CalcChargeCorrection(void)
{

	_chargeTemperature = map(
		_heatSoakMap->GetFrom3dMap(RunningData.GetRPM(),
								   MapSensor.getMap()),
		0, 1000, MAT_Sensor.GetTemperature(),
		CLT_Sensor.GetTemperature());
	return;
}
//----------------------------------------------------------------------------------------
void InjFuel::CalcDeadtime(void)
{

	DeadTime = _deadTimeMap->GetFrom3dMap(Battery.GetVoltage(),
										  _diffFuelPressure);
	return;
}
//----------------------------------------------------------------------------------------

void InjFuel::CalcDifFuelPressure(void)
{

	switch (fuelSettings.Fuel_RegulatorType)
	{

	case FIXED_PRESSURE:

		_diffFuelPressure = (int32_t)fuelSettings.Fuel_NominalFuelPressure - ((MapSensor.getMap() - _atmosphericPressure) / 10); //;TODO
		break;
	case MANIFOLD_REFERENCED:
		_diffFuelPressure = fuelSettings.Fuel_NominalFuelPressure;
		break;

	case MEASURED: //TODO
		break;
	}
	return;
}
//----------------------------------------------------------------------------------------

void InjFuel::CalcPostCrankEnrich(void)
{
	if (RunningData.IsEngineRunning() == ENGINE_RUNNING)
	{

		_post_crank_enrich = _postCrankEnrichMap->GetFrom3dMap(
			RunningData.GetEngineRunningTime(),
			CLT_Sensor.GetTemperature());
	}
	else
	{
		_post_crank_enrich = 0;
	}
	return;
}

//----------------------------------------------------------------------------------------
void InjFuel::CalcX(void)
{

	_CalculatedX = _poolingMap->GetFrom3dMap(CLT_Sensor.GetTemperature(),
											 MapSensor.getMap());
	return;
}

//----------------------------------------------------------------------------------------

void InjFuel::CalcTau(void)
{

	_CalculatedTau = _evapTimeMap->GetFrom3dMap(RunningData.GetRPM(),
												MapSensor.getMap());
	return;
}
//----------------------------------------------------------------------------------------

uint32_t InjFuel::CalcFuelFilm(const uint32_t requiredVolume,
							   const int16_t Injector_Number)
{

	float dt = Cyl_360_degree_Time / 50000.0; //50000

	_Tau_dt = _CalculatedTau / dt; // amount of time fuel has evaporated in the given time

	uint32_t evapPuddleFuel = (_puddleSize[Injector_Number] / _Tau_dt); //calculate how much has evaporated from the puddle in the given time

	int32_t remainingRequiredVolume = requiredVolume - evapPuddleFuel; //calculate how much we need to inject given that the above amount is now been added to fuel volume

	int32_t ActualVolume = remainingRequiredVolume
						   //+ ((remainingRequiredVolume * _CalculatedX) / 1000.0); //calculate how much we need to inject now that x percentage of fuel is dropping out into the puddle
						   + ((requiredVolume * _CalculatedX) / 1000.0); //calculate how much we need to inject now that x percentage of fuel is dropping out into the puddle

	int32_t puddleAfter = (_puddleSize[Injector_Number] - evapPuddleFuel) //calculate  how much fuel is now in the puddle
						  + (ActualVolume - remainingRequiredVolume);

	if (puddleAfter < 0)
		puddleAfter = 0;

	_puddleSize[Injector_Number] = puddleAfter;

	_film_Enrich = ((ActualVolume - (int32_t)requiredVolume) * 1000) / (int32_t)requiredVolume;

	return ActualVolume;
}
//----------------------------------------------------------------------------------------

void InjFuel::CalcInjectorPeriod(void)
{

	if (_calcNow)
	{ // not ready yet return

		_calcNow = false;
		CalcChargeCorrection();
		CalcInjectorFlowRate();
		CalcDifFuelPressure();
		CalcDeadtime();
		CalcPostCrankEnrich();
		CalcVE();
		CalcTargetLambda();
		CalcInjectorTiming();
		CalcRequiredFuelVolume();
		CalcX();
		CalcTau();

		_Fuelmg = CalcFuelFilm(_fuelmg, _nextInjector2Calc); // check accell enrich

		uint32_t period = 10000000 / (InjectorFlowRate * (uint32_t)_fuelCorrection / _Fuelmg);

		period += DeadTime; // Apply injector dead time correction

		CalcMaxInjectorPeriod();

		if (period < fuelSettings.Fuel_MinInjEffectivePW)
			period = fuelSettings.Fuel_MinInjEffectivePW; //Check we are running under 100% duty

		if (period > _MaxInjEffectivePW)
			period = _MaxInjEffectivePW; //Check we are not running over 100% duty

		_InjPeriod[_nextInjector2Calc] = period;
	}
	return;
}

//----------------------------------------------------------------------------------------

void InjFuel::CalcCrankingVE(const int16_t cltValue)
{

	if (_clock >= _sampleTime)
	{ // do once every sample time
		_clock = _clock - _sampleTime;

		_crankingVE = _crankingVeMap->GetFrom2dMapX(cltValue, 1);
	}
	return;
}
//----------------------------------------------------------------------------------------

void InjFuel::CalcVE(void)
{

	if (RunningData.IsEngineRunning() == ENGINE_RUNNING)
	{

		_VE = _veMap->GetFrom3dMap(RunningData.GetRPM(), MapSensor.getMap());
	}
	else
	{
		_VE = _crankingVE;
	}

	return;
}
//----------------------------------------------------------------------------------------

void InjFuel::CalcTargetLambda(void)
{

	_targetLambda = _targetLambdaMap->GetFrom3dMap(RunningData.GetRPM(),
												   MapSensor.getMap());
	return;
}
//----------------------------------------------------------------------------------------

void InjFuel::CalcInjectorFlowRate(void)
{
	InjectorFlowRate = _injFlowRateMap->GetFrom2dMapX(_diffFuelPressure, 1);
	return;
}
//----------------------------------------------------------------------------------------
void InjFuel::CalcInjectorTiming(void)
{
	_injectionTiming = _injTimingMap->GetFrom3dMap(RunningData.GetRPM(),
												   MapSensor.getMap());

	return;
}

//----------------------------------------------------------------------------------------

int16_t InjFuel::Load(void)
{

	int16_t err;
	HWSERIAL.print("Loading Fuel Settings -> ");
	err = eRAM.f_open(_filePtr, _fName, SPIFFS_RDONLY);
	if (err < 0)
	{

		HWSERIAL.println("err open");
		return err;
	}
	err = eRAM.f_read(_filePtr, reinterpret_cast<const char *>(&fuelSettings),
					  sizeof(fuelSettings));
	if (err < 0)
	{
		HWSERIAL.println("err Read");
		return err;
	}
	eRAM.f_close(_filePtr);

	if (!crcCheckSettings())
	{
		HWSERIAL.print("Fuel crc error ");
	
	}
	else
	{
		HWSERIAL.println("Fuel Load Ok");
	}

	return 1; // all is ok
}
// ---------------------------------------------------------------------------
int16_t InjFuel::Save(void)
{

	int16_t err;

	HWSERIAL.print("Saving Fuel Settings -> ");
	fuelSettings.Crc = CRC16.ccitt(reinterpret_cast<uint8_t *>(&fuelSettings), //Calculate CRC
							   sizeof(fuelSettings) - WORDSIZE);

	err = eRAM.f_open(_filePtr, _fName,
					  SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR);
	if (err < 0)
	{
		HWSERIAL.println("err Open");
		return err;
	}
	err = eRAM.f_write(_filePtr, reinterpret_cast<const char *>(&fuelSettings),
					   sizeof(fuelSettings));
	if (err < 0)
	{
		HWSERIAL.println("err Write");
		return err;
	}
	eRAM.f_close(_filePtr);

	HWSERIAL.println("Fuel Save OK");

	return 1; // all is ok
}
// ---------------------------------------------------------------------------
bool InjFuel::crcCheckSettings(void)
{
	uint16_t crc = CRC16.ccitt(reinterpret_cast<uint8_t *>(&fuelSettings), //Calculate CRC
							   sizeof(fuelSettings) - WORDSIZE);

	if (fuelSettings.Crc != crc)
	{

		HWSERIAL.print("Wanted ");
		HWSERIAL.println(fuelSettings.Crc);
		HWSERIAL.print("Got ");
		HWSERIAL.println(crc);

		return false;
	}

	return true; // all is ok
}

// ---------------------------------------------------------------------------
