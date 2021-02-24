/*
 * USB.cpp
 *
 *  Created on: 10 Oct. 2019
 *      Author: apaml
 */
#include "USB.h"

rxData RXdata;

CircularBuffer<rxData, 0x03> CommandBuffer;
bool BufferOverflow = false;
// ---------------------------------------------------------------------------
void CheckUSB(void)
{return;
	uint8_t RXbuffer[64];

	if (RawHID.recv(RXbuffer, 0) > 0)
	{ // 0 timeout = do not wait

		RawHID.send(RXbuffer, 0);

		memcpy(&RXdata, RXbuffer, sizeof(RXdata));

		if (CommandBuffer.isFull())
			BufferOverflow = true;
		CommandBuffer.unshift(RXdata); // store Data in circular buffer
	}
}
// ---------------------------------------------------------------------------
void sendPacketUSB(uint16_t Command, uint16_t SubCommand, uint16_t Offset,
				   uint16_t Data)
{
	uint16_t TXpacket1[32];

	TXpacket1[0] = Command;
	TXpacket1[1] = SubCommand;
	TXpacket1[2] = Offset;
	TXpacket1[3] = Data;

	RawHID.send(TXpacket1, 10);
}

// ---------------------------------------------------------------------------

int16_t hid_sendAck(uint8_t *buf)
{
	return RawHID.send(buf, 100);
}
// ---------------------------------------------------------------------------
int16_t hid_checkAck(uint8_t *buf)
{
	char buf2[BYTE_PACKET_SIZE];
	int n;
	n = RawHID.recv(buf, 100);
	if (n < 0)
		return n;
	return memcmp(buf, buf2, BYTE_PACKET_SIZE);
}
// ---------------------------------------------------------------------------
int16_t hid_sendWithAck(uint8_t *buf)
{
	char buf2[BYTE_PACKET_SIZE];
	int n;
	n = RawHID.send(buf, 100);
	if (n < 1)
		return -1;
	n = RawHID.recv(buf2, 100);
	if (n < 1)
		return -1;
	n = memcmp(buf, buf2, BYTE_PACKET_SIZE);
	if (n)
		return -1;
	return 0;
}
// ---------------------------------------------------------------------------
