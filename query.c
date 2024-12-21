// handle the query given from application and make the struct so other files can access it

#include <stdint.h> // uint8_t
#include <stddef.h> // size_t

typedef struct BytePacketBuffer {
	uint8_t buffer[512];
	size_t position;
}BytePacketBuffer;

// Write Utility function to traverse BytePacket

void step(BytePacketBuffer *bufferPacket, size_t step) {
	bufferPacket->position += step;
}

void seek(BytePacketBuffer *bufferPacket, size_t seekPosition) {
	bufferPacket->position = seekPosition;
}

uint8_t read(BytePacketBuffer *bufferPacket) {
	if (bufferPacket->position >= 512) {
		return 255;
	}
	uint8_t value = bufferPacket->buffer[bufferPacket->position];
	step(bufferPacket, 1);
	return value;
}

uint8_t get_range(BytePacketBuffer *buffer, size_t start, size_t len) {
	if (start + len >= 512) {
		return -1;
	}
	return &buffer->buffer[start]; // return the address of the start of the slice
}

int16_t read_u16(BytePacketBuffer *buffer) {
	uint8_t byte1 = read(buffer);
	if (byte1 == 255) {
		return -1;
	}
	uint8_t byte2 = read(buffer);
	if (byte2 == 255) {
		return -1;
	}

	uint16_t result = ((uint16_t)byte1 << 8) | (uint16_t)byte2;// Combine byte1 (high byte) and byte2 (low byte) into a 16-bit value
	return result;
}

BytePacketBuffer handleQuery(BytePacketBuffer buffer) {
 // TODO
}