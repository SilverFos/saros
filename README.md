# saros

Packet sniffer. Captures Ethernet frames from a socket bound to an interface (currently only supports ethernet), parses them, and stores them in a SQLite database.

## Build
    cmake -S . -B build
    cmake --build build

## Run
    sudo ./build/parser

Captures from `enp2s0` (change in main.cpp). Requires root access to create sockets.
Ensure to run from within the root directory so `packets.db` is generated in the root.
