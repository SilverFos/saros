#include<array>
#include<vector>
#include<cstdio>
#include<net/if.h>
#include<cstring>
#include<cstdlib>
#include"socks.h"
#include"parse.h"
#include"database.h"
#include<linux/if_ether.h>

//struct eth_sockaddr{
//	uint16_t family;
//	uint16_t protocol;
//	int interface;
//};

//struct etherFrame{
//	std::array<uint8_t, ETH_ALEN> destAdd = {};
//	std::array<uint8_t, ETH_ALEN> srcAdd = {};
//	uint16_t ethType;
//	std::vector<uint8_t> payload;
//
//};

//void check_error(uint8_t rc, sqlite3 *db) {
//	if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) // not_executed/op_not_completed/v2-indicating_no_new_row_added
//	{
//		fprintf(stderr, "SQLite error: %s", sqlite3_errmsg(db));
//		sqlite3_close(db);
//		exit(1);
//	}
//}

char * macToString(char * buffer, size_t bufSize, const std::array<uint8_t, ETH_ALEN> &addr)
{
	snprintf(buffer, bufSize, "%02X:%02x:%02X:%02X:%02X:%02X", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
	return buffer;
}

etherFrame *frameParse(uint8_t *buffer, ssize_t frameSize, etherFrame &frame)
{
	if(frameSize<ETH_HLEN){
		perror("Failed to obtain frame.");
		return nullptr;
	}
	for( int i = 0; i < 6; i++){
		frame.destAdd[i] = buffer[i];
		frame.srcAdd[i] = buffer[i+6];
	}
	frame.ethType = (buffer[12] << 8) | buffer[13];
	frame.payload.assign(buffer + ETH_HLEN, buffer + frameSize);
	printf("EtherType: 0x%04X\n", frame.ethType);
	printf("Destination MAC: %02X:%02x:%02X:%02X:%02X:%02X \n",
		frame.destAdd[0], frame.destAdd[1], frame.destAdd[2],
       		frame.destAdd[3], frame.destAdd[4], frame.destAdd[5]);
	
	printf("Source MAC: %02X:%02x:%02X:%02X:%02X:%02X \n",
		frame.srcAdd[0], frame.srcAdd[1], frame.srcAdd[2], 
		frame.srcAdd[3], frame.srcAdd[4], frame.srcAdd[5]);

	if(frame.ethType>=1536){
		printf("Ethernet II (DIX) Protocol: %04X\n", frame.ethType);
	}else{
		printf("IEEE 802.3 frame. Size: %d\n", frame.ethType);
	}

	return &frame;	
}
