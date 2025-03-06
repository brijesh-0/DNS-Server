// handle the query given from application and make the struct so other files can access it

#include <stdint.h> // uint8_t
#include <stddef.h> // size_t
#include <stdlib.h>

/* typedef struct BytePacketBuffer {
	uint8_t buffer[512];
	size_t position;
}BytePacketBuffer;

// Write Utility function to traverse BytePacket

void step(BytePacketBuffer *bufferPacket, size_t step) {
	bufferPacket->position += step;
}

void seek(BytePacketBuffer *bufferPacket, size_t seekPosition) {
	bufferPacket->position = seekPosition;
}

uint8_t read(BytePacketBuffer *bufferPacket) {
	if (bufferPacket->position >= 512) {
		return 255;
	}
	uint8_t value = bufferPacket->buffer[bufferPacket->position];
	step(bufferPacket, 1);
	return value;
}

uint8_t get_range(BytePacketBuffer *buffer, size_t start, size_t len) {
	if (start + len >= 512) {
		return -1;
	}
	return &buffer->buffer[start]; // return the address of the start of the slice
}

int16_t read_u16(BytePacketBuffer *buffer) {
	uint8_t byte1 = read(buffer);
	if (byte1 == 255) {
		return -1;
	}
	uint8_t byte2 = read(buffer);
	if (byte2 == 255) {
		return -1;
	}
	uint16_t result = ((uint16_t)byte1 << 8) | (uint16_t)byte2;// Combine byte1 (high byte) and byte2 (low byte) into a 16-bit value
	return result;
}
*/

uint8_t handle_query(uint8_t* packet) {
	uint8_t position = 5; // start after the ID and flags
	uint8_t number_of_questions = packet[position];
	position += 6;
	
	char domain_name[512];
	int offset = 0;
	int lable = packet[position];
	
	while (lable != 0 && packet[position] != 0x00 && offset < 512 -1) {
		position += 1;
		for(int i = 0; i < lable; i++) {
			domain_name[offset + i] = packet[position + i];
		}
		offset += lable;
		domain_name[offset++] = '.';
		position += lable;
		lable = packet[position];
	}
	if (offset > 0) {
		domain_name[offset-1] = '\0'; //remove last dot and add null terminator
	 } else {
		 domain_name[0] = '\0';
	 }

	 position += 2;
	 uint8_t record_type = packet[position];
	 position += 2;
	 uint8_t class_type = packet[position];

	 packet[position++] = 0xC0;
	 packet[position++] = 0x0C;
	 position++;
	 packet[position] = record_type;
	 position += 2;
	 packet[position++] = class_type;
	 position += 3;
	 packet[position] = 0x0F;
	 position += 2;
	 
	 if (record_type == 0x01) {
		packet[position++] = 0x04;
	 }

	 // Add IP to packet
	 packet[position++] = 0x0C;
     packet[position++] = 0x0C;
     packet[position++] = 0x0C;
     packet[position++] = 0x0F;

	 

	 // SQL QUERY HERE

	// uint8_t sumlables = packet[position];
	// while(1) {
	// 	if (packet[position + sumlables + 1] != 0x00) {
	// 		sumlables += packet[position + sumlables];
	// 	}
	// }
	// char delim = "";
	// char domain_name[sumlables+1];
	// while(1) {
	// 	uint8_t len_lable = packet[position];
	// 	for (int i = 0; i<len_lable; i++) {
	// 		domain_name[i] = packet[position + i + 1];
	// 	}
	// 	// append "."
	// }
	// for(int i= 0; i < domain_length; i++) {
	// 	printf("%c", packet[position+i]);
	// }

	// Here figure out how to construct the domain name string.
	
}