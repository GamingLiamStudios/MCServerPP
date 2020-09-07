#pragma once

#ifdef WIN32  // Winsock Includes

#define WIN32_LEAN_AND_MEAN

#include <iphlpapi.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")
#define WINSOCK
#elif _unix_  // UNIX Socket Includes
#include <sys/socket.h>
#include <sys/types.h>

#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif

#include <iostream>
#include <vector>

struct Connection {
    SOCKET socket;
    enum State { Handshake, Status, Login, Play } currentState;
    // Add any user-specific data here
};

struct Packet {
    int ID;
    int size;
    char *data;
};

class Server {
   private:
    unsigned short port;
    SOCKET serverSocket;
    unsigned long tps;
    std::vector<Connection> clients;

   public:
    bool running;

    Server(unsigned short port);

    int Start();

    int Tick();
    void RunCommand(std::string cmd);

    int Stop();

   private:
    void AddClients();
    bool HandleNextPacket(Connection client);
};

void PrintHexMemory(void *mem, int len) {
    int i;
    unsigned char *p = (unsigned char *)mem;
    for (i = 0; i < len; i++) {
        printf("0x%02x ", p[i]);
        if ((i % 16 == 0) && i) printf("\n");
    }
    printf("\n");
}