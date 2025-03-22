#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include "response.h"
#include "database.h"
#include "shared.h"

void write_u16(BytePacketBuffer *buffer, uint16_t value) {
    if (buffer->position + 2 > 512) return;
    buffer->buffer[buffer->position++] = (value >> 8) & 0xFF;
    buffer->buffer[buffer->position++] = value & 0xFF;
}

void write_u8(BytePacketBuffer *buffer, uint8_t value) {
    if (buffer->position < 512) {
        buffer->buffer[buffer->position++] = value;
    }
}

int construct_response(const DNSHeader *header, const DNSQuestion *question, BytePacketBuffer *response) {
    response->position = 0;

    // 1. Transaction ID
    write_u16(response, header->id);

    // 2. Flags: Response, Authoritative
    uint16_t flags = 0x8180; // QR=1, AA=1, NOERROR=0 (default)
    char ip_address[16] = {0};
    int query_result = lookup_dns_record(question->qname, ip_address, sizeof(ip_address));

    if (query_result != 0 || strlen(ip_address) == 0) {
        flags = 0x8183; // NXDOMAIN (RCODE=3)
    }
    write_u16(response, flags);

    // 3. Counts
    write_u16(response, 1); // QDCOUNT
    write_u16(response, query_result == 0 ? 1 : 0); // ANCOUNT
    write_u16(response, 0); // NSCOUNT
    write_u16(response, 0); // ARCOUNT

    // 4. Question Section
    size_t qname_len = strlen(question->qname);
    char qname_copy[256];
    strncpy(qname_copy, question->qname, sizeof(qname_copy));
    qname_copy[sizeof(qname_copy) - 1] = '\0';
    char *label = strtok(qname_copy, ".");
    while (label != NULL) {
        write_u8(response, strlen(label));
        for (size_t i = 0; i < strlen(label); i++) {
            write_u8(response, label[i]);
        }
        label = strtok(NULL, ".");
    }
    write_u8(response, 0); // End of QNAME
    write_u16(response, question->qtype);
    write_u16(response, question->qclass);

    // 5. Answer Section (only if record found)
    if (query_result == 0 && strlen(ip_address) > 0) {
        write_u16(response, 0xC00C); // Pointer to QNAME
        write_u16(response, question->qtype); // Type A
        write_u16(response, question->qclass); // Class IN
        write_u16(response, 0); // TTL (high)
        write_u16(response, 15); // TTL (low, 15 seconds)
        write_u16(response, 4); // Data length (IPv4)

        // Parse IP address (e.g., "12.12.12.15")
        uint8_t ip[4];
        sscanf(ip_address, "%hhu.%hhu.%hhu.%hhu", &ip[0], &ip[1], &ip[2], &ip[3]);
        write_u8(response, ip[0]);
        write_u8(response, ip[1]);
        write_u8(response, ip[2]);
        write_u8(response, ip[3]);
    }

    return 0;
}