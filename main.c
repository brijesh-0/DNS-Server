#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

// Prints char as hex to avoid non printable charecters
void print_hex(const unsigned char *data, size_t length) {
    for (size_t i = 0; i < length; i++) {
        printf("%02x ", data[i]);
    }
    printf("\nLength: %zd", length);
    printf("\n\n");
}

int main() {
    int socket_file_descriptor;
    unsigned char buffer[1024];
    const char *hello = "Hello from UDP server!";
    struct sockaddr_in server_address, client_address;

    if ((socket_file_descriptor = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    memset(&server_address, 0, sizeof(server_address)); // make the memory address space of server address all 0s
    memset(&client_address, 0, sizeof(client_address)); // make the memory address space of client address all 0s

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY; // all addresses of localhost will be listened to
    server_address.sin_port = htons(8080); // htons = host byte order to network byte order (TODO: write a note on this)

    if(bind(socket_file_descriptor, (const struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("bind failed");
        exit(1);
    }

    socklen_t len = sizeof(client_address);
    int n = recvfrom(socket_file_descriptor, (char *)buffer, 1024, MSG_WAITALL, (struct sockaddr *) &client_address, &len);
	buffer[n] = '\0';
    printf("Client : %c\n", buffer[15]);
    
    printf("Length of dname: %d\n", buffer[12]); // can access individual values!!!
    for(int i = 13; i < 20; i++) {
        printf("%c", buffer[i]);
    } printf("\n");

    print_hex(buffer, n);

    // construct hard-coded response for example.com with IP 12.12.12.15
    unsigned char response[1024];
    int response_len = 0;

    // All values in Hex for now for print_hex
    // 1. Transaction ID: Copy from the request
    response[response_len++] = buffer[0];  // Transaction ID (2 bytes)
    response[response_len++] = buffer[1];

    // 2. Flags: Response, Authoritative, No Error (QR=1, AA=1, NOERROR=0)
    response[response_len++] = 0x80;  // 0x81 = QR (response) + AA (authoritative)
    response[response_len++] = 0x80;  // 0x80 = NOERROR (status)

    // 3. Number of Questions (1)
    response[response_len++] = 0x00;
    response[response_len++] = 0x01;  // 1 question

    // 4. Number of Answers (1)
    response[response_len++] = 0x00;
    response[response_len++] = 0x01;  // 1 answer

    // 5. Number of Authority Records (0)
    response[response_len++] = 0x00;
    response[response_len++] = 0x00;

    // 6. Number of Additional Records (0)
    response[response_len++] = 0x00;
    response[response_len++] = 0x00;

    // 7. Question Section (query for example.com)
    //    Domain Name: "example.com" as a sequence of labels
    response[response_len++] = 0x07;  // Length of "example" = 7
    response[response_len++] = 'e';
    response[response_len++] = 'x';
    response[response_len++] = 'a';
    response[response_len++] = 'm';
    response[response_len++] = 'p';
    response[response_len++] = 'l';
    response[response_len++] = 'e';

    response[response_len++] = 0x03;  // Length of "com" = 3
    response[response_len++] = 'c';
    response[response_len++] = 'o';
    response[response_len++] = 'm';

    response[response_len++] = 0x00;  // End of domain name

    // - Type (A record)
    response[response_len++] = 0x00;
    response[response_len++] = 0x01;  // Type A, IPv4 address

    // - Class (IN, Internet)
    response[response_len++] = 0x00;
    response[response_len++] = 0x01;  // Class 'IN'

    // 8. Answer Section with IP address 12.12.12.15
    // - Name should be the same as the one recieved else it's be rejected by client.
    response[response_len++] = 0xC0;
    response[response_len++] = 0x0C;  // Pointer to name in question section

    // - Type (A record)
    response[response_len++] = 0x00;
    response[response_len++] = 0x01;  // Type A (IPv4 address)

    // - Class (IN, Internet)
    response[response_len++] = 0x00;
    response[response_len++] = 0x01;  // Class IN

    // - TTL (Time to Live)
    response[response_len++] = 0x00;
    response[response_len++] = 0x00;  // TTL (0)
    response[response_len++] = 0x00;
    response[response_len++] = 0x0F;  // TTL (15 seconds)

    // - Data Length 4 bytes for IPv4 address
    response[response_len++] = 0x00;  // gap
    response[response_len++] = 0x04;  // 4 bytes for IPv4 address

    // - IP Address (12.12.12.15)
    response[response_len++] = 0x0C;
    response[response_len++] = 0x0C;
    response[response_len++] = 0x0C;
    response[response_len++] = 0x0F;  // 12.12.12.15

    printf("\nResponse: \n");
    print_hex(response, 53); // print the response to compare to query

	sendto(socket_file_descriptor, (const char *)response, response_len, MSG_CONFIRM, (const struct sockaddr *)&client_address, len);
	printf("\nMessage sent!\n");

	return 0;
}