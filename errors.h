/*
 * errors.h
 *
 *  Created on: 20 Oct. 2019
 *      Author: apaml
 */

#ifndef ERRORS_H_
#define ERRORS_H_

#include "Arduino.h"
#include "defines.h"
//#include "Fram_12c.h"

//#define ERR_NONE        			0 //No error
#define ERR_UNKNOWN     			0 //Unknown error
#define ERR_IAT_SHORT   			1 //Inlet sensor shorted
#define ERR_IAT_GND     			2 //Inlet sensor grounded
#define ERR_CLT_SHORT   			3 //Coolant sensor shorted
#define ERR_CLT_GND     			4 //Coolant Sensor grounded
#define ERR_O2_SHORT    			5 //O2 sensor shorted
#define ERR_O2_GND      			6 //O2 sensor grounded
#define ERR_TPS_SHORT   			7 //TPS shorted (Is potentially valid)
#define ERR_TPS_GND     			8 //TPS grounded (Is potentially valid)
#define ERR_BAT_HIGH    			9 //Battery voltage is too high
#define ERR_BAT_LOW     			10 //Battery voltage is too low
#define ERR_MAP_HIGH    			11 //Map output is too high
#define ERR_MAP_LOW     			12 //Map output is too low
#define ERR_SYNC_MISSING    		13 //Sync (Cam) input signal missing
#define ERR_CRANK_MISSING   		14 //Cylinder (Crank) input signal missing
#define ERR_OVER_VOLTAGE    		15 //ECU supply over voltage
#define ERR_FRAM_ERROR				16 //ECU supply over voltage
#define ERR_OVER_BOOST				17 //ECU supply over voltage
#define ERR_MEM_LOAD_CRC			18 //ECU supply over voltage
#define ERR_SETTINGS_LOAD_CRC		19 //ECU supply over voltage

class _errors {
public:
	_errors(void);

	void processCommand(const uint16_t subCommand, const uint16_t offset,
			const uint16_t data) {
		switch (subCommand) {

		case CLEAR_ERROR_CODES:
			clearErrors();
			break;
		case CLEAR_ERROR_CODE:
			clearError(data);
			break;
		}
	}

	void setError(uint8_t);
	void clearErrors(void);
	void clearError(uint8_t);
	uint32_t GetErrors(void);
	uint16_t GetErrorsHighByte(void);
	uint16_t GetErrorsLowByte(void);
	void loadSettings(void);
	void setErrorFlag(void);
	void clearErrorFlag(void);
	bool getErrorFlag(void);

private:
	uint32_t _errorStack = 0x00000000;
	uint32_t _errorStackMemory = 0x00000000;
	bool _errorFlag = false; // temporary error flag for checking for a error on a group of functions

};

extern _errors Errors;

#endif /* ERRORS_H_ */
