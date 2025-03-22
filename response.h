#ifndef RESPONSE_H
#define RESPONSE_H

#include "shared.h"

int construct_response(const DNSHeader *header, const DNSQuestion *question, BytePacketBuffer *response);

#endif