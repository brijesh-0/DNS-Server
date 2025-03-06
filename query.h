#include <stdint.h> // uint8_t
#include <stddef.h> // size_t

typedef struct BytePacketBuffer {
	uint8_t buffer[512];
	size_t position;
}BytePacketBuffer;

void step(BytePacketBuffer *bufferPacket, size_t step);
void seek(BytePacketBuffer *bufferPacket, size_t seekPosition);