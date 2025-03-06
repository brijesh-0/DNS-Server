// Start the server and hand data to server

#include <stdio.h>
#include <string.h> // memset_s
#include <sys/socket.h> // sockaddr_in, bind, recvfrom 
#include <arpa/inet.h> // htons | also includes 'netinet/in.h'

int main() {
	int socket_file_descriptor;
	uint8_t buffer[513];
	struct sockaddr_in server_address, client_address;

	if((socket_file_descriptor = socket(AF_INET, SOCK_DGRAM, 0) != 0)) {
		perror("Socket Creation failed");
		exit(1);
	}

	memset_s(&server_address, sizeof(server_address), 0, sizeof(server_address));
	memset_s(&client_address, sizeof(client_address), 0, sizeof(client_address));

	server_address.sin_family = AF_INET; //IPv4
	server_address.sin_addr.s_addr = INADDR_ANY; // Any address can access the server
	server_address.sin_port = htons(8080); // htons = host byte order to network byte order (TODO: write a note on this)

	if(bind(socket_file_descriptor, (const struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
		perror("bind failed");
        exit(1);
	}

	int nbytes = recvfrom(socket_file_descriptor, buffer, sizeof(buffer), MSG_WAITALL, (struct sockaddr *) &client_address, sizeof(client_address));
	buffer[nbytes] = '\0';

	// Handle query.c
	handle_query(buffer); // will need a struct to return two values position and the response packet

	// respond

	return 0;
}