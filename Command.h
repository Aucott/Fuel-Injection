/*
 * Command.h
 *
 *  Created on: 10 Nov. 2018
 *      Author: apaml
 */

#ifndef COMMAND_H_
#define COMMAND_H_

#include <CircularBuffer.h>
#include "Arduino.h"
#include "Ignition.h"


//void getDataFromUSB(void *arg);
void processCommands(void );
void uploadData2PC(void);
void SendRunningData(void);
bool LoadMapsFromRam(void);
void LoadSettingsFromEprom(void);



#endif /* COMMAND_H_ */
