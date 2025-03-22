#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "question.h"

void parse_dns_question(const uint8_t *packet, int *index, DNSQuestion *question) {
    int i = 0;
    int len;
    while (packet[*index] != 0) {
        len = packet[*index];
        (*index)++;
        for (int j = 0; j < len && i < 255; j++) {
            question->qname[i++] = packet[(*index)++];
        }
        if (i < 255) question->qname[i++] = '.';
    }
    if (i > 0) question->qname[i - 1] = '\0';
    else question->qname[0] = '\0';
    (*index)++; // Skip null byte

    question->qtype = (packet[*index] << 8) | packet[*index + 1];
    *index += 2;
    question->qclass = (packet[*index] << 8) | packet[*index + 1];
    *index += 2;

    printf("Parsed QNAME: %s\n", question->qname);
    printf("QTYPE: %u\n", question->qtype);
    printf("QCLASS: %u\n", question->qclass);
}