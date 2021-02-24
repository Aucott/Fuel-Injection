#include "ICosworth.h"

#define rpmA(input) 500000 / ((input / 60.0) * 4.0)

//PeriodicTimer Adc_Interval(TCK); // Tick-Timer does not use any hardware timer (20 32bit channels)
PeriodicTimer RPM_Interval(GPT2); // Tick-Timer does not use any hardware timer (20 32bit channels)

ADC *adc = new ADC(); // adc object
_Triggers Triggers;
_RunningData RunningData;
_errors Errors;
_RpmLimiter RpmLimiter(&RPM_Limit_Table);
_BoostCut BoostCut;
Temperature_Sensor CLT_Sensor(&CLT_Calibration_Table);
Temperature_Sensor MAT_Sensor(&MAT_Calibration_Table);
InjFuel INJfuel(&VE_Table, &Inj_Flow_Rate_Table, &Post_Crank_Enrich_Table,
				&Heat_Soak_Table, &Inj_Dead_Time_Table, &Target_AFR_Table,
				&Pooling_Table, &Evap_Time_Table, &Cranking_VE_Table,
				&Injector_Timing_Table);
_Ignition Ignition(&Ignition_Table, &Crank_Ign_Timing_Table,
				   &Ign_CLT_Correction_Table, &Ign_MAT_Correction_Table,
				   &Ignition_Dwell_Table, &Idle_Ign_Timing_Trim_Table);
_MapSensor MapSensor(&MAP_Calibration_Table, &Predicted_Table);
_BoostControl BoostControl(&Target_Boost_1_Table, &Target_Boost_1_Table,
						   &Boost_Duty_Cycle_Table);
_TPS TpsSensor(&TPS_Trigger_Rate_Table);
_Idle Idle(&Target_Idle_Speed_Table, &Base_Idle_Duty_Cycle_Table,
		   &Post_Crank_Idle_Table);
_Battery Battery;
_FuelPump FuelPump;
_ThermalFan ThermalFan;
FastCRC16 CRC16;

int16_t ScratchPad[SCRATCHPADSIZE];

spiffs_t4 eRAM;

void setup()
{
	pinMode(ledPin, OUTPUT);
	pinMode(INJECTOR_1_PIN, OUTPUT);
	pinMode(INJECTOR_2_PIN, OUTPUT);
	pinMode(INJECTOR_3_PIN, OUTPUT);
	pinMode(INJECTOR_4_PIN, OUTPUT);


	//pinMode(TRIGGER_PIN, OUTPUT);// todo CHANGE TO INPUT
	//pinMode(SYNC_PIN, OUTPUT);

	pinMode(D_TRIGGER_PIN, OUTPUT);
	pinMode(D_SYNC_PIN, OUTPUT);

	pinMode(IGNITOR_1_PIN, OUTPUT);
	pinMode(IGNITOR_2_PIN, OUTPUT);

	digitalWriteFast(ledPin, LOW);
	digitalWriteFast(INJECTOR_1_PIN, LOW);
	digitalWriteFast(INJECTOR_2_PIN, LOW);
	digitalWriteFast(INJECTOR_3_PIN, LOW);
	digitalWriteFast(INJECTOR_4_PIN, LOW);

	digitalWriteFast(INJECTOR_4_PIN, LOW);
	digitalWriteFast(INJECTOR_4_PIN, LOW);

	digitalWriteFast(IGNITOR_1_PIN, LOW);
	digitalWriteFast(IGNITOR_2_PIN, LOW);

	HWSERIAL.begin(9600);
	TeensyTimerTool::attachErrFunc(ErrorHandler(HWSERIAL));

	///// ADC0 ////
	adc->adc0->setAveraging(16);									// set number of averages
	adc->adc0->setResolution(12);									// set bits of resolution
	adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::MED_SPEED); // change the conversion speed
	adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED);		// change the sampling speed

	///// ADC1 ////
	adc->adc1->setAveraging(16);									// set number of averages
	adc->adc1->setResolution(12);									// set bits of resolution
	adc->adc1->setConversionSpeed(ADC_CONVERSION_SPEED::MED_SPEED); // change the conversion speed
	adc->adc1->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED);		// change the sampling speed

	eRAM.begin();
	eRAM.fs_mount();
	HWSERIAL.println("Loading Maps");

	if (LoadMapsFromRam())
	{
		HWSERIAL.println("Load Maps Error");
	}
	HWSERIAL.println("Setting Interupts");

	attachInterrupt(TRIGGER_PIN, ISR_Trigger_Signal, FALLING); // setup External Pin Interrupts with ISR+=
	attachInterrupt(SYNC_PIN, isrCam_Signal,FALLING);

	CLT_Sensor.SetReturnVal(800);//delete
	MAT_Sensor.SetReturnVal(135);
	MapSensor.SetReturnVal(1013);

	t_Inj1.begin(Injector_1_Close);
	t_Inj2.begin(Injector_2_Close);
	t_Inj3.begin(Injector_3_Close);
	t_Inj4.begin(Injector_4_Close);

	t_Inj1_Wait.begin(Injector_1_Open);
	t_Inj2_Wait.begin(Injector_2_Open);
	t_Inj3_Wait.begin(Injector_3_Open);
	t_Inj4_Wait.begin(Injector_4_Open);

	if (Ignition.GetIgnitionMode() == DISTRIBUTOR)
	{

		t_Ign1_Wait.begin(Ignitor_1_Dwell_Start);
		t_Ign2_Wait.begin(Ignitor_1_Dwell_Start);
		t_Ign3_Wait.begin(Ignitor_1_Dwell_Start);
		t_Ign4_Wait.begin(Ignitor_1_Dwell_Start);

		t_Ign1.begin(Ignitor_1_Spark);
		t_Ign2.begin(Ignitor_1_Spark);
		t_Ign3.begin(Ignitor_1_Spark);
		t_Ign4.begin(Ignitor_1_Spark);
		HWSERIAL.println("Ign Mode -> Distributor");
	}
	if (Ignition.GetIgnitionMode() == WASTED_SPARK)

	{
		t_Ign1_Wait.begin(Ignitor_1_Dwell_Start);
		t_Ign2_Wait.begin(Ignitor_2_Dwell_Start);
		t_Ign3_Wait.begin(Ignitor_1_Dwell_Start);
		t_Ign4_Wait.begin(Ignitor_2_Dwell_Start);

		t_Ign1.begin(Ignitor_1_Spark);
		t_Ign2.begin(Ignitor_2_Spark);
		t_Ign3.begin(Ignitor_1_Spark);
		t_Ign4.begin(Ignitor_2_Spark);
		HWSERIAL.println("Ign Mode -> Wasted");
	}
	threads.setSliceMicros(50);
	//threads.setSliceMillis(10);
	threads.addThread(processCommands, 0, 2048);
	RPM_Interval.begin(tmr_isr_RPM, rpmA(5000)); // Delete
	HWSERIAL.println("Entering Loop");

	
}

void loop()
{

	RunningData.ClockEngineRunningTime();
	RunningData.ClockMainLoopCounter();

	tmr_isr_100_Micros();
	//	tmr_isr_RPM();

	if (RunningData.CalcRpm(Cyl_360_degree_Time))
	{ // New Rpm valve so check it against the Rev limiter
		RpmLimiter.checkRpmLimits(RunningData.GetRPM(),
								  CLT_Sensor.GetTemperature());
	}

	if (MapSensor.calcMap(RunningData.GetRPM(), TpsSensor.GetTPS(),
						  RunningData.IsEngineRunning()))
	{
		BoostCut.checkBoostLimits(MapSensor.getMap()); //New Map valve so check for overboost
	}
	yield();

	Triggers.checkTimeOuts();

	Battery.CalcVoltage();

	CLT_Sensor.CalcTemperature();

	MAT_Sensor.CalcTemperature();

	INJfuel.CalcCrankingVE(CLT_Sensor.GetTemperature());

	FuelPump.checkRunningTime();

	INJfuel.CalcInjectorPeriod();

	Ignition.DoIgnCalcs(CLT_Sensor.GetTemperature(),
						MAT_Sensor.GetTemperature(), Battery.GetVoltage(),
						Idle.getIdleRpmError());

	Ignition.CalcIgnAngle();

	Idle.calcIdleDuty(RunningData.GetRPM(), RunningData.IsEngineRunning(),
					  CLT_Sensor.GetTemperature(), TpsSensor.IsClosedThrottle(),
					  RunningData.GetEngineRunningTime(), MapSensor.isBoosting());
}
