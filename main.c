#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main() {
    int socket_file_descriptor;
    char buffer[1024];
    const char *hello = "Hello from UDP server!";
    struct sockaddr_in server_address, client_address;

    if ((socket_file_descriptor = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    memset(&server_address, 0, sizeof(server_address));
    memset(&client_address, 0, sizeof(client_address));

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8080);

    if(bind(socket_file_descriptor, (const struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("bind failed");
        exit(1);
    }

    socklen_t len = sizeof(client_address);
    int n = recvfrom(socket_file_descriptor, (char *)buffer, 1024, MSG_WAITALL, (struct sockaddr *) &client_address, &len);
	buffer[n] = '\0';
    printf("Client : %s\n", buffer);
	sendto(socket_file_descriptor, (const char *)hello, strlen(hello), MSG_CONFIRM, (const struct sockaddr *) &client_address, len);
	printf("\nMessage sent!\n");

	return 0;
}