/*
 * Settings.h
 *
 *  Created on: 30 Sep. 2020
 *      Author: apaml
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "Arduino.h"
#include "defines.h"
#include <FastCRC.h>
#include "USB.h"


#define  TX_PACKET_SIZE    32
#define  DATA_PACKET_SIZE    27

	struct SETTINGS_t {

	};

class _Settings {
public:
	_Settings();

	void processCommand(const int16_t subCommand, const int16_t offset,
			const int16_t data) {
		switch (subCommand) {

		case 0XA1:
		//	LoadNewMap(offset, data);
			break;

		}
	}

int16_t Load(void);
int16_t Save(const char * ,const uint16_t);
uint16_t LoadNewMap(rxData);
private:

//Variables for loading new Table
uint16_t _writeCommand;
uint16_t _Crc;
uint16_t _numPackets;
uint16_t _packetCounter;
uint16_t _dataOffset;

 char _fName[9];
spiffs_file _filePtr;
const int settingsSize = sizeof(SETTINGS_t);
void printSettings(void);
};


extern _Settings Settings;
extern SETTINGS_t Settings_t;

#endif /* SETTINGS_H_ */
