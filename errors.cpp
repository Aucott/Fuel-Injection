/*
 * errors.cpp
 *
 *  Created on: 20 Oct. 2019
 *      Author: apaml
 */

#include "errors.h"

_errors::_errors(void) {

}
//------------------------------------------------------------------------------
void _errors::setError(uint8_t errorID) {
	BIT_SET(_errorStack, errorID);
	//framLow.writeLong(EEPROM_ERROR_CODES, _errorStack);

}
//------------------------------------------------------------------------------
void _errors::clearErrors(void) {
	_errorStack = 0;
	//framLow.writeLong(EEPROM_ERROR_CODES, _errorStack);
}
//------------------------------------------------------------------------------
void _errors::clearError(uint8_t errorID) {
	BIT_CLEAR(_errorStack, errorID);
}
//------------------------------------------------------------------------------
uint32_t _errors::GetErrors(void) {
	return _errorStack;
}
//------------------------------------------------------------------------------
uint16_t _errors::GetErrorsHighByte(void) {
	return highWord(_errorStack);
}
//------------------------------------------------------------------------------
uint16_t _errors::GetErrorsLowByte(void) {

	return lowWord(_errorStack);
}
//------------------------------------------------------------------------------
void _errors::loadSettings(void) {
	//framLow.readLong(EEPROM_ERROR_CODES, &_errorStack);
}
//------------------------------------------------------------------------------
void _errors::setErrorFlag(void) {
	_errorFlag = true;
}
//------------------------------------------------------------------------------
void _errors::clearErrorFlag(void) {
	_errorFlag = false;
}
//------------------------------------------------------------------------------
bool _errors::getErrorFlag(void) {
	return _errorFlag;
}

