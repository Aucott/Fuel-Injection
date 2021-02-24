/*
 * Settings.cpp
 *
 *  Created on: 30 Sep. 2020
 *      Author: apaml
 */

#include "Settings.h"

SETTINGS_t Settings_t;

_Settings::_Settings() {
	_writeCommand = 0x100;
	_Crc = 0x100;
}
// ---------------------------------------------------------------------------
int16_t _Settings::Load(void) {

	int16_t err;
	err = eRAM.f_open(_filePtr, _fName, SPIFFS_RDONLY);
	if (err < 0) {
		return err;
	}
	err = eRAM.f_read(_filePtr, reinterpret_cast<const char *>(&Settings_t),
			settingsSize);
	if (err < 0) {
		return err;
	}
	eRAM.f_close(_filePtr);

	return 1; // all is ok
}
// ---------------------------------------------------------------------------
int16_t _Settings::Save(const char * data, const uint16_t numBytes) {

	int16_t err;
	err = eRAM.f_open(_filePtr, _fName, SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR);
	if (err < 0) {
		return err;
	}
	err = eRAM.f_write(_filePtr, data, numBytes);
	if (err < 0) {
		return err;
	}
	eRAM.f_close(_filePtr);

	return 1; // all is ok
}
// ---------------------------------------------------------------------------
uint16_t _Settings::LoadNewMap(rxData Rxdata) {
	uint16_t crc;

	if (_writeCommand == Rxdata.subCommand) {
		_Crc = Rxdata.dataX;  // Check sum in Rxdata
		_numPackets = Rxdata.dataY; // Number of bytes to load in array

		_packetCounter = 0;
		_dataOffset = 0;

	}

	if ((_writeCommand + _packetCounter) == Rxdata.subCommand) {

		//memcpy(ScratchPad + (_packetCounter * 54), Rxdata.dataArray, 54);

		for (int i = 0; i < DATA_PACKET_SIZE; i++) {

			if (_dataOffset < SCRATCHPADSIZE) { /// check if array is in bounds

				ScratchPad[_dataOffset++] = Rxdata.dataArray[i]; // Write to external Ram

			}

		}

		if (_numPackets == _packetCounter) { // The required number of ints have read in

			crc = CRC16.ccitt(reinterpret_cast<uint8_t*>(&ScratchPad),
					settingsSize);

			if (crc == _Crc) { // CRC check is OK copy from Ram to Low Fram

				digitalWrite(ledPin, !digitalRead(ledPin));
				Save(reinterpret_cast<const char *>(&ScratchPad), settingsSize);
				Load();
				printSettings();
			}
		}

		_packetCounter++;
	} else {
		//Errors.setError(ERR_MEM_LOAD_CRC);
	}

	return 0;
}

void _Settings::printSettings(void) {
	uint16_t TXpacket[65];

	uint16_t *ptr = reinterpret_cast<uint16_t *>(&Settings_t);
	TXpacket[0] = ptr[0];
	TXpacket[1] = ptr[1];
	TXpacket[2] = ptr[2];
	TXpacket[3] = ptr[3];
	TXpacket[4] = ptr[4];
	TXpacket[5] = ptr[5];
	TXpacket[6] = ptr[6];
	TXpacket[7] = ptr[7];
	TXpacket[8] = ptr[8];
	TXpacket[9] = ptr[9];
	TXpacket[10] = ptr[10];
	TXpacket[11] = ptr[11];
	TXpacket[12] = ptr[12];
	TXpacket[13] = ptr[13];
	TXpacket[14] = ptr[14];
	TXpacket[15] = ptr[15];
	TXpacket[16] = ptr[16];
	TXpacket[17] = ptr[17];
	TXpacket[18] = ptr[18];
	TXpacket[19] = ptr[19];
	TXpacket[20] = ptr[20];
	TXpacket[21] = ptr[21];
	TXpacket[22] = ptr[22];
	TXpacket[23] = ptr[23];
	TXpacket[24] = ptr[24];
	TXpacket[25] = ptr[25];
	TXpacket[26] = ptr[26];
	TXpacket[27] = ptr[27];
	TXpacket[28] = ptr[28];
	TXpacket[29] = ptr[29];
	TXpacket[30] = ptr[30];
	TXpacket[31] = ptr[31];
	RawHID.send(reinterpret_cast<uint8_t*>(TXpacket), 10);
	delay(200);

	TXpacket[0] = ptr[32];
	TXpacket[1] = ptr[33];
	TXpacket[2] = ptr[34];
	TXpacket[3] = ptr[35];
	TXpacket[4] = ptr[36];
	TXpacket[5] = ptr[37];
	TXpacket[6] = ptr[38];
	TXpacket[7] = ptr[39];
	TXpacket[8] = ptr[40];
	TXpacket[9] = ptr[41];
	TXpacket[10] = ptr[42];
	TXpacket[11] = ptr[43];
	TXpacket[12] = ptr[44];
	TXpacket[13] = ptr[45];
	TXpacket[14] = ptr[46];
	TXpacket[15] = ptr[47];
	TXpacket[16] = ptr[48];
	TXpacket[17] = ptr[49];
	TXpacket[18] = ptr[50];
	TXpacket[19] = ptr[51];
	TXpacket[20] = ptr[52];
	TXpacket[21] = ptr[53];
	TXpacket[22] = ptr[54];
	TXpacket[23] = ptr[55];
	TXpacket[24] = ptr[56];
	TXpacket[25] = ptr[57];
	TXpacket[26] = ptr[58];
	TXpacket[27] = ptr[59];
	TXpacket[28] = ptr[60];
	TXpacket[29] = ptr[61];
	TXpacket[30] = ptr[62];
	TXpacket[31] = ptr[63];
	RawHID.send(reinterpret_cast<uint8_t*>(TXpacket), 10);
	delay(200);

	TXpacket[0] = ptr[64];
	TXpacket[1] = ptr[65];
	TXpacket[2] = ptr[0];
	TXpacket[3] = ptr[1];
	TXpacket[4] = ptr[2];
	TXpacket[5] = 0;
	TXpacket[6] = 0;
	TXpacket[7] = 0;
	TXpacket[8] = 0;
	TXpacket[9] = 0;
	TXpacket[10] = 0;
	TXpacket[11] = 0;
	TXpacket[12] = 0;
	TXpacket[13] = 0;
	TXpacket[14] = 0;
	TXpacket[15] = 0;
	TXpacket[16] = 0;
	TXpacket[17] = 0;
	TXpacket[18] = 0;
	TXpacket[19] = 0;
	TXpacket[20] = 0;
	TXpacket[21] = 0;
	TXpacket[22] = 0;
	TXpacket[23] = 0;
	TXpacket[24] = 0;
	TXpacket[25] = 0;
	TXpacket[26] = 0;
	TXpacket[27] = 0;
	TXpacket[28] = 0;
	TXpacket[29] = 0;
	TXpacket[30] = 0;
	TXpacket[31] = 0;
	RawHID.send(reinterpret_cast<uint8_t*>(TXpacket), 10);

}
