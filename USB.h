/*
 * USB.h
 *
 *  Created on: 10 Oct. 2019
 *      Author: apaml
 */

#ifndef USB_H_
#define USB_H_
#define CIRCULAR_BUFFER_INT_SAFE
#include "Arduino.h"
#include <CircularBuffer.h>
#include "defines.h"

#define  WORD_PACKET_SIZE    		32
#define  BYTE_PACKET_SIZE    		64

typedef struct {
	uint16_t command;
	uint16_t subCommand;
	uint16_t dataX;
	uint16_t dataY;
	uint16_t dataArray[28];
} rxData;

typedef struct {

	uint16_t Command;
	uint16_t X;
	uint16_t Y;
	uint16_t data;
} txData;

void CheckUSB(void);
void sendPacketUSB(uint16_t, uint16_t, uint16_t, uint16_t);
int16_t hid_sendAck(uint8_t *buf);
int16_t hid_checkAck(uint8_t *buf);
int16_t hid_sendWithAck(uint8_t *buf);


extern rxData RXdata;
extern uint16_t usb_ACK;
extern CircularBuffer<rxData, 0x03> CommandBuffer;
extern bool BufferOverflow;

#endif /* USB_H_ */
