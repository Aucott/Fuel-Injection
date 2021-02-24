#include "Injection_ISR.h"

/***************************************************************************/
#define rpmA(input) 400000 / ((input / 60.0) * 4.0)

//Timers
OneShotTimer t_Ign1_Wait(PIT);
OneShotTimer t_Ign2_Wait(PIT);
OneShotTimer t_Ign3_Wait(PIT);
OneShotTimer t_Ign4_Wait(PIT);

OneShotTimer t_Inj1_Wait(TCK);
OneShotTimer t_Inj2_Wait(TCK);
OneShotTimer t_Inj3_Wait(TCK);
OneShotTimer t_Inj4_Wait(TCK);

OneShotTimer t_Ign1(TMR1); // first free channel of TMR1
OneShotTimer t_Ign2(TMR1);
OneShotTimer t_Ign3(TMR1);
OneShotTimer t_Ign4(TMR1);

OneShotTimer t_Inj1(TMR2); // first free channel of TMR2
OneShotTimer t_Inj2(TMR2);
OneShotTimer t_Inj3(TMR2);
OneShotTimer t_Inj4(TMR2);

volatile uint32_t Cyl_360_degree_Time = 0;
volatile uint32_t Rpm_Buffer[5];

uint8_t ignitorMap[] = {1, 2, 3, 4};
uint8_t injectorMap[] = {2, 3, 4, 1};

uint16_t dwell_time[4];

elapsedMicros triggerClock;
elapsedMicros AdcClock;
elapsedMicros RpmClock;

/***************************************************************************/

void ISR_Trigger_Signal(void)
{

	if (Triggers.clockCrankTrigger())
	{ // checks for noise

		ISR_Running_Trigger();
		RunningData.SetCalcRpmFlag();
	}

	//	if (RunningData.IsEngineRunning()) {

	//} else {
	//ISR_Cranking_Trigger();
	//	}
	//return;
}

/***************************************************************************/
void ISR_Running_Trigger(void)
{ //crank trigger when engine is running

	//digitalWriteFast(AUX_PIN_1, 1);
	uint8_t Injector;
	uint8_t Ignitor;
	float Injector_Open_Time_uS;

	uint32_t Current_Micro_Count;

	bool TdcTrigger = false;
	bool BtdcTrigger = false;
	//digitalWriteFast(INJECTOR_1_PIN,!digitalReadFast(INJECTOR_1_PIN)); //open injector 1

	Current_Micro_Count = micros();

	//Calc rpm-------------------------------------------------------------------------------------

	//	uint32_t previous_Cyl_Revolution_Time = Rpm_Buffer[4] - Rpm_Buffer[0];

	Rpm_Buffer[0] = Rpm_Buffer[1]; // store the last four trigger times
	Rpm_Buffer[1] = Rpm_Buffer[2];
	Rpm_Buffer[2] = Rpm_Buffer[3];
	Rpm_Buffer[3] = Rpm_Buffer[4];
	Rpm_Buffer[4] = Current_Micro_Count;

	FuelPump.reset_runtime();

	if (!Triggers.getTriggerFlag())
		return;

	uint32_t tmp_Revolution_Time = Rpm_Buffer[4] - Rpm_Buffer[0]; //get time for one rev for each cyl

	//uint32_t Cyl_Revolution_dt = previous_Cyl_Revolution_Time
	//	- tmp_Revolution_Time;

	//Cyl_360_degree_Time = tmp_Revolution_Time - Cyl_Revolution_dt;//get time for one rev for each cyl
	Cyl_360_degree_Time = tmp_Revolution_Time;

	//Calc rpm-------------------------------------------------------------------------------------

	if (!Triggers.getSyncFlag())
	{ //sync flag expired no point so exit function
		return;
	}

	switch (Triggers.getTriggerIndex())
	{

	case 9:
		Errors.setError(ERR_SYNC_MISSING);
		//Only here if missed sync
		//set count back to 0 and continue to trigger 1
		Triggers.ResetTriggerIndex();
		//no break GO TO CASE 1
	case 1:						//90  // Set up for cosworth can move to BDC interrupts
		INJfuel.SetCalcFlag(0); //set flag to do injector period calcs
		Ignition.SetCalcFlag();
		BtdcTrigger = true;
		break;
	case 2: //180 BDC CYL3 FIRING
		MapSensor.setCalcFlag();
		Injector = injectorMap[0]; // get injector firing order
		Ignitor = ignitorMap[0];
		Injector_Open_Time_uS = INJfuel.GetInjPeriod_1();
		TdcTrigger = true;
		break;
	case 3:						//270
		INJfuel.SetCalcFlag(1); //set flag to do injector period calcs
		Ignition.SetCalcFlag();
		BtdcTrigger = true;
		break;
	case 4: //360 TDC  CYL4 FIRING
		MapSensor.setCalcFlag();
		Injector = injectorMap[1]; // get injector firing order
		Ignitor = ignitorMap[1];
		Injector_Open_Time_uS = INJfuel.GetInjPeriod_2();
		TdcTrigger = true;
		break;
	case 5:						//450
		INJfuel.SetCalcFlag(2); //set flag to do injector period calcs
		Ignition.SetCalcFlag();
		BtdcTrigger = true;
		break;
	case 6: //540 BDC CYL2 FIRING
		MapSensor.setCalcFlag();
		Ignition.SetCalcFlag();
		Injector = injectorMap[2]; // get injector firing order
		Ignitor = ignitorMap[2];
		Injector_Open_Time_uS = INJfuel.GetInjPeriod_3();
		TdcTrigger = true;
		break;
	case 7:						//630
		INJfuel.SetCalcFlag(3); //set flag to do injector period calcs
		Ignition.SetCalcFlag();
		BtdcTrigger = true;
		break;
	case 8: //0 TDC  CYL1 FIRING
		MapSensor.setCalcFlag();
		Ignition.SetCalcFlag();
		Injector = injectorMap[3]; // get injector firing order
		Ignitor = ignitorMap[3];
		Injector_Open_Time_uS = INJfuel.GetInjPeriod_4();
		TdcTrigger = true;
		break;
	default:
		TdcTrigger = false;
		BtdcTrigger = false;
		break;
	}

	float microsPerDegree = Cyl_360_degree_Time / Degrees_Per_Rev;

	if (BtdcTrigger)
	{
	}

	if (TdcTrigger)
	{ //Fuel
		float injector_Wait_uS;

		//if (!(RpmLimiter.IsFuelCut() | !BoostCut.IsFuelCut()))
		{ //Fuel cut

			//Fuel cut
			float injector_End_uS = ((microsPerDegree * INJfuel.GetInjectionTiming()) / 100.0); //

			if (injector_End_uS < Injector_Open_Time_uS)
			{
				injector_Wait_uS = 1;
			}
			else
			{
				injector_Wait_uS = injector_End_uS - Injector_Open_Time_uS;
			}

			switch (Injector)
			{
			case 1:
				t_Inj1_Wait.trigger(injector_Wait_uS);
				break;
			case 2:
				t_Inj2_Wait.trigger(injector_Wait_uS);
				break;
			case 3:
				t_Inj3_Wait.trigger(injector_Wait_uS);
				break;
			case 4:
				t_Inj4_Wait.trigger(injector_Wait_uS);
				break;
			}
		}

		if ((!RpmLimiter.isIgnCut() | !BoostCut.isIgnCut()))
		{ //Ign cut

			uint16_t dwell = Ignition.GetDwellTime();

			float ignitor_Fire_Micros = ((microsPerDegree * Ignition.GetIgnAngle()) / 100.0);

			float ignitor_Wait_Micros = ignitor_Fire_Micros - dwell;

			Ignition.RecordActualDwell(
				ignitor_Fire_Micros - ignitor_Wait_Micros); //Store dwell time for interest

			switch (Ignitor)
			{
			case 1:
				dwell_time[0] = dwell;
				t_Ign1_Wait.trigger(ignitor_Wait_Micros);
				break;
			case 2:
				dwell_time[1] = dwell;
				t_Ign2_Wait.trigger(ignitor_Wait_Micros);
				break;
			case 3:
				dwell_time[0] = dwell;
				t_Ign3_Wait.trigger(ignitor_Wait_Micros);
				break;
			case 4:
				dwell_time[1] = dwell;
				t_Ign4_Wait.trigger(ignitor_Wait_Micros);
				break;
			}
		}
	}
	if (RpmLimiter.isIgnCut())
	{
		HWSERIAL.println("Ign Cut 1");
	}

	if (RpmLimiter.isFuelCut())
	{
		HWSERIAL.println("Fuel Cut 1");
	}

	if (BoostCut.isIgnCut())
	{
		HWSERIAL.println("Ign Cut 2");
	}

	if (BoostCut.isFuelCut())
	{
		HWSERIAL.println("Fuel Cut 2");
	}
asm("DSB");
	return;
}

/***************************************************************************/
void isrCam_Signal(void)
{
	Triggers.clockSyncTrigger();
asm("DSB");
	return;
}

/***************************************************************************/

void Ignitor_1_Spark(void)
{

	digitalWriteFast(IGNITOR_1_PIN, 0);
	return;
}

/***************************************************************************/

void Ignitor_2_Spark(void)
{

	digitalWriteFast(IGNITOR_2_PIN, 0);
	return;
}

/***************************************************************************/

void Ignitor_1_Dwell_Start(void)
{

	digitalWriteFast(IGNITOR_1_PIN, 1);

	t_Ign1.trigger(dwell_time[0]);
	return;
}

/***************************************************************************/

void Ignitor_2_Dwell_Start(void)
{

	digitalWriteFast(IGNITOR_2_PIN, 1);

	t_Ign2.trigger(dwell_time[1]);
	return;
}

/***************************************************************************/

void Injector_1_Open(void)
{

	digitalWriteFast(INJECTOR_1_PIN, 1); //open injector 1

	t_Inj1.trigger(INJfuel.GetInjPeriod_1());
	return;
}

/***************************************************************************/

void Injector_2_Open(void)
{

	digitalWriteFast(INJECTOR_2_PIN, 1); //open injector 2

	t_Inj2.trigger(INJfuel.GetInjPeriod_2());
	return;
}

/***************************************************************************/

void Injector_3_Open(void)
{

	digitalWriteFast(INJECTOR_3_PIN, 1); //open injector 3

	t_Inj3.trigger(INJfuel.GetInjPeriod_3());

	return;
}

/***************************************************************************/

void Injector_4_Open(void)
{

	digitalWriteFast(INJECTOR_4_PIN, 1); //TODO  Change back to Injector

	t_Inj4.trigger(INJfuel.GetInjPeriod_4());
	return;
}

/***************************************************************************/

void Injector_1_Close(void)
{

	digitalWriteFast(INJECTOR_1_PIN, 0); //close injector1
	return;
}

/***************************************************************************/

void Injector_2_Close(void)
{

	digitalWriteFast(INJECTOR_2_PIN, 0); //close injector1
	return;
}

/***************************************************************************/

void Injector_3_Close(void)
{

	digitalWriteFast(INJECTOR_3_PIN, 0); //close injector1
	return;
}

/***************************************************************************/

void Injector_4_Close(void)
{

	digitalWriteFast(INJECTOR_4_PIN, 0); //TODO
	return;
}

//***************************************************************************/
// when the measurement finishes, this will be called
// first: see which pin finished and then save the measurement into the correct buffer
void adc0_isr()
{

	uint8_t pin = ADC::sc1a2channelADC0[ADC1_HC0 & 0x1f]; // the bits 0-4 of ADC0_SC1A have the channel
	switch (pin)
	{ // add value to correct buffer
	case MAP_PIN:
		MapSensor.addValue(adc->readSingle(ADC_0));
		break;
	case BATT_PIN:
		Battery.AddValue(adc->readSingle(ADC_0));
		break;
	case TPS_PIN:
		TpsSensor.AddValue(adc->readSingle(ADC_0));
		break;
	case CLT_PIN:
		CLT_Sensor.AddValue(adc->readSingle(ADC_0));
		break;
	case MAT_PIN:
		MAT_Sensor.AddValue(adc->readSingle(ADC_0));
		break;

	default:
		adc->readSingle(ADC_0); // clear flag anyway
	}
	asm("DSB");
}
/***************************************************************************/
// when the measurement finishes, this will be called
// first: see which pin finished and then save the measurement into the correct buffer
void adc1_isr()
{

	uint8_t pin = ADC::sc1a2channelADC1[ADC1_HC1 & 0x1f]; // the bits 0-4 of ADC0_SC1A have the channel

	switch (pin)
	{ // add value to correct buffer
	case MAP_PIN:
		MapSensor.addValue(adc->readSingle(ADC_1));
		break;
	case BATT_PIN:
		Battery.AddValue(adc->readSingle(ADC_1));
		break;
	case TPS_PIN:
		TpsSensor.AddValue(adc->readSingle(ADC_1));
		break;
	case CLT_PIN:
		CLT_Sensor.AddValue(adc->readSingle(ADC_1));
		break;

	case MAT_PIN:
		MAT_Sensor.AddValue(adc->readSingle(ADC_1));
		break;

	default:
		adc->readSingle(ADC_1); // clear flag anyway
	}

	asm("DSB");
}

/********************************** *****************************************/

void tmr_isr_100_Micros(void)
{

	if (AdcClock >= 100)
	{ // do once every sample time
		AdcClock = AdcClock - 100;

		static uint16_t AdcCounter = 0;

		if (AdcCounter++ == 1000)
		{
			adc->startSingleRead(BATT_PIN, ADC_1);
			adc->startSingleRead(TPS_PIN, ADC_0);
			AdcCounter = 0;
		}
		else
		{
			adc->startSingleRead(MAP_PIN, ADC_1);
		}

		switch (AdcCounter)
		{ // add value to correct buffer
		case 100:
			adc->startSingleRead(TPS_PIN, ADC_0);
			break;
		case 150:
			adc->startSingleRead(CLT_PIN, ADC_0);

			break;
		case 200:
			adc->startSingleRead(TPS_PIN, ADC_0);
			break;
		case 250:
			adc->startSingleRead(MAT_PIN, ADC_0);
			break;
		case 300:
			adc->startSingleRead(TPS_PIN, ADC_0);
			break;
		case 400:
			adc->startSingleRead(TPS_PIN, ADC_0);
			break;
		case 500:
			adc->startSingleRead(TPS_PIN, ADC_0);
			break;
		case 600:
			adc->startSingleRead(TPS_PIN, ADC_0);
			break;
		case 700:
			adc->startSingleRead(TPS_PIN, ADC_0);
			break;
		case 800:
			adc->startSingleRead(TPS_PIN, ADC_0);
			break;
		case 900:
			adc->startSingleRead(TPS_PIN, ADC_0);
			break;
		}
	}
	return;
}

/********************************** *****************************************/

void tmr_isr_RPM(void)
{
	const uint16_t t = rpmA(5000);
	static int f = 0;

	if (RpmClock >= t)
	{ // do once every sample time
		RpmClock = RpmClock - t;

		switch (f++)
		{
		case 0:
			digitalWriteFast(D_TRIGGER_PIN, HIGH);
		//	ISR_Trigger_Signal();
			digitalWrite(13, !digitalRead(13));
			break;
		case 1:
			digitalWriteFast(D_TRIGGER_PIN, LOW);
			digitalWriteFast(D_SYNC_PIN, HIGH);
		//	isrCam_Signal();
			break;
		case 2:
			digitalWriteFast(D_TRIGGER_PIN, HIGH);
			digitalWriteFast(D_SYNC_PIN, LOW);
		//	ISR_Trigger_Signal();
			break;
		case 3:
			digitalWriteFast(D_TRIGGER_PIN, LOW);
			break;
		case 4:
			digitalWriteFast(D_TRIGGER_PIN, HIGH);
		//	ISR_Trigger_Signal();
			break;
		case 5:
			digitalWriteFast(D_TRIGGER_PIN, LOW);
			digitalWriteFast(D_SYNC_PIN, HIGH);
		//	isrCam_Signal();
			break;
		case 6:
			digitalWriteFast(D_TRIGGER_PIN, HIGH);
			digitalWriteFast(D_SYNC_PIN, LOW);
		//	ISR_Trigger_Signal();
			break;
		case 7:
			digitalWriteFast(D_TRIGGER_PIN, LOW);
			break;
		case 8:
			digitalWriteFast(D_TRIGGER_PIN, HIGH);
		//	ISR_Trigger_Signal();
			break;
		case 9:
			digitalWriteFast(D_TRIGGER_PIN, LOW);
			break;
		case 10:
			digitalWriteFast(D_TRIGGER_PIN, HIGH);
		//	ISR_Trigger_Signal();
			break;
		case 11:
			digitalWriteFast(D_TRIGGER_PIN, LOW);
			break;
		case 12:
			digitalWriteFast(D_TRIGGER_PIN, HIGH);
		//	ISR_Trigger_Signal();
			break;
		case 13:
			digitalWriteFast(D_TRIGGER_PIN, LOW);
			break;
		case 14:
			digitalWriteFast(D_TRIGGER_PIN, HIGH);
		//	ISR_Trigger_Signal();
			break;
		case 15:
			digitalWriteFast(D_TRIGGER_PIN, LOW);

			f = 0;
			break;
		default:
			f = 0;
		}
	}

	return;
}
