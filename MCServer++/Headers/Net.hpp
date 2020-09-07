#pragma once

#ifdef WIN32  // Winsock Includes

#define WIN32_LEAN_AND_MEAN

#include <iphlpapi.h>
#include <stdio.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")
#define WINSOCK
#else  // UNIX Socket Includes

#endif

namespace gls::TCP {
    class Server {
       private:
        std::string port;
        SOCKET serverSocket;
        bool initalized, listening;

       public:
        Server(std::string port) : port(port), listening(false) {
            initalized = false;
            int iResult;
#ifdef WINSOCK
            WSADATA wsaData;
            struct addrinfo *result = NULL, hints;
            serverSocket = INVALID_SOCKET;

            // Initialize Winsock
            printf("Initalizing Winsock... ");
            iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
            if (iResult != 0) {
                printf("WSAStartup failed: %d\n", iResult);
                return;
            }
            printf("Initalized\n");

            ZeroMemory(&hints, sizeof(hints));
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_protocol = IPPROTO_TCP;
            hints.ai_flags = AI_PASSIVE;

            // Resolve the local address and port to be used by the server
            printf("Resolving local address info... ");
            iResult = getaddrinfo(NULL, port.c_str(), &hints, &result);
            if (iResult != 0) {
                printf("getaddrinfo failed: %d\n", iResult);
                WSACleanup();
                return;
            }
            printf("Resolved\n");

            // Create a SOCKET for the server to listen for client connections
            printf("Creating Socket...");
            serverSocket = socket(result->ai_family, result->ai_socktype,
                                  result->ai_protocol);
            if (serverSocket == INVALID_SOCKET) {
                printf("Error at socket(): %d\n", WSAGetLastError());
                freeaddrinfo(result);
                WSACleanup();
                return;
            }

            // Setup the TCP listening socket
            iResult =
                bind(serverSocket, result->ai_addr, (int)result->ai_addrlen);
            if (iResult == SOCKET_ERROR) {
                printf("bind failed with error: %d\n", WSAGetLastError());
                freeaddrinfo(result);
                stop();
                return;
            }
            freeaddrinfo(result);

            // Set socket to Non-Blocking
            unsigned long mode = 1;
            iResult = ioctlsocket(serverSocket, FIONBIO, &mode);
            if (iResult == SOCKET_ERROR) {
                printf("ioctlsocket failed with error: %d\n",
                       WSAGetLastError());
                stop();
                return;
            }
            printf("Created\n");
#else

#endif
            initalized = true;
        }

        int start() {
            if (initalized) {
                printf("Starting Server... ");
                listening = true;
#ifdef WINSOCK
                if (::listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
                    printf("failed with error: %d\n", WSAGetLastError());
                    stop();
                    return 1;
                }
#else

#endif
                printf("Started\n");
                return 0;
            } else
                printf("Server has not been Initalized\n");
            return 1;
        }

        SOCKET accept() {
            if (listening)
                return ::accept(serverSocket, NULL, NULL);
            else
                printf("Server has not Started!\n");
            return INVALID_SOCKET;
        }

        int write(SOCKET target, char *data, size_t length) { int iResult; }

        int releaseClient(SOCKET clientSocket) {
            int iResult;
#ifdef WINSOCK
            // shutdown the connection since we're done
            printf("Shutting down socket... ");
            iResult = shutdown(clientSocket, SD_SEND);
            if (iResult == SOCKET_ERROR) {
                printf("shutdown failed with error: %d\n", WSAGetLastError());
                closesocket(clientSocket);
                return 1;
            }
            closesocket(clientSocket);
            printf("done\n");
#else

#endif
            return 0;
        }

        void stop() {
#ifdef WINSOCK
            closesocket(serverSocket);
            WSACleanup();
#else

#endif
        }
    };
}  // namespace gls::TCP