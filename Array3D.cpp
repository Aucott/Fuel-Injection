/*
 * Array3D.cpp
 *
 *  Created on: 9 Jul. 2019
 *      Author: apaml
 */

#include "Array3D.h"

//RAM STORAGE OF TABLES ALLOCATION

_Array3D VE_Table(COLS_32, ROWS_32, LOAD_VE_TABLE, VeTable);

_Array3D Ignition_Table(COLS_32, ROWS_32, LOAD_IGN_TABLE, IgnTable);

_Array3D Target_AFR_Table(COLS_32, ROWS_32,
						  LOAD_TARGETAFR_TABLE, TargetAFR);

_Array3D Ignition_Dwell_Table(COLS_16, ROWS_1, LOAD_IGN_DWELL_TABLE, IgnDwell);

_Array3D Injector_Timing_Table(COLS_16, ROWS_16, LOAD_INJ_TIMING_TABLE,
							   InjTiming);

_Array3D Predicted_Table(COLS_16, ROWS_16,
						 LOAD_PREDICTED_TABLE, Predicted);

_Array3D Pooling_Table(COLS_16, ROWS_16,
					   LOAD_X_POOLING_TABLE, Pooling);

_Array3D Evap_Time_Table(COLS_16, ROWS_16, LOAD_TAU_EVAP_TABLE, EvapTime);

_Array3D Heat_Soak_Table(COLS_16, ROWS_16,
						 LOAD_HEAT_SOAK_TABLE, HeatSoak);

_Array3D Charge_Temp_Corr_Table(COLS_16, ROWS_16,
								LOAD_CHARGE_TEMP_CORR_TABLE, ChargeTemp);

_Array3D Inj_Dead_Time_Table(COLS_16, ROWS_16,
							 LOAD_INJ_DEAD_TIME_TABLE, InjDeadTime);

_Array3D Inj_Flow_Rate_Table(COLS_16, ROWS_1,
							 LOAD_INJ_FLOW_RATE_TABLE, InjFlowRate);

_Array3D TPS_Trigger_Rate_Table(COLS_16, ROWS_1,
								LOAD_TPS_TRIGGER_RATE_TABLE, TPSTrigger);

_Array3D Cranking_VE_Table(COLS_16, ROWS_1,
						   LOAD_CRANKING_VE_TABLE, CrankingVE);

_Array3D Post_Crank_Enrich_Table(COLS_16, ROWS_16,
								 LOAD_POST_CRANK_ENRICH_TABLE, PostCrankEnrich);

_Array3D Crank_Ign_Timing_Table(COLS_16, ROWS_1,
								LOAD_CRANKING_IGN_TIMING_TABLE, CrankIgnTiming);

_Array3D Ign_MAT_Correction_Table(COLS_16, ROWS_1,
								  LOAD_IGN_MAT_CORR_TABLE, IgnMATCorr);

_Array3D Ign_CLT_Correction_Table(COLS_16, ROWS_1,
								  LOAD_IGN_CLT_CORR_TABLE, IgnCLTCorr);

_Array3D MAP_Calibration_Table(COLS_16, ROWS_1,
							   LOAD_MAP_CAL_TABLE, MAPCalTable);

_Array3D CLT_Calibration_Table(COLS_20, ROWS_1,
							   LOAD_CLT_CAL_TABLE, CLTCalTable);

_Array3D MAT_Calibration_Table(COLS_20, ROWS_1,
							   LOAD_MAT_CAL_TABLE, MATCalTable);

_Array3D Post_Crank_Idle_Table(COLS_16, ROWS_16,
							   LOAD_POST_CRANK_IDLE_TABLE, PostCrankIdle);

_Array3D Target_Idle_Speed_Table(COLS_26, ROWS_1,
								 LOAD_TARGET_IDLE_TABLE, TargetIdleSpeed);

_Array3D Base_Idle_Duty_Cycle_Table(COLS_26, ROWS_1,
									LOAD_BASE_IDLE_DUTY_TABLE, BaseIdleDuty);

_Array3D Target_Boost_1_Table(COLS_16, ROWS_16,
							  LOAD_TARGET_BOOST_1_TABLE, TargetBoost1);

_Array3D Target_Boost_2_Table(COLS_16, ROWS_16,
							  LOAD_TARGET_BOOST_2_TABLE, TargetBoost2);

_Array3D Boost_Duty_Cycle_Table(COLS_16, ROWS_16, LOAD_BOOST_DUTY_TABLE,
								BoostDuty);

_Array3D Aux_Output_PWM_Table(COLS_16, ROWS_1, LOAD_AUX_PWM_TABLE, AuxOutput);

_Array3D Idle_Ign_Timing_Trim_Table(COLS_16, ROWS_1,
									LOAD_IDLE_IGN_TRIM_TABLE, IdleIgnTiming);

_Array3D RPM_Limit_Table(COLS_16, ROWS_1,
						 LOAD_RPM_LIMIT_TABLE, RpmLimit);

_Array3D::_Array3D()
{
}
_Array3D::_Array3D(const uint16_t maxCols, const uint16_t maxRows,
				   const uint16_t writeCommand, const char *fname)
{

	_fName = fname;
	_writeCommand = writeCommand;
	_maxCols = maxCols;
	_maxRows = maxRows;
	_data = NULL;
	_newdataCount = 0;
	_newRowCount = 0;
	_newColCount = 0;
	_default = 1;
	//_crcVal=0;
}

//----------------------------------------------------------------------------------------
int16_t _Array3D::DATA(uint16_t x, uint16_t y)
{
	return _data[x * _rowCount + y];
}
//----------------------------------------------------------------------------------------

int16_t _Array3D::GetFrom3dMap(const int16_t Xaxis, const int16_t Yaxis)
{

	int16_t xIndex, yIndex ;
	float x1, x2, y1, y2;

	float X, Y, q11, q12, q21, q22;

	X = Xaxis; //Get map and Rpm values
	Y = Yaxis;

	if (X < DATA(1, 0))
	{ // check lower bounds X-Axis
		X = DATA(1, 0);
	}

	if (X > DATA(_colCount - 1, 0))
	{ // check upper bounds X-Axis
		X = DATA(_colCount - 1, 0);
	}

	for (int i = 1; i < _colCount - 1; i++)
	{ // scroll though axis x for position on axis
		if (X >= DATA(i, 0) && X <= DATA(i + 1, 0))
		{
			xIndex = i;
			break;
		}
	}

	if (Y < DATA(0, 1))
	{ // check lower bounds
		Y = DATA(0, 1);
	}

	if (Y > DATA(0, _rowCount - 1))
	{ // check upper bounds
		Y = DATA(0, _rowCount - 1);
	}

	for (int i = 1; i < _rowCount - 1; i++)
	{ // scroll though axis y for position on axis
		if (Y >= DATA(0, i) && Y <= DATA(0, i + 1))
		{
			yIndex = i;
			break;
		}
	}

	x1 = DATA(xIndex, 0); // get above and below values from table
	x2 = DATA(xIndex + 1, 0);
	y1 = DATA(0, yIndex);
	y2 = DATA(0, yIndex + 1);

	q11 = DATA(xIndex, yIndex);
	q12 = DATA(xIndex, yIndex + 1);
	q21 = DATA(xIndex + 1, yIndex);
	q22 = DATA(xIndex + 1, yIndex + 1);

	return bilinearInterpolation(q11, q12, q21, q22, x1, x2, y1, y2, X, Y);
}
//----------------------------------------------------------------------------------------
uint16_t _Array3D::GetAxisFrom3dMap(const int16_t Xaxis,
									const int16_t DataPoint)
{

	int16_t yIndex;
	int16_t UpperX, LowerX;
	int16_t X;

	X = Xaxis; //Get map and Rpm values

	if (X < DATA(1, 0))
	{ // check lower bounds X-Axis
		X = DATA(1, 0);
	}

	if (X > DATA(_colCount - 1, 0))
	{ // check upper bounds X-Axis
		X = DATA(_colCount - 1, 0);
	}

	for (int i = 1; i < _rowCount - 1; i++)
	{ // scroll though axis x for position on axis
		yIndex = i;
		LowerX = GetFrom2dMapX(X, i);
		UpperX = GetFrom2dMapX(X, i + 1);
		if (DataPoint >= LowerX && DataPoint <= UpperX)
		{
			break;
		}
	}

	return map(DataPoint, LowerX, UpperX, DATA(0, yIndex), DATA(0, yIndex + 1));
}

//----------------------------------------------------------------------------------------

int16_t _Array3D::bilinearInterpolation(float q11, float q12, float q21,
										float q22, float x1, float x2, float y1, float y2, float x, float y)
{
	float fxy1, fxy2, fxy;

	fxy1 = ((x2 - x) / (x2 - x1)) * q11 + ((x - x1) / (x2 - x1)) * q21;
	fxy2 = ((x2 - x) / (x2 - x1)) * q12 + ((x - x1) / (x2 - x1)) * q22;

	fxy = ((y2 - y) / (y2 - y1)) * fxy1 + ((y - y1) / (y2 - y1)) * fxy2;

	return fxy;
}

//----------------------------------------------------------------------------------------

void _Array3D::LoadArrayFromFram(void)
{

	uint16_t crc;
	int16_t err;
	char rows, cols;

	err = eRAM.f_open(_filePtr, _fName, SPIFFS_RDONLY);
	if (err < 0)
	{
		Errors.setErrorFlag();
		HWSERIAL.print("err Open File ");
		HWSERIAL.println(_fName);
		return;
	}

	eRAM.f_read(_filePtr, &rows, 1);
	eRAM.f_read(_filePtr, &cols, 1);

	if (cols > 0 && cols <= _maxCols && rows > 0 && rows <= _maxRows)
	{ //corrupt data

		_colCount = cols;
		_rowCount = rows;
		_dataCount = (_colCount * _rowCount);
		allocate(); // allocate space

		eRAM.f_seek(_filePtr, 0, SPIFFS_SEEK_SET); //Set position to start of file

		eRAM.f_read(_filePtr, reinterpret_cast<const char *>(_data),
					(_dataCount + 1) * WORDSIZE);

		eRAM.f_close(_filePtr);

		crc = CRC16.ccitt(reinterpret_cast<uint8_t *>(_data), //Calculate CRC
						  _dataCount * WORDSIZE);

		if (crc != (uint16_t)_data[_dataCount])
		{
			HWSERIAL.println("ERR_MEM_LOAD_CRC ");
			Errors.setError(ERR_MEM_LOAD_CRC);
			Errors.setErrorFlag();
		}
		else
			HWSERIAL.print("Ok Loaded ");
		HWSERIAL.println(_fName);
	}
}

//----------------------------------------------------------------------------------------

void _Array3D::allocate()
{
	// Allocate.

	if (_data != NULL)
		delete _data;

	_data = new int16_t[_dataCount + 1]; //add space for crc
}
//----------------------------------------------------------------------------------------
void _Array3D::uploadTable(void)
{

	uint16_t TXpacket[WORD_PACKET_SIZE];

	uint16_t packetCount, y, i;

	_data[_dataCount] = CRC16.ccitt(reinterpret_cast<uint8_t *>(_data), //Calculate CRC add to end of array
									_dataCount * WORDSIZE);

	if ((_dataCount + 1) <= DATA_PACKET_SIZE) // get the number of packets we need to send
		packetCount = 1;
	else if ((_dataCount + 1) % DATA_PACKET_SIZE == 0)
	{
		packetCount = (_dataCount + 1) / DATA_PACKET_SIZE;
	}
	else
	{
		packetCount = ((_dataCount + 1) / DATA_PACKET_SIZE) + 1;
	}

	uint16_t dataIndex = 0;

	for (i = 0; i < packetCount; i++)
	{

		//framLow.readWord(_framAddress + (x * sizeof(uint16_t)), &tempValue); //Write array into epprom
		memset(TXpacket, 0, WORD_PACKET_SIZE * sizeof(uint16_t));

		TXpacket[0] = _writeCommand;
		TXpacket[1] = _writeCommand + i;
		TXpacket[2] = packetCount;
		TXpacket[3] = _dataCount; // position of crc int
		for (y = 4; y < WORD_PACKET_SIZE; y++)
		{

			TXpacket[y] = _data[dataIndex++];
			if (dataIndex > _dataCount)
			{
				break;
			}
		}
		RawHID.send(TXpacket, 10);
	}

	HWSERIAL.print("Uploaded ");
	HWSERIAL.println(_fName);
}
//----------------------------------------------------------------------------------------

int16_t _Array3D::GetFrom2dMap(const int16_t Value)
{

	int16_t returnVal = 0;
	int16_t iValue;

	int16_t mapLow = DATA(1, 1);
	int16_t mapHigh = DATA(_colCount - 1, 1);

	if (mapLow <= mapHigh)
	{ //if map values go from low to high (PTC type sensor)
		iValue = Value;
		if (iValue < mapLow)
		{
			iValue = mapLow;
		}
		if (iValue > mapHigh)
		{
			iValue = mapHigh;
		}

		for (int i = 1; i < _colCount - 1; i++)
		{

			if (iValue >= DATA(i, 1) && iValue <= DATA(i + 1, 1))
			{ // size of array stored in [0][0]

				returnVal = map(iValue, DATA(i, 1), // Temperature stored in [1][x+1]
								DATA(i + 1, 1), DATA(i, 0), DATA(i + 1, 0));
				break;
			}
		}
	}
	else
	{ //if map values go from high to low (NTC type sensor)

		iValue = Value;
		if (iValue > mapLow)
			iValue = mapLow;  // ADC value stored in [0][x+1]
		if (iValue < mapHigh) // size of array stored in [0][0]
			iValue = mapHigh;

		for (uint16_t i = 1; i < _colCount - 1; i++)
		{

			if (iValue <= DATA(i, 1) && iValue >= DATA(i + 1, 1))
			{ // size of array stored in [0][0]

				returnVal = map(iValue, DATA(i, 1), // Temperature stored in [1][x+1]
								DATA(i + 1, 1), DATA(i, 0), DATA(i + 1, 0));
				break;
			}
		}
	}
	return returnVal;
}
//----------------------------------------------------------------------------------------

int16_t _Array3D::GetFrom2dMapX(const int16_t Value, int16_t RowNumber)
{

	int16_t returnVal = 0;
	int16_t iValue;

	int16_t mapLow = DATA(1, 0);
	int16_t mapHigh = DATA(_colCount - 1, 0);

	iValue = Value;
	if (iValue < mapLow)
	{
		iValue = mapLow;
	}
	if (iValue > mapHigh)
	{
		iValue = mapHigh;
	}

	for (int i = 1; i < _colCount - 1; i++)
	{

		if (iValue >= DATA(i, 0) && iValue <= DATA(i + 1, 0))
		{ // size of array stored in [0][0]

			returnVal = map(iValue, DATA(i, 0), // Temperature stored in [1][x+1]
							DATA(i + 1, 0), DATA(i, RowNumber), DATA(i + 1, RowNumber));
			break;
		}
	}

	return returnVal;
}

//----------------------------------------------------------------------------------------
bool _Array3D::burnMap2Fram(int16_t *array, uint16_t arraySize)
{

	bool retval = false;

	int16_t crc = CRC16.ccitt(reinterpret_cast<uint8_t *>(array), //Calculate CRC
							  arraySize * WORDSIZE);

	eRAM.f_open(_filePtr, _fName,
				SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR);
	eRAM.f_write(_filePtr, reinterpret_cast<const char *>(array), // Store into fram
				 (arraySize * WORDSIZE) + WORDSIZE);
	eRAM.f_write(_filePtr, reinterpret_cast<const char *>(&crc), //Add CRC to end
				 WORDSIZE);

	eRAM.f_close(_filePtr);

	retval = true;

	return retval;
}

//----------------------------------------------------------------------------------------

int16_t _Array3D::update_Checksum(int16_t crc, int16_t c)
{

	crc += c;
	crc = crc << 3 | crc >> (32 - 3); // rotate a little
	crc ^= 0xFFFFFFFF;				  // invert just for fun

	return crc;
}

//----------------------------------------------------------------------------------------

void _Array3D::rewriteMap(int16_t offset, int16_t data)
{

	//framLow.writeWord(SCRATCHPAD_ADDRESS + (offset * sizeof(int16_t)), data);
}
//----------------------------------------------------------------------------------------

void _Array3D::setMapValue(int16_t offset, int16_t data)
{
	noInterrupts();

	HWSERIAL.print(_fName);
	HWSERIAL.print("-Value Changed->");
	HWSERIAL.println(data);
	_data[offset] = data;
	interrupts();
}

//----------------------------------------------------------------------------------------
int16_t _Array3D::Get(const uint16_t x, const uint16_t y)
{
	return DATA(x, y);
}

//----------------------------------------------------------------------------------------
void _Array3D::Set(const uint16_t col, const uint16_t row, const int16_t data)
{

	if (col > 0 && col <= _maxCols && row > 0 // check range
		&& row <= _maxRows)
	{
		noInterrupts();
		_data[col * _rowCount + row] = data;
		interrupts();
	}
}

//----------------------------------------------------------------------------------------
uint16_t _Array3D::LoadNewMap(rxData Rxdata)
{
	uint16_t crc, Crc;

	if (_writeCommand == Rxdata.subCommand)
	{
		Crc = Rxdata.dataX;		// Check sum in Rxdata
		_numOfBytes = Rxdata.dataY; // Number of bytes to load in array
		_packetCounter = 0;
		_wordCount = 0;
	}

	if ((_writeCommand + _packetCounter) == Rxdata.subCommand)
	{

		//memcpy(ScratchPad + (_packetCounter * 54), Rxdata.dataArray, 54);

		for (int i = 0; i < DATA_PACKET_SIZE; i++)
		{

			if (_wordCount < SCRATCHPADSIZE)
			{ /// check if array is in bounds

				ScratchPad[_wordCount++] = Rxdata.dataArray[i]; // Write to external Ram
			}
		}

		if ((_wordCount * WORDSIZE) >= _numOfBytes)
		{ // The required number of ints have read in

			crc = CRC16.ccitt(reinterpret_cast<uint8_t *>(&ScratchPad),
							  _numOfBytes);

			if (crc == Crc)
			{ // CRC check is OK copy from Ram to Low Fram
				HWSERIAL.print("Downloaded");
				HWSERIAL.println(_fName);
				ScratchPad[(_numOfBytes / WORDSIZE)] = Crc;

				SaveToFlash(reinterpret_cast<const char *>(&ScratchPad),
							_numOfBytes + WORDSIZE);
				LoadArrayFromFram();
			}
			else
			{
				HWSERIAL.println("Crc Fail");
			}
		}

		_packetCounter++;
	}
	else
	{
		//Errors.setError(ERR_MEM_LOAD_CRC);
	}

	return 0;
}
//----------------------------------------------------------------------------------------
int16_t _Array3D::SaveToFlash(const char *data, const uint16_t numBytes)
{

	int16_t err;

	err = eRAM.f_open(_filePtr, _fName,
					  SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR);
	if (err < 0)
	{
		HWSERIAL.println("err Save Open");
		return err;
	}

	err = eRAM.f_write(_filePtr, data, numBytes);
	if (err < 0)
	{
		return err;
		HWSERIAL.println("err Save Write");
	}

	eRAM.f_close(_filePtr);

	return 0; // all is ok
}
// ---------------------------------------------------------------------------
void _Array3D::uploadTableX(void)
{

	int16_t n;
	uint16_t sz;

	sz = _dataCount;

	buffer[0] = sz;
	buffer[1] = CRC16.ccitt(reinterpret_cast<uint8_t *>(_data),
							_dataCount * WORDSIZE); //Calculate CRC add to end of array

	n = hid_sendWithAck(reinterpret_cast<uint8_t *>(buffer));

	if (n)
	{
		HWSERIAL.println("Error1");
		return;
	}

	uint16_t pos = 0;
	//send EEprom to PC
	while (pos < sz)
	{
		int c = MIN(WORD_PACKET_SIZE, sz - pos);
		memset(buffer, 0, WORD_PACKET_SIZE);

		memcpy(buffer, _data + pos, c * sizeof(uint16_t));
		n = hid_sendWithAck(reinterpret_cast<uint8_t *>(buffer));
		if (n)
		{
			HWSERIAL.println("Error2");
			return;
		}
		pos += WORD_PACKET_SIZE;
	};

	HWSERIAL.print("Uploaded ");
	HWSERIAL.println(_fName);
}
// ---------------------------------------------------------------------------
bool _Array3D::downLoadTableX(void)
{
	uint16_t n;

	uint16_t buf[WORD_PACKET_SIZE];

	memset(buf, 0, sizeof(buf));

	n = RawHID.recv(reinterpret_cast<uint8_t *>(buf), 500);
	if (n < 1)
	{
		HWSERIAL.println("Error 1");
		return false;
	}
	hid_sendAck(reinterpret_cast<uint8_t *>(buf));

	uint16_t sz = buf[0];
	uint16_t crc = buf[1];

	uint16_t pos = 0;

	while (pos < sz)
	{
		int c = MIN(WORD_PACKET_SIZE, sz - pos);
		memset(buf, 0, sizeof(buf));
		n = RawHID.recv(reinterpret_cast<uint8_t *>(buf), 500);

		if (n < 1)
		{
			HWSERIAL.print("Error 2->");
			return false;
		}
		//hid_sendAck(reinterpret_cast<uint8_t*>(buf));
		memcpy(ScratchPad + pos, buf, c * sizeof(uint16_t));
		pos += WORD_PACKET_SIZE;
	}

	// calculate crc for data array
	uint16_t crcCheckVal = CRC16.ccitt(reinterpret_cast<uint8_t *>(ScratchPad),
									   sz * sizeof(uint16_t));

	if (crc != crcCheckVal)
	{
		HWSERIAL.print("crc fail");
		return false;
	}

	ScratchPad[sz ] = crc;

	n = SaveToFlash(reinterpret_cast<const char *>(&ScratchPad),
					sz * sizeof(int16_t) + sizeof(int16_t));
	if (n < 0)
	{
		HWSERIAL.print("eprom save fail");
		return false;
	}

	hid_sendAck(reinterpret_cast<uint8_t *>(buf));
	HWSERIAL.print("Downloaded ");
	HWSERIAL.println(_fName);
	return true;
}
