#pragma once
#include<array>
#include<vector>
#include<linux/if_ether.h>
#include<string>
#include<cstdint>

struct etherFrame{
        std::array<uint8_t, ETH_ALEN> destAdd = {}; 
        std::array<uint8_t, ETH_ALEN> srcAdd = {}; 
        uint16_t ethType;
        std::vector<uint8_t> payload;

};

char * macToString(char * buffer, size_t bufSize, const std::array<uint8_t, ETH_ALEN> &addr);
etherFrame *frameParse(uint8_t *buffer, ssize_t frameSize, etherFrame &frame);
