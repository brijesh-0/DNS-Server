#include <stdio.h>
#include <stdint.h>
#include "shared.h"

void step(BytePacketBuffer *buffer, size_t step) {
    buffer->position += step;
}

void seek(BytePacketBuffer *buffer, size_t position) {
    buffer->position = position;
}

uint8_t read_u8(BytePacketBuffer *buffer) {
    if (buffer->position >= 512) {
        fprintf(stderr, "Buffer overflow in read_u8\n");
        return 255; // Error value
    }
    uint8_t value = buffer->buffer[buffer->position];
    step(buffer, 1);
    return value;
}

uint16_t read_u16(BytePacketBuffer *buffer) {
    uint8_t byte1 = read_u8(buffer);
    if (byte1 == 255) {
        return -1; // Error value
    }
    uint8_t byte2 = read_u8(buffer);
    if (byte2 == 255) {
        return -1; // Error value
    }
    return ((uint16_t)byte1 << 8) | (uint16_t)byte2;
}