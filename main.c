#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "shared.h"
#include "query.h"
#include "response.h"
#include "database.h"

void print_hex(const uint8_t *data, size_t length) {
    for (size_t i = 0; i < length; i++) {
        printf("%02x ", data[i]);
    }
    printf("\nLength: %zd\n\n", length);
}

int main() {
    // Initialize database
    if (init_dns_database() != 0) {
        fprintf(stderr, "Failed to initialize database\n");
        return EXIT_FAILURE;
    }

    int socket_fd;
    BytePacketBuffer buffer = { .position = 0 };
    struct sockaddr_in server_addr, client_addr;

    // Create UDP socket
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize server address
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(5353);

    // Bind socket
    if (bind(socket_fd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("DNS server listening on port 53...\n");

    while (1) {
        socklen_t len = sizeof(client_addr);
        // Receive DNS query
        int n = recvfrom(socket_fd, buffer.buffer, sizeof(buffer.buffer), MSG_WAITALL,
                         (struct sockaddr *)&client_addr, &len);
        if (n < 0) {
            perror("Receive failed");
            continue;
        }
        buffer.position = 0;

        printf("Received query:\n");
        print_hex(buffer.buffer, n);

        // Process query and construct response
        BytePacketBuffer response = { .position = 0 };
        if (handle_query(&buffer, &response) == 0) {
            // Send response
            sendto(socket_fd, response.buffer, response.position, MSG_CONFIRM,
                   (const struct sockaddr *)&client_addr, len);
            printf("Response sent:\n");
            print_hex(response.buffer, response.position);
        } else {
            printf("Failed to process query\n");
        }
    }

    close(socket_fd);
    return 0;
}