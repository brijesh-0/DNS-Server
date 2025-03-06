#include <stdio.h>
#include <stdint.h>
#include <string.h>

// DNS header structure (simplified)
typedef struct {
    uint16_t id;           // Transaction ID
    uint16_t flags;        // Flags
    uint16_t qdcount;      // Number of questions
    uint16_t ancount;      // Number of answer RRs
    uint16_t nscount;      // Number of authority RRs
    uint16_t arcount;      // Number of additional RRs
} DNSHeader;

// DNS Question structure
typedef struct {
    char *qname;
    uint16_t qtype;
    uint16_t qclass;
} DNSQuestion;

// Function to parse a domain name from a DNS packet
// Returns the number of bytes read
int parse_qname(const uint8_t *packet, int index, char *qname) {
    int i = 0;
    int len;
    while (packet[index] != 0) {
        len = packet[index];  // Length of the next label
        index++;
        for (int j = 0; j < len; j++) {
            qname[i++] = packet[index++];
        }
        qname[i++] = '.';  // Add a dot between labels
    }
    qname[i - 1] = '\0';  // Replace the last dot with a null terminator
    return index + 1;      // Skip the null byte
}

// Function to parse the DNS Question section
void parse_dns_question(const uint8_t *packet, int *index) {
    DNSQuestion question;
    
    // Parse the QName (domain name)
    question.qname = (char *)malloc(256 * sizeof(char)); // Allocate memory for the domain name
    if (question.qname == NULL) {
        printf("Memory allocation failed\n");
        return;
    }
    *index = parse_qname(packet, *index, question.qname);
    
    // Parse QType (2 bytes)
    question.qtype = (packet[*index] << 8) | packet[*index + 1];
    *index += 2;
    
    // Parse QClass (2 bytes)
    question.qclass = (packet[*index] << 8) | packet[*index + 1];
    *index += 2;

    // Print the parsed Question section
    printf("QNAME: %s\n", question.qname);
    printf("QTYPE: %u\n", question.qtype);
    printf("QCLASS: %u\n", question.qclass);

    // Free allocated memory for QName
    free(question.qname);
}

int main() {
    // Example DNS packet (from a raw packet capture)
    uint8_t dns_packet[] = {
        0x12, 0x34, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, // Header (simplified)
        0x00, 0x00, 0x03, 0x77, 0x77, 0x77, 0x06, 0x63,  // QName ("www.example")
        0x6f, 0x6d, 0x00, 0x00, 0x01, 0x00, 0x01        // QType=1, QClass=1 (A record)
    };
    
    int index = sizeof(DNSHeader);  // Skip the DNS header
    parse_dns_question(dns_packet, &index);

    return 0;
}
