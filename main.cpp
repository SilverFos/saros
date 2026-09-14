#include"socks.h"
#include"parse.h"
#include"database.h"
#include<net/ethernet.h>
#include<cstring>
#include<cstdint>
#include<sys/socket.h>

int main()
{
        int socks = initsocket("enp2s0"); // returns a file descriptor
        struct etherFrame frame;
        struct Database DB;
        uint8_t buffer[2048] = {};
        std::string dbName = "packets.db";

        openDatabase(DB, dbName);
        initdb(DB.db);

	while(true)
	{
		ssize_t frameSize = recv(socks, buffer, sizeof(buffer), 0);
		if(frameParse(buffer, frameSize, frame) == nullptr) continue;
	        store(frame, DB);
        }

        closeDatabase(DB);
        return 0;

}
