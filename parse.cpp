#include<sys/socket.h>
#include<linux/if_ether.h>
#include<arpa/inet.h>
#include<cerrno>
#include<linux/if_packet.h>
#include<net/ethernet.h>
#include<array>
#include<vector>
#include<cstdio>
#include<net/if.h>
#include<cstring>
#include<string>
#include<sqlite3.h>
#include"socks.h"
#include"parse.h"
#include"database.h"

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

int main()
{
	int socks = initsocket("enp2s0"); // returns a file descriptor
	struct etherFrame frame;
	uint8_t buffer[2048] = {};
	char macBuffer[50] = {};

	sqlite3* db;
	std::string dbName = "packets.db";
	uint8_t ec = sqlite3_open(dbName.c_str(), &db); //constructor. writes a pointer of packets.db int db.
						      //returns SQLITE_OK or error_code
	check_error(ec, db);

	char *errMsg = nullptr;
	initdb(db, &errMsg);	
	std::string insert = "INSERT INTO packet(destaddr, srcaddr, ethType, frmType, payloadSize)  VALUES (?, ?, ?, ?, ?);"; 
	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, insert.c_str(), insert.size(), &stmt, nullptr);
	while(true){
		ssize_t frameSize = recv(socks, buffer, sizeof(buffer), 0);
		if(frameSize<0){
			perror("Failed to obtain frame.");
			return 1;
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
		std::string dest(macToString(macBuffer, sizeof(macBuffer), frame.destAdd));
		sqlite3_bind_text(stmt, 1, dest.c_str(), dest.size(), SQLITE_TRANSIENT);

		printf("Source MAC: %02X:%02x:%02X:%02X:%02X:%02X \n",
			frame.srcAdd[0], frame.srcAdd[1], frame.srcAdd[2], 
			frame.srcAdd[3], frame.srcAdd[4], frame.srcAdd[5]);
		std::string src(macToString(macBuffer, sizeof(macBuffer), frame.srcAdd));
                sqlite3_bind_text(stmt, 2, src.c_str(), src.size(), SQLITE_TRANSIENT);

		sqlite3_bind_int(stmt, 3, frame.ethType);	
		if(frame.ethType>=1536){
			printf("Ethernet II (DIX) Protocol: %04X\n", frame.ethType);
			sqlite3_bind_text(stmt, 4, "Ethernet II (DIX)", -1, SQLITE_TRANSIENT);
		}else{
			printf("IEEE 802.3 frame. Size: %d\n", frame.ethType);
			sqlite3_bind_text(stmt, 4, "IEEE 802.3 Frame", -1, SQLITE_TRANSIENT);
		}

		sqlite3_bind_int(stmt, 5, frame.payload.size());

		sqlite3_step(stmt);
		sqlite3_reset(stmt);
		sqlite3_clear_bindings(stmt);
	}

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return 0;	
}
