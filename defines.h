/*
 * defines.h
 *
 *  Created on: 12 Oct. 2019
 *      Author: apaml
 */

#ifndef DEFINES_H_
#define DEFINES_H_
#pragma once

#include <spiffs_t4.h>
#include <ADC.h>
#include <FastCRC.h>

#define HWSERIAL Serial7

#define WORDSIZE  sizeof(int16_t)

#define SCRATCHPADSIZE 0x440
//extern EXTMEM int16_t ScratchPad[SCRATCHPADSIZE];
extern  int16_t ScratchPad[SCRATCHPADSIZE];

extern spiffs_t4 eRAM;
extern ADC *adc; // adc object
 extern FastCRC16 CRC16;
extern volatile uint32_t Cyl_360_degree_Time;

#define CPU_RESTART_ADDR (uint32_t *)0xE000ED0C
#define CPU_RESTART_VAL 0x5FA0004
#define CPU_RESTART (*CPU_RESTART_ADDR = CPU_RESTART_VAL);

#define ADC_FILTER(input, alpha, prior) (((long)input * (256 - alpha) + ((long)prior * alpha))) >> 8

//#define ADC_FILTER(input, alpha, prior) (((int32_t)input * (256 - alpha) + ((int32_t)prior * alpha))) >> 8
//Handy bitsetting macros
#define BIT_SET(a,b) ((a) |= (1U<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1U<<(b)))
#define BIT_CHECK(var,pos) !!((var) & (1U<<(pos)))

#define lowWord(w) ((uint16_t)((w) & 0xFFFF))
#define highWord(w) ((uint16_t)((w) >> 16))

#define REBOOT		0xFFF

#define SHORT_GND		20
#define SHORT_PWR		4075

#define ENGINE_STALLED 	-1
#define ENGINE_CRANKING	 0
#define ENGINE_RUNNING 	1
#define  STALL_RPM    	50

/****************************************************************************/

//Map Size Constants
#define  	ROWS_1					    	2
#define  	ROWS_16					    	17
#define 	ROWS_32      					33

#define  	COLS_9					    	10
#define  	COLS_16					    	17
#define  	COLS_18				    		19
#define  	COLS_20				    		21
#define  	COLS_26			    			27
#define  	COLS_32					    	33


//****************************************************************************/
//FUEL CONSTANTS

#define		SETTINGS_PACKET_1				0X1001
#define		SETTINGS_PACKET_2				0X1002
#define		SETTINGS_PACKET_3				0X1003

#define		LOAD_SETTINGS					0X1004

#define		INJ_FUEL_SETTINGS				0X1005

//TPS CONSTANTS
#define		TPS_SETTINGS					0X1006
#define		LEARN_CLOSED_THROTTLE			0X1007
#define		LEARN_OPEN_THROTTLE				0X1008
#define		SET_TPS_OPEN_VALUE				0X1009
#define		SET_TPS_CLOSED_VALUE   			0X100A
#define		SET_TPS_CLOSED_THRESHOLD		0X100B
#define		SET_TPS_OPEN_THRESHOLD			0X100C

//IGN CONSTANTS
#define		IGN_SETTINGS					0X100D

//TRIGGER CONSTANTS
#define		TRIGGER_SETTINGS				0X100E

//FUEL PUMP CONSTANTS
#define		FUEL_PUMP_SETTINGS				0X100F

//THERMAL FAN CONSTANTS
#define		THERMAL_FAN_SETTINGS			0X1010

//IDLE CONSTANTS
#define		IDLE_SETTINGS					0X1011

//BOOST CONSTANTS
#define		BOOST_SETTINGS					0X1012

//RUNNING DATA CONSTANTS
#define		RUNNING_DATA_SETTINGS			0X1013

//TABLE CONSTANTS
#define		MAP_SETTINGS					0X1014

//RPM LIMITER CONSTANTS
#define	 	RPM_LIMITER_SETTINGS			0X1015

//BOOST CUT CONSTANTS
#define	 	BOOST_CUT_SETTINGS				0X1016

//ERROR CODES
#define		 ERRORS							0X1017
#define		 CLEAR_ERROR_CODES				0X1018
#define		 CLEAR_ERROR_CODE				0X1019


#define 	UPDATE_DATA							200

//****************************************************************************/

//ECU constants

#define		GET_RUNNING_DATA				0X600
#define 	GET_RPM 						OX601
#define 	GET_BATTERY_VOLT				0X602
#define 	GET_THROTTLE_POS				0X603
#define 	GET_CLT							0X604
#define 	GET_MAT							0X605
#define 	GET_MAP							0X606
#define 	GET_IGN_ANGLE					0X607
#define 	GET_INJ_OPEN_TIME				0X608
#define 	XXXXXX      					0X609
#define 	GET_FUEL_TABLE_ARRAY			0X610

#define 	GET_IGN_TABLE_ARRAY				0X611
#define 	GET_lAMBDA_TABLE_ARRAY			0X612
#define 	GET_SEQ_TABLE_ARRAY				0X613

#define   	GET_MAT_Calibration_Map_ADC		0X620
#define  	GET_CLT_Calibration_Map_ADC		0X621
#define 	GET_MAP_Calibration_Map_ADC		0X622

#define 	GET_MAP_Calibration_Map_X		0X625

#define 	GET_MAP_Calibration_Map_XY		0X628
#define     GET_MAT_Calibration_Map_XY	    0X629
#define		GET_CLT_Calibration_Map_XY		0X630
#define 	GET_ENGINE_SETTINGS				0X631

#define 	GET_CLOSED_THROTTLE_ADC         0X632
#define 	GET_OPEN_THROTTLE_ADC           0X633
#define 	GET_CLOSED_THROTTLE_CAL         0X634
#define 	GET_OPEN_THROTTLE_CAL           0X635
#define 	GET_TABLES						0X636

#define     SET_NUM_OF_CYLINDERS    		0X408

#define	  	SET_CYL_SIZE            		0X410

#define   	SET_MAT_Calibration_Map_ADC   	0X411
#define  	SET_CLT_Calibration_Map_ADC   	0X412
#define 	SET_MAP_Calibration_Map_ADC   	0X413
#define   	SET_MAT_Calibration_Map_X     	0X414
#define  	SET_CLT_Calibration_Map_X     	0X415
#define 	SET_MAP_Calibration_Map_X     	0X416

#define		SET_CLOSED_THROTTLE_CAL 		0X425
#define 	SET_OPEN_THROTTLE_CAL   		0X426
#define 	SET_CLOSED_THROTTLE_ADC 		0X427
#define 	SET_OPEN_THROTTLE_ADC   		0X428
#define 	SET_TRIGGER_SENSOR_TYPE 		0X429
#define 	SET_TRIGGER_EDGE        		0X430

#define 	SET_TRIGGER_PULLUP      		0X431

#define  	SET_SYNC_EDGE          			0X433

#define 	SET_IGN_TYPE            		0X435
#define 	SET_NUM_TRIGGER_EVENTS  		0X436
#define 	SET_NO_CYLS             		0X437
#define 	SET_NUM_OF_IGNITORS     		0X438
#define 	SET_IGN_FIRE_EDGE       		0X439
#define 	SET_IGN_DELAY		    		0X440

#define 	SET_IGN_DWELL           		0X441
#define 	SET_FLOW_RATE           		0X442
#define 	SET_ACCEL_ENRICH        		0X443
#define 	SET_ENRICH_DURATION     		0X444
#define 	SET_SOFT_REV_LIMIT      		0X445
#define 	SET_HARD_REV_LIMIT      		0X446
#define 	SET_BOOST_CUT           		0X447
#define 	SET_DECEL_CUT           		0X448
#define		SET_INJECTOR_DRIVER     		0X449
#define		SET_MIN_PULSE_WIDTH     		0X450

#define	 	SET_STOICHIOMETRIC      		0X452
#define	 	SET_DECELERATION_CUT    		0X453
#define	 	LOAD_TABLES_FROM_FRAM    		0X454
#define 	SET_RPM                 		0X455
#define 	SET_BATTERY_VOLT        		0X456
#define 	SET_THROTTLE_POS        		0X457
#define 	SET_CLT_TEMP            		0X458
#define 	SET_MAT                 		0X459
#define 	SET_MAP                 		0X460

#define 	SET_IGN_ANGLE           		0X461
#define 	SET_INJ_OPEN_TIME       		0X462
#define 	SET_LAMBDA_DATA         		0X463
#define		SET_FUEL_OVERALL_CORR		    0X464
#define 	SET_INJECTOR_TIMING             0X465
#define		SET_INJECTOR_TIMING_POS         0X466

#define	  	SET_IGN_MAT_Calibration_Map     0X506
#define		SET_IGN_CLT_Calibration_Map     0X507

#define   	SET_FUEL_MAT_Calibration_Map	0X510
#define  	SET_FUEL_CLT_Calibration_Map	0X511
#define  	SET_CHARGE_CORR_TABLE			0X512

#define     SET_MAT_Calibration_Map			0X514
#define     SET_CLT_Calibration_Map			0X515
#define     SET_MAP_Calibration_Map			0X516
#define 	SET_INJ_DUTY					0X517
#define 	SET_POST_CRANK_IDLE_TABLE		0X518

#define		SET_CHARGE_TEMPERATURE			0X527
#define	   	SET_RAW_IGN_ANGLE            	0X528
#define	   	SET_CRANKING_IGN_ANGLE		 	0X529
#define 	SET_TARGET_LAMBDA				0X530
#define 	SET_FUEL_PRESSURE				0X531
#define 	SET_DEAD_TIME					0X532
#define 	SET_AIR_MASS					0X533
#define 	SET_FUEL_MG						0X534
#define		SET_VE							0X535
#define		SET_INJ_FLOW_RATE				0X536
#define		SET_INJ_X						0X537
#define		SET_INJ_TAU						0X538
#define		SET_INJ_PUDDLE_SIZE				0X539
#define		SET_INJ_TAU_DT					0X540
#define		SET_INJ_FILM_ENRICH				0X541
#define		SET_THROTTLE_DELTA				0X542
#define		SET_TPS_TRIGGER					0X543

#define     PREDICTED_MAP_IS_RUNNING		0X545

#define     GET_ENGINE_RUNNING_TIME         0X549
#define		GET_POST_CRANK_ENRICH			0X550
#define     GET_CRANKING_VE					0X551

#define 	SET_AUX_PWM_TABLE				0X559

#define		GET_TARGET_IDLE_RPM             0X561
#define		GET_IDLE_DUTY                	0X562
#define		GET_IDLE_BASE_DUTY				0X563
#define     GET_CLT_ADC   		     		0X564
#define     GET_MAT_ADC        				0X565
#define     GET_MAP_ADC        				0X566
#define		GET_IDLE_PID_DUTY				0X567
#define		GET_TARGET_BOOST				0X568
#define		GET_WASTEGATE_BASE_DUTY			0X569
#define		GET_WASTEGATE_PID				0X570
#define		GET_WASTEGATE_DUTY				0X571
#define		IS_ENGINE_RUNNING				0X572
#define		SET_ACTUAL_IGN_DWELL			0X573
#define 	SET_MAIN_LOOP_COUNTER			0X574

#define     GET_INJ_TIMING					0X576
#define		SET_LAMBDA						0X577

//CREATE NEW TABLE DOWNLOADED FROM PC

//#define		RECEIVE_ACK								99
#define 	CREATE_VE_TABLE							100
#define 	CREATE_IGN_TABLE						101
#define 	CREATE_TARGETAFR_TABLE					102
#define		CREATE_INJ_TIMING_TABLE					103
#define		CREATE_IGN_CRANKING_TABLE				104
#define		CREATE_IGN_DWELL_TABLE					105
#define		CREATE_PREDICTED_TABLE					106
#define		CREATE_X_POOLING_TABLE					107
#define		CREATE_TAU_EVAP_TABLE					108
#define 	CREATE_HEAT_SOAK_TABLE					109
#define 	CREATE_CHARGE_TEMP_CORR_TABLE			110
#define		CREATE_INJ_FLOW_RATE_TABLE				111
#define 	CREATE_INJ_DEAD_TIME_TABLE				112
#define		CREATE_TPS_TRIGGER_RATE_TABLE			113
#define		CREATE_CRANKING_VE_TABLE				114
#define		CREATE_MAT_CAL_TABLE					115
#define		CREATE_CLT_CAL_TABLE					116
#define		CREATE_MAP_CAL_TABLE					117
#define     CREATE_FUEL_PRESSURE_TABLE   			118
#define		CREATE_Post_Crank_Enrich_Map			119
#define		CREATE_INJ_CLT_CORR_TABLE				120
#define		CREATE_INJ_MAT_CORR_TABLE				121
#define		CREATE_IGN_CLT_CORR_TABLE				122
#define		CREATE_IGN_MAT_CORR_TABLE				123
#define		CREATE_TARGET_IDLE_TABLE				124
#define		CREATE_BASE_IDLE_DUTY_TABLE				125
#define		CREATE_TARGET_BOOST_1_TABLE				126
#define		CREATE_TARGET_BOOST_2_TABLE				127
#define		CREATE_BOOST_DUTY_TABLE					128
#define		CREATE_AUX_PWM_TABLE					129

//NORMAL WRITE TO RAM
//#define 	SET_VE_TABLE_XY							200
#define 	SET_IGN_TABLE_XY						201
#define 	SET_TARGETAFR_TABLE_XY					202
#define		SET_IGN_DWELL_TABLE_XY					203
#define     SET_INJ_TIMING_TABLE_XY					204
#define		SET_PREDICTED_TABLE_XY					205
#define		SET_X_POOLING_TABLE_XY					206
#define		SET_TAU_EVAP_TABLE_XY       			207
#define	 	SET_HEAT_SOAK_TABLE_XY					208
#define     SET_CHARGE_TEMP_CORR_TABLE_XY			209
#define     SET_INJ_DEAD_TIME_TABLE_XY				210
#define		SET_POST_START_ENRICH_TABLE_XY       	211
#define		SET_INJ_FLOW_RATE_TABLE_XY   			212
#define		SET_TPS_TRIGGER_RATE_TABLE_XY       	213
#define     SET_CRANKING_VE_TABLE_XY		   		214
#define     SET_CRANKING_IGN_TIMING_TABLE_XY		215
#define     SET_IGN_CLT_CORR_TABLE_XY            	216
#define     SET_IGN_MAT_CORR_TABLE_XY            	217
#define     SET_MAT_CAL_TABLE_XY             		218
#define     SET_CLT_CAL_TABLE_XY            		219
#define     SET_MAP_CAL_TABLE_XY            		220
#define		SET_POST_CRANK_IDLE_TABLE_XY			221
#define		SET_TARGET_IDLE_TABLE_XY				222
#define		SET_BASE_IDLE_DUTY_TABLE_XY				223
#define		SET_IDLE_IGN_TRIM_TABLE_XY				224
#define		SET_TARGET_BOOST_1_TABLE_XY				225
#define		SET_TARGET_BOOST_2_TABLE_XY				226
#define 	SET_BOOST_DUTY_TABLE_XY               	227
#define 	SET_AUX_PWM_TABLE_XY        			228
#define		SET_RPM_LIMIT_TABLE_XY					229
#define		SET_TPS_SETTINGS						230

#define 	LOAD_VE_TABLE							300
#define 	LOAD_IGN_TABLE							301
#define 	LOAD_TARGETAFR_TABLE					302
#define		LOAD_IGN_DWELL_TABLE					303
#define     LOAD_INJ_TIMING_TABLE					304
#define		LOAD_PREDICTED_TABLE					305
#define		LOAD_X_POOLING_TABLE					306
#define		LOAD_TAU_EVAP_TABLE       				307
#define	 	LOAD_HEAT_SOAK_TABLE					308
#define     LOAD_CHARGE_TEMP_CORR_TABLE				309
#define     LOAD_INJ_DEAD_TIME_TABLE				310
#define		LOAD_POST_CRANK_ENRICH_TABLE       		311
#define		LOAD_INJ_FLOW_RATE_TABLE   				312
#define		LOAD_TPS_TRIGGER_RATE_TABLE       		313
#define     LOAD_CRANKING_VE_TABLE		   			314
#define     LOAD_CRANKING_IGN_TIMING_TABLE			315
#define     LOAD_IGN_CLT_CORR_TABLE            		316
#define     LOAD_IGN_MAT_CORR_TABLE            		317
#define     LOAD_MAT_CAL_TABLE           			318
#define     LOAD_CLT_CAL_TABLE            			319
#define     LOAD_MAP_CAL_TABLE           			320
#define		LOAD_POST_CRANK_IDLE_TABLE				321
#define		LOAD_TARGET_IDLE_TABLE					322
#define		LOAD_BASE_IDLE_DUTY_TABLE				323
#define		LOAD_IDLE_IGN_TRIM_TABLE				324
#define		LOAD_TARGET_BOOST_1_TABLE				325
#define		LOAD_TARGET_BOOST_2_TABLE				326
#define 	LOAD_BOOST_DUTY_TABLE               	327
#define 	LOAD_AUX_PWM_TABLE        				328
#define		LOAD_RPM_LIMIT_TABLE					329

#define 	ADJUST_VE_TABLE							400
#define 	ADJUST_IGN_TABLE					   	401
#define 	ADJUST_TARGETAFR_TABLE					402
#define		ADJUST_IGN_DWELL_TABLE					403
#define     ADJUST_INJ_TIMING_TABLE					404
#define		ADJUST_PREDICTED_TABLE					405
#define		ADJUST_X_POOLING_TABLE					406
#define		ADJUST_TAU_EVAP_TABLE       			407
#define	 	ADJUST_HEAT_SOAK_TABLE					408
#define     ADJUST_CHARGE_TEMP_CORR_TABLE			409
#define     ADJUST_INJ_DEAD_TIME_TABLE				410
#define		ADJUST_POST_CRANK_ENRICH_TABLE       	411
#define		ADJUST_INJ_FLOW_RATE_TABLE   			412
#define		ADJUST_TPS_TRIGGER_RATE_TABLE           413
#define     ADJUST_CRANKING_VE_TABLE		   		414
#define     ADJUST_CRANKING_IGN_TIMING_TABLE		415
#define     ADJUST_IGN_CLT_CORR_TABLE            	416
#define     ADJUST_IGN_MAT_CORR_TABLE            	417
#define     ADJUST_MAT_CAL_TABLE           			418
#define     ADJUST_CLT_CAL_TABLE            		419
#define     ADJUST_MAP_CAL_TABLE           			420
#define		ADJUST_POST_CRANK_IDLE_TABLE			421
#define		ADJUST_TARGET_IDLE_TABLE				422
#define		ADJUST_BASE_IDLE_DUTY_TABLE				423
#define		ADJUST_IDLE_IGN_TRIM_TABLE				424
#define		ADJUST_TARGET_BOOST_1_TABLE				425
#define		ADJUST_TARGET_BOOST_2_TABLE				426
#define 	ADJUST_BOOST_DUTY_TABLE               	427
#define 	ADJUST_AUX_PWM_TABLE        			428
#define		ADJUST_RPM_LIMIT_TABLE					429

//****************************************************************************
// Pin Mapping
#define 	TRIGGER_PIN 					31
#define 	SYNC_PIN 						32

#define 	D_TRIGGER_PIN 					34
#define 	D_SYNC_PIN 						33



#define 	INJECTOR_1_PIN					8
#define 	INJECTOR_2_PIN					24
#define 	INJECTOR_3_PIN					25
#define 	INJECTOR_4_PIN					30

#define 	IGNITOR_1_PIN						2
#define 	IGNITOR_2_PIN						3
//#define 	IGN_3_PIN						2
//#define 	IGN_4_PIN						3

#define 	KNOCK_SAMPLE_PIN				38
#define 	AUX_EXTRA_PIN					37

#define 	BOOST_VALVE_PIN					5
#define 	IDLE_VALVE_PIN					4
#define 	FUEL_PUMP_PIN					35
#define 	OUTPUT_1_PIN					33

#define 	BOOST_VALVE_DIAG_PIN			7
#define 	IDLE_VALVE_DIAG_PIN				20
#define 	FUEL_PUMP_DIAG_PIN				19
#define 	OUTPUT_1_DIAG_PIN				21

#define 	AUX_CPU_RESET_PIN				9
#define 	INJ_CPU_RESET_PIN				6

#define 	WHEEL_SPEED_1					36
#define 	WHEEL_SPEED_2					34

#define 	ledPin 							13

/****************************************************************************/
//ADC pins
#define MAP_PIN					A0
#define CLT_PIN					A2
#define MAT_PIN					A3
#define TPS_PIN					A1
#define BATT_PIN  				A4
#define FUEL_PRESSURE_PIN		A8
#define OIL_PRESSURE_PIN		A9

//****************************************************************************/

#endif /* DEFINES_H_ */

