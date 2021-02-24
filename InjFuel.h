/*
 * InjFuel.h
 *
 *  Created on: 29 Mar. 2019
 *      Author: apaml
 */

#ifndef INJFUEL_H_
#define INJFUEL_H_
#include "Arduino.h"
#include "Array3D.h"
#include "MapSensor.h"
#include "Injection_ISR.h"

#define SATURATED 0
#define PEAK_HOLD 1
#define FIXED_PRESSURE 0
#define MANIFOLD_REFERENCED 1
#define MEASURED 2
#define DRYAIRCONSTANT 28.7f
#define TEMP2KELVIN 2731.5f
#define kgToMillsSec 83916201.0f
#define CC2mg 6

class InjFuel
{
public:
	InjFuel(_Array3D *, _Array3D *, _Array3D *, _Array3D *, _Array3D *,
			_Array3D *, _Array3D *, _Array3D *, _Array3D *, _Array3D *);

	//----------------------------------------------------------------------------------------
	void processCommand(const rxData Rxdata)
	{
		uint16_t subCommand = Rxdata.subCommand;
		uint16_t dataCount = Rxdata.dataX;
		uint16_t Crc = Rxdata.dataY;

		switch (subCommand)
		{
		case 0XA1:
		{
			uint16_t buf[WORD_PACKET_SIZE];
			memset(buf, 0, sizeof(buf));
			HWSERIAL.print("Uploading Fuel Settings ->");
			buf[1] = GetEngineCapacity();
			buf[2] = GetNumberOfCyls();
			buf[3] = GetCylinderSize();
			buf[4] = GetMasterFuelTrim();
			buf[5] = GetFuelDensity();
			buf[6] = GetRegulatorType();
			buf[7] = GetNominalFuelPressure();
			buf[8] = GetFuelPressureModelling();
			buf[9] = GetInjectorType();
			buf[10] = GetInjPeakCurrent();
			buf[11] = GetInjHoldCurrent();
			buf[12] = GetMinInjEffectivePW();
			buf[13] = GetMinInjFuelVolume();
			buf[14] = GetStoichiometric();
			buf[0] = CRC16.ccitt(
				reinterpret_cast<uint8_t *>(&(buf[1])),
				(WORD_PACKET_SIZE * sizeof(uint16_t)) - sizeof(uint16_t));

			int n = hid_sendWithAck(reinterpret_cast<uint8_t *>(buf));

			if (n)
			{
				HWSERIAL.println("Error Uploading Fuel Settings");
			}
			HWSERIAL.println("OK");
			break;
		}
		case 0XA2:
			HWSERIAL.println("Downloading Fuel Settings");
			uint16_t crc = //check crc
				CRC16.ccitt(
					reinterpret_cast<uint8_t *>((uint16_t *)(&(Rxdata.dataArray[0]))),
					dataCount * sizeof(uint16_t));
			if (Crc == crc)
			{
				SetEngineCapacity(Rxdata.dataArray[0]);
				SetNumberOfCyls(Rxdata.dataArray[1]);
				SetCylinderSize(Rxdata.dataArray[2]);
				SetMasterFuelTrim(Rxdata.dataArray[3]);
				SetFuelDensity(Rxdata.dataArray[4]);
				SetRegulatorType(Rxdata.dataArray[5]);
				SetNominalFuelPressure(Rxdata.dataArray[6]);
				SetFuelPressureModelling(Rxdata.dataArray[7]);
				SetInjectorType(Rxdata.dataArray[8]);
				SetInjPeakCurrent(Rxdata.dataArray[9]);
				SetInjHoldCurrent(Rxdata.dataArray[10]);
				SetMinInjEffectivePW(Rxdata.dataArray[11]);
				SetMinInjFuelVolume(Rxdata.dataArray[12]);
				SetStoichiometric(Rxdata.dataArray[13]);

				fuelSettings.Crc = Crc;
				Save();
			}
			else
			{
				Errors.setError(ERR_SETTINGS_LOAD_CRC);
				HWSERIAL.println("Fuel Settings Crc error");
			}
			break;
		}
	}

	//----------------------------------------------------------------------------------------
	uint16_t GetInjPeriod_1()
	{
		//return 8000;
		return _InjPeriod[0];
	}
	//----------------------------------------------------------------------------------------
	uint16_t GetInjPeriod_2()
	{
		//return 8000;
		return _InjPeriod[1];
	}
	//----------------------------------------------------------------------------------------
	uint16_t GetInjPeriod_3()
	{
		//return 8000;
		return _InjPeriod[2];
	}
	//----------------------------------------------------------------------------------------
	uint16_t GetInjPeriod_4()
	{
		//return 8000;
		return _InjPeriod[3];
	}

	//----------------------------------------------------------------------------------------
	void SetCalcFlag(uint16_t Injector_Number)
	{
		_calcNow = true;
		_nextInjector2Calc = Injector_Number;
		return;
	}

	//----------------------------------------------------------------------------------------
	int16_t GetDeadTime()
	{
		if (!RunningData.IsEngineRunning())
		{

			CalcDeadtime();
		}
		return DeadTime;
	}
	//----------------------------------------------------------------------------------------
	int16_t GetInjFlowRate(void)
	{

		if (!RunningData.IsEngineRunning())
		{

			CalcInjectorFlowRate();
		}
		return InjectorFlowRate;
	}

	//----------------------------------------------------------------------------------------
	uint32_t GetMaxInjEffectivePW(void)
	{
		return _MaxInjEffectivePW;
	}

	//----------------------------------------------------------------------------------------
	uint16_t GetInjectionTiming(void)
	{
		return _injectionTiming;
	}
	//----------------------------------------------------------------------------------------
	int16_t GetChargeTemperature(void)
	{
		if (!RunningData.IsEngineRunning())
		{

			CalcChargeCorrection();
		}
		return _chargeTemperature;
	}
	//----------------------------------------------------------------------------------------
	uint16_t GetTargetLambda(void)
	{
		if (!RunningData.IsEngineRunning())
		{

			CalcTargetLambda();
		}
		return _targetLambda;
	}
	//----------------------------------------------------------------------------------------
	int16_t GetFuelPressure(void)
	{
		if (!RunningData.IsEngineRunning())
		{

			CalcDifFuelPressure();
		}

		return _diffFuelPressure;
	}
	//----------------------------------------------------------------------------------------
	uint16_t GetAirMass(void)
	{

		return _AirMass;
	}
	//----------------------------------------------------------------------------------------
	uint16_t GetFuelmg(void)
	{
		return _fuelmg;
	}
	//----------------------------------------------------------------------------------------
	uint16_t GetVE(void)
	{
		if (!RunningData.IsEngineRunning())
		{

			CalcVE();
		}
		return _VE;
	}
	//----------------------------------------------------------------------------------------
	uint16_t GetTau(void)
	{
		if (!RunningData.IsEngineRunning())
		{

			CalcTau();
		}

		return _CalculatedTau;
	}
	//----------------------------------------------------------------------------------------
	uint16_t GetX(void)
	{

		if (!RunningData.IsEngineRunning())
		{

			CalcX();
		}
		return _CalculatedX;
	}
	//----------------------------------------------------------------------------------------
	uint16_t GetPuddleSize(int16_t Injector_Number)
	{
		return _puddleSize[Injector_Number];
	}
	//----------------------------------------------------------------------------------------
	uint16_t GetTau_dt(void)
	{
		return _Tau_dt * 10;
	}
	//----------------------------------------------------------------------------------------
	int16_t GetFilmEnrich(void)
	{
		return _film_Enrich;
	}
	//----------------------------------------------------------------------------------------
	int16_t GetPostCrankEnrich(void)
	{
		return _post_crank_enrich;
	}
	//----------------------------------------------------------------------------------------
	uint16_t GetCrankingVE(void)
	{
		return _crankingVE;
	}
	//----------------------------------------------------------------------------------

	void SetStoichiometric(uint16_t Stoich)
	{

		noInterrupts();
		fuelSettings.Fuel_Stoichiometric = Stoich;
		interrupts();
	}

	uint16_t GetStoichiometric(void)
	{
		return fuelSettings.Fuel_Stoichiometric;
	}
	//----------------------------------------------------------------------------------------

	void SetMasterFuelTrim(uint16_t MasterFuelTrim)
	{

		noInterrupts();
		fuelSettings.Fuel_MasterFuelTrim = MasterFuelTrim;
		interrupts();
	}
	uint16_t GetMasterFuelTrim(void)
	{
		//return -120;
		return fuelSettings.Fuel_MasterFuelTrim;
	}

	//----------------------------------------------------------------------------------------
	void SetRegulatorType(uint16_t RegulatorType)
	{

		noInterrupts();
		fuelSettings.Fuel_RegulatorType = RegulatorType;
		interrupts();
	}

	uint16_t GetRegulatorType(void)
	{
		return fuelSettings.Fuel_RegulatorType;
	}

	//----------------------------------------------------------------------------------------

	void SetFuelDensity(uint16_t FuelDensity)
	{

		noInterrupts();
		fuelSettings.Fuel_FuelDensity = FuelDensity;
		interrupts();

		_fuelCorrection = (fuelSettings.Fuel_FuelDensity * 10) / CC2mg;
	}

	uint16_t GetFuelDensity(void)
	{
		return fuelSettings.Fuel_FuelDensity;
	}

	//----------------------------------------------------------------------------------------

	void SetNumberOfCyls(uint16_t NumberOfCyls)
	{

		noInterrupts();

		fuelSettings.Fuel_NumberOfCyls = NumberOfCyls;
		interrupts();
	}

	uint16_t GetNumberOfCyls(void)
	{
		return fuelSettings.Fuel_NumberOfCyls;
	}

	//----------------------------------------------------------------------------------------

	void SetEngineCapacity(uint16_t EngineCapacity)
	{
		noInterrupts();

		fuelSettings.Fuel_EngineCapacity = EngineCapacity;
		interrupts();
	}

	uint16_t GetEngineCapacity(void)
	{
		return fuelSettings.Fuel_EngineCapacity;
	}

	//----------------------------------------------------------------------------------------

	void SetCylinderSize(uint16_t CylinderSize)
	{

		noInterrupts();
		fuelSettings.Fuel_CylinderSize = CylinderSize;
		interrupts();
	}

	uint16_t GetCylinderSize(void)
	{
		return fuelSettings.Fuel_CylinderSize;
	}

	//----------------------------------------------------------------------------------------

	void SetNominalFuelPressure(uint16_t NominalFuelPressure)
	{
		//framLow.writeWord(NOMINAL_FUEL_PRESSURE_FRAM, NominalFuelPressure);
		noInterrupts();
		fuelSettings.Fuel_NominalFuelPressure = NominalFuelPressure;
		interrupts();
	}

	uint16_t GetNominalFuelPressure(void)
	{
		return fuelSettings.Fuel_NominalFuelPressure;
	}

	//----------------------------------------------------------------------------------------

	void SetFuelPressureModelling(bool FuelPressureModelling)
	{

		noInterrupts();
		fuelSettings.Fuel_UseFuelPressureSensor = FuelPressureModelling;
		interrupts();
	}

	uint16_t GetFuelPressureModelling(void)
	{
		return fuelSettings.Fuel_UseFuelPressureSensor;
	}

	//----------------------------------------------------------------------------------------

	void SetMinInjEffectivePW(uint16_t MinEffectivePW)
	{

		noInterrupts();
		fuelSettings.Fuel_MinInjEffectivePW = MinEffectivePW;
		interrupts();
	}

	uint16_t GetMinInjEffectivePW(void)
	{
		return fuelSettings.Fuel_MinInjEffectivePW;
	}

	//----------------------------------------------------------------------------------------

	void SetMinInjFuelVolume(uint16_t MinInjFuelVolume)
	{

		noInterrupts();
		fuelSettings.Fuel_MinInjFuelVolume = MinInjFuelVolume;
		interrupts();
	}

	uint16_t GetMinInjFuelVolume(void)
	{
		return fuelSettings.Fuel_MinInjFuelVolume;
	}

	//----------------------------------------------------------------------------------------

	void SetInjPeakCurrent(uint16_t InjPeakCurrent)
	{

		noInterrupts();
		fuelSettings.Fuel_InjPeakCurrent = InjPeakCurrent;
		interrupts();
	}

	uint16_t GetInjPeakCurrent(void)
	{
		return fuelSettings.Fuel_InjPeakCurrent;
	}

	//----------------------------------------------------------------------------------------

	void SetInjHoldCurrent(uint16_t InjHoldCurrent)
	{

		noInterrupts();
		fuelSettings.Fuel_InjHoldCurrent = InjHoldCurrent;
		interrupts();
	}

	uint16_t GetInjHoldCurrent(void)
	{
		return fuelSettings.Fuel_InjHoldCurrent;
	}

	//----------------------------------------------------------------------------------------
	void SetInjectorType(uint16_t InjectorType)
	{

		noInterrupts();
		fuelSettings.Fuel_InjectorType = InjectorType;
		interrupts();
	}

	uint16_t GetInjectorType(void)
	{
		return fuelSettings.Fuel_InjectorType;
	}

	void CalcCrankingVE(const int16_t);
	void CalcInjectorPeriod(void);
	int16_t Load(void);

private:
	_Array3D *_veMap;
	_Array3D *_injFlowRateMap;
	_Array3D *_postCrankEnrichMap;
	_Array3D *_heatSoakMap;
	_Array3D *_deadTimeMap;
	_Array3D *_targetLambdaMap;
	_Array3D *_poolingMap;
	_Array3D *_evapTimeMap;
	_Array3D *_crankingVeMap;
	_Array3D *_injTimingMap;

	void CalcInjectorTiming(void);
	void CalcTargetLambda(void);
	void CalcVE(void);
	void CalcPostCrankEnrich(void);
	void CalcChargeCorrection(void);
	void CalcX(void);
	void CalcTau(void);
	void CalcRequiredFuelVolume(void);
	void CalcInjectorFlowRate(void);
	void CalcDeadtime(void);
	void CalcMaxInjectorPeriod(void);
	void CalcDifFuelPressure(void);
	uint32_t CalcFuelFilm(const uint32_t, const int16_t);
	bool crcCheckSettings(void);
	int16_t Save(void);
	char _fName[6] = "sFuel";
	spiffs_file _filePtr;

	bool _calcNow = false;
	struct t_SETTINGS
	{

		uint16_t Fuel_EngineCapacity;
		uint16_t Fuel_NumberOfCyls;	 // 0-6
		uint16_t Fuel_CylinderSize;	 //cc
		int16_t Fuel_MasterFuelTrim; //
		uint16_t Fuel_FuelDensity;	 //kg/m3
		uint16_t Fuel_RegulatorType;
		uint16_t Fuel_NominalFuelPressure;
		uint16_t Fuel_UseFuelPressureSensor;
		uint16_t Fuel_InjectorType;
		uint16_t Fuel_InjPeakCurrent;
		uint16_t Fuel_InjHoldCurrent;
		uint16_t Fuel_MinInjEffectivePW; //microseconds
		uint16_t Fuel_MinInjFuelVolume;	 // microlitres
		uint16_t Fuel_Stoichiometric;
		uint16_t Crc;
	};
	t_SETTINGS fuelSettings;

	//Calculated values

	int32_t _atmosphericPressure = 1013;
	uint32_t _VE;
	uint32_t _crankingVE;
	uint32_t _AirMass;
	uint32_t _Fuelmg;
	uint32_t _fuelmg;
	uint32_t _CalculatedX;
	uint32_t _CalculatedTau;
	uint32_t _targetLambda;
	int32_t _post_crank_enrich;
	uint16_t _fuelCorrection;
	int32_t _film_Enrich;
	float _Tau_dt;
	float _evapPuddleFuel;
	uint32_t InjectorFlowRate = 350;
	uint32_t _MaxInjEffectivePW; //keep 32bit
	uint32_t DeadTime = 0;
	int32_t _diffFuelPressure;
	uint32_t _injectionTiming = 380;
	uint16_t _nextInjector2Calc = 0;
	int32_t _chargeTemperature;
	uint16_t _InjPeriod[4];
	uint16_t _puddleSize[4] = {0};

	uint16_t _sampleTime;
	elapsedMillis _clock;
};

extern InjFuel INJfuel;

#endif /* INJFUEL_H_ */
