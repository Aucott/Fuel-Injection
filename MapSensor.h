/*
 * MapSensor.h
 *
 *  Created on: 31 May 2019
 *      Author: apaml
 */

#ifndef MAPSENSOR_H_
#define MAPSENSOR_H_

#define VALID_MAP_MAX 4090 //The largest ADC value that is valid for the MAP sensor
#define VALID_MAP_MIN 5	   //The smallest ADC value that is valid for the MAP sensor
#define ADCFILTER_MAP 20   //This is only used on MAP readings and is intentionally very weak to allow for faster response

/*
 * The highest sea-level pressure on Earth occurs in Siberia, where the Siberian High often attains a sea-level pressure above 105 kPa;
 * with record highs close to 108.5 kPa.
 * The lowest measurable sea-level pressure is found at the centers of tropical cyclones and tornadoes, with a record low of 87 kPa;
 */
#define BARO_MIN 870
#define BARO_MAX 1080

#include "Arduino.h"
#include "RunningData.h"
#include "errors.h"
#include "Array3D.h"
#include "Settings.h"

//----------------------------------------------------------------------------------------
class _MapSensor
{
public:
	_MapSensor();
	_MapSensor(_Array3D *, _Array3D *);

	void processCommand(const rxData Rxdata)
	{
		uint16_t subCommand = Rxdata.subCommand;
		uint16_t dataCount = Rxdata.dataX;
		uint16_t Crc = Rxdata.dataY;
		uint16_t crc;

		switch (subCommand)
		{
		case 0XA1:
		{
			uint16_t buf[WORD_PACKET_SIZE];
			memset(buf, 0, sizeof(buf));
			HWSERIAL.print("Uploading Map Settings ->");
			buf[1] = getMapPredictionEnable();
			buf[2] = getMapPredictionTransitionTime();
			buf[3] = getAdaptiveMapPredictionEnable();

			buf[0] = CRC16.ccitt(
				reinterpret_cast<uint8_t *>(&(buf[1])),
				(WORD_PACKET_SIZE * sizeof(uint16_t)) - sizeof(uint16_t));

			int n = hid_sendWithAck(reinterpret_cast<uint8_t *>(buf));

			if (n)
			{
				HWSERIAL.println("Error Uploading Map Settings");
			}
			HWSERIAL.println("OK");
			break;
		}

		case 0XA2:

			HWSERIAL.println("Downloading Map Settings");
			crc =
				CRC16.ccitt(
					reinterpret_cast<uint8_t *>((uint16_t *)(&(Rxdata.dataArray[0]))),
					dataCount * sizeof(uint16_t));
			if (Crc == crc)
			{
				setMapPredictionEnable(Rxdata.dataArray[0]);
				setMapPredictionTransitionTime(Rxdata.dataArray[1]);
				setAdaptiveMapPredictionEnable(Rxdata.dataArray[2]); // Not implemented yet
				MapSettings.Crc = Crc;
				Save();
			}
			else
			{
				HWSERIAL.print("Map settings load error ");
				Errors.setError(ERR_SETTINGS_LOAD_CRC);
			}
			break;
		}
	}
	//----------------------------------------------------------------------------------------
	void SetReturnVal(uint16_t returnVal)
	{
		_returnVal = returnVal;
		return;
	}

	//----------------------------------------------------------------------------------------

	void addValue(uint16_t);
	bool isBoosting(void);
	uint16_t getMap(void);
	uint16_t getRawADC(void);
	bool isMapPredictionRunning(void);
	uint16_t getPredictedMapValue(void);
	void setCalcFlag(void);
	bool calcMap(const uint16_t, const uint16_t, const int16_t);
	void calcInstantaneousMAP(void);
	uint16_t getUnfilteredMap(void);
	void useMapPrediction(void);
	void loadSettings(void);
	void setMapPredictionEnable(uint16_t);
	uint16_t getMapPredictionEnable(void);
	void setAdaptiveMapPredictionEnable(uint16_t);
	uint16_t getAdaptiveMapPredictionEnable(void);
	void setMapPredictionTransitionTime(uint16_t);
	uint16_t getMapPredictionTransitionTime(void);

	uint16_t getAtmosphericPressure(void);
	uint16_t getAverageADC(void);
	void setAtmosphericPressure(void);
	int16_t Load(void);

private:
	void zero(void);
	void setAtmosphericPressure(uint16_t);
	const int _boostHystheresis = 20;
	int16_t Save(void);
	bool crcCheckSettings(void);
	char _fName[5] = "sMap";
	spiffs_file _filePtr;

	uint16_t _returnVal;

	struct t_SETTINGS
	{
		uint16_t Map_PredictionEnabled;
		uint16_t Map_PredictionTransitionTime;
		uint16_t Map_AdaptiveMapPredictionEnabled;
		uint16_t Crc;
	};
	t_SETTINGS MapSettings;

	_Array3D *_MapPtr;
	_Array3D *_PredictedMapPtr;

	// Calculated values
	float _predictedMapValue;
	uint16_t _MAP; // kPa
	uint16_t _MAPcount;
	uint32_t _MAPrunningValue;
	uint16_t _rawMapADC;
	uint16_t _averageMapADC;
	elapsedMillis _mapPredictionTimer;
	uint16_t _atmosphericPressure; //x10

	//Flags
	bool _mapPredictionFlag = false;
	bool _calcNow = false;
	bool _is_Boosting = false;
};
//----------------------------------------------------------------------------------------
extern _MapSensor MapSensor;

#endif /* MAPSENSOR_H_ */
