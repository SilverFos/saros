#include<linux/if_packet.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<cstdio>
#include<cerrno>
#include<linux/if_ether.h>
#include<net/if.h>
#include"socks.h"

int initsocket(std::string ifName)
{
	int sockFD = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)); //refer man(7) packet for info
	if(sockFD<0)
	{
		perror("Failed to open socket: ");
		return -1;
	}

	struct sockaddr_ll ether = {};
	ether.sll_family = AF_PACKET;
	ether.sll_protocol = htons(ETH_P_ALL); //host-t--network-short
	ether.sll_ifindex = if_nametoindex(ifName.c_str()); //needs net/if.h

	if(bind(sockFD, (struct sockaddr*) &ether, sizeof(struct sockaddr_ll)) == -1){
		perror("Failed to bind: ");
		return -1;
	}

	return sockFD;
}
