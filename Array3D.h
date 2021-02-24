/*
 * Array3D.h
 *
 *  Created on: 9 Jul. 2019
 *      Author: apaml
 */

#ifndef ARRAY3D_H_
#define ARRAY3D_H_

#include <spiffs_t4.h>
//#include <FastCRC.h>
#include "Arduino.h"
#include "defines.h"
#include "USB.h"

#include "errors.h"

//#define  TX_PACKET_SIZE    32
#define DATA_PACKET_SIZE 27

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define BYTES_IN_A_PACKET DATA_PACKET_SIZE * sizeof(uint16_t) // number of data bytes in a recieved packet

class _Array3D
{
public:
	_Array3D();
	_Array3D(const uint16_t, const uint16_t, const uint16_t, const char *);
	//----------------------------------------------------------------------------------------
	void processCommand(rxData Rxdata)
	{
		

		
		int16_t offset = Rxdata.dataX;
		int16_t data = Rxdata.dataY;
		switch (Rxdata.subCommand)
		{

		case 0XA1:
			setMapValue(offset, data);
			break;
		case 0XA2:
			uploadTableX();
			break;
		case 0XA3:
			downLoadTableX();
			break;
		}
	return;	
	}

	uint16_t getCols()
	{
		return _colCount;
	}
	//----------------------------------------------------------------------------------------
	uint16_t getRows()
	{
		return _rowCount;
	}
	//----------------------------------------------------------------------------------------
	uint16_t getRowCount()
	{
		return DATA(0, 0) & 0x00FF;
	}
	//----------------------------------------------------------------------------------------
	uint16_t getColCount()
	{
		return (DATA(0, 0) & 0xFF00) >> 8;
	}
	//----------------------------------------------------------------------------------------
	uint16_t getWriteCommand(void)
	{
		return _writeCommand;
	}
	//----------------------------------------------------------------------------------------
	void UploadTable(void)
	{
		uploadTable();
	}
	//----------------------------------------------------------------------------------------
	uint16_t getCrc(void)
	{
		return 0;
	}
	//----------------------------------------------------------------------------------------
	void LoadArrayFromFram(void);
	uint16_t GetAxisFrom3dMap(const int16_t, const int16_t);
	int16_t GetFrom2dMap(const int16_t);
	int16_t GetFrom3dMap(const int16_t, const int16_t);
	int16_t GetFrom2dMapX(const int16_t, const int16_t);
	int16_t Get(const uint16_t, const uint16_t);
	void Set(const uint16_t, const uint16_t, const int16_t);
	uint16_t GetFramAddress(const uint16_t, const uint16_t);
	uint16_t LoadNewMap(rxData);

private:
	int16_t bilinearInterpolation(float, float, float, float, float, float,
								  float, float, float, float);
	void loadArrayFromFram(void);
	void uploadTable(void);
	void uploadTableX(void);
	bool downLoadTableX(void);
	bool burnMap2Fram(int16_t *, uint16_t);
	int16_t update_Checksum(int16_t, int16_t);
	int16_t DATA(uint16_t x, uint16_t y);
	void rewriteMap(int16_t, int16_t);
	void setMapValue(int16_t, int16_t);
	void allocate(void);
	int16_t SaveToFlash(const char *, const uint16_t);

	spiffs_file _filePtr;
	const char *_fName;
	uint16_t _maxCols;
	uint16_t _maxRows;
	uint16_t _colCount;
	uint16_t _rowCount;
	uint16_t _newdataCount;
	uint16_t _newRowCount;
	uint16_t _newColCount;
	uint16_t _dataCount;
	uint16_t _crcAddress;
	float _default;
	int16_t *_data;
	int16_t *_ptrData;
	uint16_t buffer[WORD_PACKET_SIZE]; // RawHID packets are always 64 bytes

	//Variables for loading new Table

	uint16_t _writeCommand;
	//uint16_t _Crc;
	uint16_t _numOfBytes;
	uint16_t _packetCounter;
	uint16_t _wordCount;
};

const char VeTable[] = "VeTable";
const char IgnTable[] = "IgnTable";
const char TargetAFR[] = "TargetAFR";
const char IgnDwell[] = "IgnDwell";
const char InjTiming[] = "InjTiming";
const char Predicted[] = "Predicted";
const char Pooling[] = "Pooling";
const char EvapTime[] = "EvapTime";
const char HeatSoak[] = "HeatSoak";
const char ChargeTemp[] = "ChargeTemp";
const char InjDeadTime[] = "InjDeadTime";
const char PostCrankEnrich[] = "PostCrankEnrich";
const char InjFlowRate[] = "InjFlowRate";
const char TPSTrigger[] = "TPSTrigger";
const char CrankingVE[] = "CrankingVE";
const char CrankIgnTiming[] = "CrankIgnTiming";
const char IgnCLTCorr[] = "IgnCLTCorr";
const char IgnMATCorr[] = "IgnMATCorr";
const char MATCalTable[] = "MATCalTable";
const char CLTCalTable[] = "CLTCalTable";
const char MAPCalTable[] = "MAPCalTable";
const char PostCrankIdle[] = "PostCrankIdle";
const char TargetIdleSpeed[] = "TargetIdleSpeed";
const char BaseIdleDuty[] = "BaseIdleDuty";
const char IdleIgnTiming[] = "IdleIgnTiming";
const char TargetBoost1[] = "TargetBoost1";
const char TargetBoost2[] = "TargetBoost2";
const char BoostDuty[] = "BoostDuty";
const char AuxOutput[] = "AuxOutput";
const char RpmLimit[] = "RpmLimit";

extern _Array3D VE_Table;
extern _Array3D Ignition_Table;
extern _Array3D Target_AFR_Table;
extern _Array3D Ignition_Dwell_Table;
extern _Array3D Injector_Timing_Table;
extern _Array3D Predicted_Table;
extern _Array3D Pooling_Table;
extern _Array3D Evap_Time_Table;
extern _Array3D Heat_Soak_Table;
extern _Array3D Charge_Temp_Corr_Table;
extern _Array3D Inj_Dead_Time_Table;
extern _Array3D Post_Crank_Enrich_Table;
extern _Array3D Inj_Flow_Rate_Table;
extern _Array3D TPS_Trigger_Rate_Table;
extern _Array3D Cranking_VE_Table;
extern _Array3D Crank_Ign_Timing_Table;
extern _Array3D Ign_CLT_Correction_Table;
extern _Array3D Ign_MAT_Correction_Table;
extern _Array3D MAT_Calibration_Table;
extern _Array3D CLT_Calibration_Table;
extern _Array3D MAP_Calibration_Table;
extern _Array3D Post_Crank_Idle_Table;
extern _Array3D Target_Idle_Speed_Table;
extern _Array3D Base_Idle_Duty_Cycle_Table;
extern _Array3D Idle_Ign_Timing_Trim_Table;
extern _Array3D Target_Boost_1_Table;
extern _Array3D Target_Boost_2_Table;
extern _Array3D Boost_Duty_Cycle_Table;
extern _Array3D Aux_Output_PWM_Table;
extern _Array3D RPM_Limit_Table;

#endif /* ARRAY3D_H_ */
