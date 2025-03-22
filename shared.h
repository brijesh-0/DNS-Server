#ifndef SHARED_H
#define SHARED_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint8_t buffer[512];
    size_t position;
} BytePacketBuffer;

typedef struct {
    uint16_t id;      // Transaction ID
    uint16_t flags;   // Flags
    uint16_t qdcount; // Number of questions
    uint16_t ancount; // Number of answer RRs
    uint16_t nscount; // Number of authority RRs
    uint16_t arcount; // Number of additional RRs
} DNSHeader;

typedef struct {
    char qname[256];
    uint16_t qtype;
    uint16_t qclass;
} DNSQuestion;

void step(BytePacketBuffer *buffer, size_t step);
void seek(BytePacketBuffer *buffer, size_t position);
uint8_t read_u8(BytePacketBuffer *buffer);
uint16_t read_u16(BytePacketBuffer *buffer);

#endif