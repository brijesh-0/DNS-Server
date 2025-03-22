#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "query.h"
#include "question.h"
#include "response.h"

int handle_query(BytePacketBuffer *query, BytePacketBuffer *response) {
    // Parse DNS header
    DNSHeader header;
    header.id = read_u16(query);
    header.flags = read_u16(query);
    header.qdcount = read_u16(query);
    header.ancount = read_u16(query);
    header.nscount = read_u16(query);
    header.arcount = read_u16(query);

    if (header.qdcount != 1) {
        printf("Only one question supported\n");
        return -1;
    }

    // Parse question section
    DNSQuestion question;
    int index = query->position;
    parse_dns_question(query->buffer, &index, &question);
    query->position = index;

    // Construct response
    return construct_response(&header, &question, response);
}