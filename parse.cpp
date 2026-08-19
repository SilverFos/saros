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

//struct eth_sockaddr{
//	uint16_t family;
//	uint16_t protocol;
//	int interface;
//};

struct etherFrame{
	std::array<uint8_t, ETH_ALEN> destAdd = {};
	std::array<uint8_t, ETH_ALEN> srcAdd = {};
	uint16_t ethType;
	std::vector<uint8_t> payload;

};	
int main()
{
	int socks = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (socks<0){
		perror("Failed to open socket: ");
		return 1;
	}
	struct sockaddr_ll ether;
	memset(&ether, 0, sizeof(ether));
	ether.sll_family = AF_PACKET;
	ether.sll_protocol = htons(ETH_P_ALL);
	ether.sll_ifindex = if_nametoindex("enp2s0");
	struct etherFrame frame;
	if(bind(socks, (struct sockaddr *) &ether, sizeof(struct sockaddr_ll)) == -1){
		perror("Failed to bind: ");
		return 1;
	}
	uint8_t buffer[2048] = {};
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
		printf("Source MAC: %02X:%02x:%02X:%02X:%02X:%02X \n",
			frame.srcAdd[0], frame.srcAdd[1], frame.srcAdd[2], 
			frame.srcAdd[3], frame.srcAdd[4], frame.srcAdd[5]);
		if(frame.ethType>=1536){
			printf("Ethernet II (DIX) Protocol: %04X\n", frame.ethType);
		}else{
			printf("IEEE 802.3 frame. Size: %d\n", frame.ethType);
		}
	}
	return 0;	
}
