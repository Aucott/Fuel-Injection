/*
 * Injection_ISR.h
 *
 *  Created on: 21 Aug. 2018
 *      Author: apaml
 */

#ifndef INJECTION_ISR_H_
#define INJECTION_ISR_H_

#include "Arduino.h"
#include "ICosworth.h"
#include <ADC.h>
#include "TeensyTimerTool.h"
#include "Triggers.h"

using namespace TeensyTimerTool;
extern OneShotTimer  t_Ign1_Wait;
extern OneShotTimer  t_Ign2_Wait;
extern OneShotTimer  t_Ign3_Wait;
extern OneShotTimer  t_Ign4_Wait;

extern OneShotTimer  t_Ign1; // first free channel of TMR1
extern OneShotTimer  t_Ign2;
extern OneShotTimer  t_Ign3;
extern OneShotTimer  t_Ign4;

extern OneShotTimer  t_Inj1_Wait;
extern OneShotTimer  t_Inj2_Wait;
extern OneShotTimer  t_Inj3_Wait;
extern OneShotTimer  t_Inj4_Wait;

extern OneShotTimer  t_Inj1; // first free channel of TMR2
extern OneShotTimer  t_Inj2;
extern OneShotTimer  t_Inj3;
extern OneShotTimer  t_Inj4;


//----------------------------------------------------------------------------------------

void Cyl_Counter_Tick(void);
void ISR_Trigger_Signal(void);
void ISR_Running_Trigger(void);
void isrCam_Signal(void);
void tmr_isr_100_Micros(void);
void tmr_isr_RPM(void);


void Injector_1_Open(void);
void Injector_2_Open(void);
void Injector_3_Open(void);
void Injector_4_Open(void);

void Injector_1_Close(void);
void Injector_2_Close(void);
void Injector_3_Close(void);
void Injector_4_Close(void);

void Ignitor_1_Dwell_Start(void);
void Ignitor_2_Dwell_Start(void);

void Ignitor_1_Spark(void);
void Ignitor_2_Spark(void);




/***************************************************************************/

enum {
	Cyl_1, Cyl_2, Cyl_3, Cyl_4, Cyl_5, Cyl_6,

};

#define Degrees_Per_Rev 36.0
#define Degrees_Per_Tooth 90.0

/***************************************************************************/

#define TRUE 1
#define FALSE 0

#endif /* INJECTION_ISR_H_ */
