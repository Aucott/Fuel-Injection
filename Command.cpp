/*
 * Command.cpp
 *
 *  Created on: 10 Nov. 2018
 *      Author: apaml
 */
#include "Command.h"

void processCommands(void)
{
	uint8_t RXbuffer[64];

	rxData RXstruct;

	while (1)
	{

		if (RawHID.recv(RXbuffer, 0) > 0)

		{
			RawHID.send(RXbuffer, 0);
			memcpy(&RXstruct, RXbuffer, sizeof(RXstruct));
			//	HWSERIAL.println(RXstruct.command, HEX);

			uint16_t Command = RXstruct.command;
			uint16_t SubCommand = RXstruct.subCommand;
			uint16_t Offset = RXstruct.dataX;
			uint16_t Data = RXstruct.dataY;

			// ---------------------------------------------------------------------------
			switch (Command)
			{
				
			case GET_RUNNING_DATA:
				SendRunningData();
				break;
				//-----------------------------------------------------------------------------
				//	case REBOOT:
				///	noInterrupts();

			//	CPU_RESTART;
			//	break;
			//------------------------------------------------------------------------------------------------
			case INJ_FUEL_SETTINGS:
				INJfuel.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case TPS_SETTINGS:
				TpsSensor.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case IGN_SETTINGS:
				Ignition.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case RUNNING_DATA_SETTINGS:
				RunningData.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case TRIGGER_SETTINGS:
				Triggers.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case FUEL_PUMP_SETTINGS:
				FuelPump.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case THERMAL_FAN_SETTINGS:
				ThermalFan.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case IDLE_SETTINGS:
				Idle.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case BOOST_SETTINGS:
				BoostControl.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case MAP_SETTINGS:
				MapSensor.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case RPM_LIMITER_SETTINGS:
				RpmLimiter.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case BOOST_CUT_SETTINGS:
				BoostCut.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case ERRORS:
				Errors.processCommand(SubCommand, Offset, Data);
				break;
				//------------------------------------------------------------------------------------------------
			case LOAD_VE_TABLE:
				VE_Table.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case LOAD_IGN_TABLE:
				Ignition_Table.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case LOAD_TARGETAFR_TABLE:
				Target_AFR_Table.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case LOAD_IGN_DWELL_TABLE:
				Ignition_Dwell_Table.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case LOAD_INJ_TIMING_TABLE:
				Injector_Timing_Table.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case LOAD_PREDICTED_TABLE:
				Predicted_Table.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case LOAD_X_POOLING_TABLE:
				Pooling_Table.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case LOAD_TAU_EVAP_TABLE:
				Evap_Time_Table.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case LOAD_HEAT_SOAK_TABLE:
				Heat_Soak_Table.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case LOAD_CHARGE_TEMP_CORR_TABLE:
				Charge_Temp_Corr_Table.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case LOAD_INJ_DEAD_TIME_TABLE:
				Inj_Dead_Time_Table.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case LOAD_INJ_FLOW_RATE_TABLE:
				Inj_Flow_Rate_Table.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case LOAD_TPS_TRIGGER_RATE_TABLE:
				TPS_Trigger_Rate_Table.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case LOAD_CRANKING_VE_TABLE:
				Cranking_VE_Table.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case LOAD_POST_CRANK_ENRICH_TABLE:
				Post_Crank_Enrich_Table.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case LOAD_CRANKING_IGN_TIMING_TABLE:
				Crank_Ign_Timing_Table.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case LOAD_MAT_CAL_TABLE:
				MAT_Calibration_Table.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case LOAD_IGN_CLT_CORR_TABLE:
				Ign_CLT_Correction_Table.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case LOAD_MAP_CAL_TABLE:
				MAP_Calibration_Table.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case LOAD_CLT_CAL_TABLE:
				CLT_Calibration_Table.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case LOAD_IGN_MAT_CORR_TABLE:
				Ign_MAT_Correction_Table.processCommand(RXstruct);
				break;
				//-------------------------------------------------------------------------------
			case LOAD_POST_CRANK_IDLE_TABLE:
				Post_Crank_Idle_Table.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case LOAD_TARGET_IDLE_TABLE:
				Target_Idle_Speed_Table.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case LOAD_BASE_IDLE_DUTY_TABLE:
				Base_Idle_Duty_Cycle_Table.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case LOAD_IDLE_IGN_TRIM_TABLE:
				Idle_Ign_Timing_Trim_Table.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case LOAD_TARGET_BOOST_1_TABLE:
				Target_Boost_1_Table.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case LOAD_TARGET_BOOST_2_TABLE:
				Target_Boost_2_Table.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case LOAD_BOOST_DUTY_TABLE:
				Boost_Duty_Cycle_Table.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case LOAD_RPM_LIMIT_TABLE:
				RPM_Limit_Table.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			case LOAD_AUX_PWM_TABLE:
				Aux_Output_PWM_Table.processCommand(RXstruct);
				break;
				//------------------------------------------------------------------------------------------------
			}
		}
		threads.yield();
	}
}

//----------------------------------------------------------------------------------------

void SendRunningData(void)
{

	uint16_t TXpacket1[32];

	uint32_t temp32Value = RunningData.GetEngineRunningTime();

	TXpacket1[0] = 0xABC1;
	TXpacket1[1] = RunningData.GetEngineState();
	TXpacket1[2] = highWord(temp32Value); // Engine Running Time
	TXpacket1[3] = lowWord(temp32Value);
	TXpacket1[4] = RunningData.GetRPM();
	TXpacket1[5] = MapSensor.getMap();
	TXpacket1[6] = 0; //Sensors
	TXpacket1[7] = TpsSensor.GetTPS();
	TXpacket1[8] = TpsSensor.IsClosedThrottle();
	TXpacket1[9] = TpsSensor.IsWOT();
	TXpacket1[10] = CLT_Sensor.GetTemperature();
	TXpacket1[11] = MAT_Sensor.GetTemperature();
	TXpacket1[12] = INJfuel.GetChargeTemperature();
	TXpacket1[13] = 0; //Lambda.Get_Lambda();
	TXpacket1[14] = Battery.GetVoltage();
	TXpacket1[15] = 0; //Ignition
	TXpacket1[16] = Ignition.GetIgnAngle();
	TXpacket1[17] = Ignition.GetRawIgnAngle();
	TXpacket1[18] = Ignition.GetCrankingIgnAngle();
	TXpacket1[19] = Ignition.GetDwellTime();
	TXpacket1[20] = Ignition.GetActualDwell();
	TXpacket1[21] = Ignition.GetCltCorrection();
	TXpacket1[22] = Ignition.GetMatCorrection();
	TXpacket1[23] = Ignition.isTimingLocked();
	TXpacket1[24] = Ignition.GetIdleIgnTrim();
	TXpacket1[25] = TpsSensor.GetRawADC(); //Raw Sensor Values
	TXpacket1[26] = MapSensor.getRawADC();
	TXpacket1[27] = MAT_Sensor.GetRawADC();
	TXpacket1[28] = CLT_Sensor.GetRawADC();
	TXpacket1[29] = 0;
	TXpacket1[30] = 0;
	TXpacket1[31] = BufferOverflow;

	uint16_t n = RawHID.send(TXpacket1, 1);
	//uint16_t n = hid_sendWithAck(reinterpret_cast<uint8_t *>(TXpacket1));

	if (n == 0)
	{
		HWSERIAL.println("Error 1 SendRunningData");
		return;
	}

	temp32Value = INJfuel.GetMaxInjEffectivePW(); //

	TXpacket1[0] = 0xABC2; //Injector
	TXpacket1[1] = INJfuel.GetInjPeriod_1();
	TXpacket1[2] = highWord(temp32Value);
	TXpacket1[3] = lowWord(temp32Value);
	TXpacket1[4] = INJfuel.GetDeadTime();
	TXpacket1[5] = INJfuel.GetVE();
	TXpacket1[6] = INJfuel.GetAirMass();
	TXpacket1[7] = INJfuel.GetFuelmg();
	TXpacket1[8] = INJfuel.GetInjFlowRate();
	TXpacket1[9] = INJfuel.GetPostCrankEnrich();
	TXpacket1[10] = INJfuel.GetCrankingVE();
	TXpacket1[11] = INJfuel.GetTargetLambda();
	TXpacket1[12] = INJfuel.GetFuelPressure();
	TXpacket1[13] = INJfuel.GetInjectionTiming();
	TXpacket1[14] = 0; //Transient
	TXpacket1[15] = MapSensor.isMapPredictionRunning();
	TXpacket1[16] = MapSensor.getPredictedMapValue();
	TXpacket1[17] = TpsSensor.GetTpsTriggerValue();
	TXpacket1[18] = TpsSensor.GetTpsDelta();
	TXpacket1[19] = INJfuel.GetX();
	TXpacket1[20] = INJfuel.GetTau();
	TXpacket1[21] = INJfuel.GetPuddleSize(0);
	TXpacket1[22] = INJfuel.GetTau_dt();
	TXpacket1[23] = INJfuel.GetFilmEnrich();
	TXpacket1[24] = 0;
	TXpacket1[25] = RpmLimiter.isFuelCut();
	TXpacket1[26] = RpmLimiter.isIgnCut();
	TXpacket1[27] = RpmLimiter.getHardRPM();
	TXpacket1[28] = RpmLimiter.getSoftRPM();
	TXpacket1[29] = RpmLimiter.isSoftRevLimited();
	TXpacket1[30] = RpmLimiter.isHardRevLimited();
	TXpacket1[31] = 0;
	//n = hid_sendWithAck(reinterpret_cast<uint8_t *>(TXpacket1));

	n = RawHID.send(TXpacket1, 1);
	if (n == 0)
	{
		HWSERIAL.println("Error 2 SendRunningData");
		return;
	}

	TXpacket1[0] = 0xABC3; //Injector
	TXpacket1[1] = BoostCut.isFuelCut();
	TXpacket1[2] = BoostCut.isIgnCut();
	TXpacket1[3] = 0;
	TXpacket1[4] = 0;
	TXpacket1[5] = 0;
	TXpacket1[6] = 0;
	TXpacket1[7] = 0;
	TXpacket1[8] = 0;
	TXpacket1[9] = 0;
	TXpacket1[10] = 0;
	TXpacket1[11] = 0;
	TXpacket1[12] = 0;
	TXpacket1[13] = 0;
	TXpacket1[14] = 0;
	TXpacket1[15] = 0;
	TXpacket1[16] = 0;
	TXpacket1[17] = 0;
	TXpacket1[18] = 0;
	TXpacket1[19] = 0;
	TXpacket1[20] = 0;
	TXpacket1[21] = 0;
	TXpacket1[22] = 0;
	TXpacket1[23] = 0;
	TXpacket1[24] = 0;
	TXpacket1[25] = 0;
	TXpacket1[26] = 0;
	TXpacket1[27] = 0;
	TXpacket1[28] = 0;
	TXpacket1[29] = 0;
	TXpacket1[30] = 0;
	TXpacket1[31] = 0;
	//n = hid_sendWithAck(reinterpret_cast<uint8_t *>(TXpacket1));
	n = RawHID.send(TXpacket1, 1);
	if (n == 0)
	{
		HWSERIAL.println("Error 3 SendRunningData");
		return;
	}

	TXpacket1[0] = 0xABC4; //Idle
	TXpacket1[1] = Idle.getRawTargetIdleRpm();
	TXpacket1[2] = Idle.getTargetIdleRpm();
	TXpacket1[3] = Idle.getRawBaseDuty();
	TXpacket1[4] = Idle.getIdleEffortPostCrank();
	TXpacket1[5] = Idle.getIdleEffortThermalFan();
	TXpacket1[6] = Idle.getIdleEffortPID();
	TXpacket1[7] = Idle.getIdleEffortOpenLoop();
	TXpacket1[8] = Idle.getIdleEffortUnclipped();
	TXpacket1[9] = Idle.getIdleRpmError();
	TXpacket1[10] = Idle.getIdleEffortDuty();
	TXpacket1[11] = Idle.closedLoopIsActive();
	TXpacket1[12] = 0;
	TXpacket1[13] = MapSensor.isBoosting();
	TXpacket1[14] = BoostControl.getClosedLoopEnable();
	TXpacket1[15] = BoostControl.getActiveMap();
	TXpacket1[16] = BoostControl.getTargetBoost();
	TXpacket1[17] = BoostControl.getBoostPressureError();
	TXpacket1[18] = BoostControl.getRawBaseDuty();
	TXpacket1[19] = BoostControl.getPidDuty();
	TXpacket1[20] = BoostControl.getWasteGateDuty();
	TXpacket1[21] = 0;
	TXpacket1[22] = ThermalFan.isRunning();
	TXpacket1[23] = ThermalFan.getAddedIdleEffort();
	TXpacket1[24] = ThermalFan.getAddedIdleRpm();
	TXpacket1[25] = FuelPump.isRunning();
	TXpacket1[26] = 0;
	TXpacket1[27] = 0; //RunningData.GetCrc();
	TXpacket1[28] = Errors.GetErrorsHighByte();
	TXpacket1[29] = Errors.GetErrorsLowByte();
	TXpacket1[30] = 0;
	TXpacket1[31] = RunningData.GetMainLoopCounter();

	//n = hid_sendWithAck(reinterpret_cast<uint8_t *>(TXpacket1));

	n = RawHID.send(TXpacket1, 1);
	if (n == 0)
	{
		HWSERIAL.println("Error 4 SendRunningData");
	}
	return;
}

/***************************************************************************/
//Upload Maps to PC
void uploadData2PC(void)
{

	VE_Table.UploadTable();
	Ignition_Table.UploadTable();
	Target_AFR_Table.UploadTable();
	Ignition_Dwell_Table.UploadTable();
	Injector_Timing_Table.UploadTable();
	Predicted_Table.UploadTable();

	Pooling_Table.UploadTable();
	Evap_Time_Table.UploadTable();
	Heat_Soak_Table.UploadTable();
	Charge_Temp_Corr_Table.UploadTable();

	Inj_Dead_Time_Table.UploadTable();
	Inj_Flow_Rate_Table.UploadTable();
	TPS_Trigger_Rate_Table.UploadTable();
	Cranking_VE_Table.UploadTable();
	Post_Crank_Enrich_Table.UploadTable();
	Crank_Ign_Timing_Table.UploadTable();

	Ign_CLT_Correction_Table.UploadTable();
	Ign_MAT_Correction_Table.UploadTable();

	MAP_Calibration_Table.UploadTable();
	CLT_Calibration_Table.UploadTable();
	MAT_Calibration_Table.UploadTable();

	Post_Crank_Idle_Table.UploadTable();
	Target_Idle_Speed_Table.UploadTable();
	Base_Idle_Duty_Cycle_Table.UploadTable();
	Idle_Ign_Timing_Trim_Table.UploadTable();
	Target_Boost_1_Table.UploadTable();
	Target_Boost_2_Table.UploadTable();
	Boost_Duty_Cycle_Table.UploadTable();
	RPM_Limit_Table.UploadTable();
	Aux_Output_PWM_Table.UploadTable();

	return;
}

/***************************************************************************/

//Load maps from ram
bool LoadMapsFromRam(void)
{

	//Errors.loadSettings();
	Errors.clearErrorFlag();

	VE_Table.LoadArrayFromFram();
	Ignition_Table.LoadArrayFromFram();
	Target_AFR_Table.LoadArrayFromFram();
	Ignition_Dwell_Table.LoadArrayFromFram();
	Injector_Timing_Table.LoadArrayFromFram();
	Predicted_Table.LoadArrayFromFram();
	Pooling_Table.LoadArrayFromFram();
	Evap_Time_Table.LoadArrayFromFram();
	Heat_Soak_Table.LoadArrayFromFram();
	Charge_Temp_Corr_Table.LoadArrayFromFram();
	Inj_Dead_Time_Table.LoadArrayFromFram();
	Inj_Flow_Rate_Table.LoadArrayFromFram();
	TPS_Trigger_Rate_Table.LoadArrayFromFram();
	Cranking_VE_Table.LoadArrayFromFram();
	Post_Crank_Enrich_Table.LoadArrayFromFram();
	Crank_Ign_Timing_Table.LoadArrayFromFram();
	Ign_CLT_Correction_Table.LoadArrayFromFram();
	Ign_MAT_Correction_Table.LoadArrayFromFram();
	MAP_Calibration_Table.LoadArrayFromFram();
	CLT_Calibration_Table.LoadArrayFromFram();
	MAT_Calibration_Table.LoadArrayFromFram();
	Post_Crank_Idle_Table.LoadArrayFromFram();
	Target_Idle_Speed_Table.LoadArrayFromFram();
	Base_Idle_Duty_Cycle_Table.LoadArrayFromFram();
	Idle_Ign_Timing_Trim_Table.LoadArrayFromFram();
	Target_Boost_1_Table.LoadArrayFromFram();
	Target_Boost_2_Table.LoadArrayFromFram();
	Boost_Duty_Cycle_Table.LoadArrayFromFram();
	RPM_Limit_Table.LoadArrayFromFram();
	Aux_Output_PWM_Table.LoadArrayFromFram();

	Ignition.Load();
	INJfuel.Load();
	TpsSensor.Load();
	Triggers.Load();
	ThermalFan.Load();
	FuelPump.Load();
	Idle.Load(); //RunningData.Load();
	BoostControl.Load();
	MapSensor.Load();
	RpmLimiter.Load();
	BoostCut.Load();

	return Errors.getErrorFlag();
}

/***************************************************************************/
