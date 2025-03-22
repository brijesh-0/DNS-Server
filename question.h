#ifndef QUESTION_H
#define QUESTION_H

#include "shared.h"

void parse_dns_question(const uint8_t *packet, int *index, DNSQuestion *question);

#endif