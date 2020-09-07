#include "Server.hpp"

Server::Server(unsigned short port)
    : port(port), tps(20), clients(), running(false) {
    int iResult;
    serverSocket = INVALID_SOCKET;
#ifdef WINSOCK
    WSADATA wsaData;

    // Initialize Winsock
    printf("Initalizing Winsock... ");
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return;
    }
    printf("Initalized\n");
#endif

    sockaddr_in hint{};
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    hint.sin_addr.S_un.S_addr = INADDR_ANY;

    // Create a SOCKET for the server to listen for client connections
    printf("Creating Socket...");
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
#ifdef WINSOCK
        printf("socket failed with error: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
#elif _unix_
        perror("socket failed\n");
        close(serverSocket);
#endif
        return;
    }

    // Setup the TCP listening socket
    iResult = bind(serverSocket, (sockaddr *)&hint, sizeof(hint));
    if (iResult == SOCKET_ERROR) {
#ifdef WINSOCK
        printf("bind failed with error: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
#elif _unix_
        perror("bind failed\n");
        close(serverSocket);
#endif
        return;
    }

    // Set socket to Non-Blocking
    unsigned long mode = 1;
#ifdef WINSOCK
    iResult = ioctlsocket(serverSocket, FIONBIO, &mode);
#elif _unix_
    iResult = ioctl(serverSocket, FIONBIO, &mode);
#endif
    if (iResult == SOCKET_ERROR) {
#ifdef WINSOCK
        printf("ioctlsocket failed with error: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
#elif _unix_
        perror("ioctl failed\n");
        close(serverSocket);
#endif
        return;
    }
    printf("Created\n");
}

int Server::Start() {
    printf("Starting Server... ");
#ifdef WINSOCK
    if (::listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        printf("failed with error: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
#else

#endif
    printf("Started\n");
    running = true;
    return 0;
}

int Server::Tick() {
    // Packet Logic here
    AddClients();
    for (Connection client : clients) {
        if (!HandleNextPacket(client)) {
            // Handle Packet Failure
            running = false;
        }
    }
    return 0;
}

void Server::AddClients() {
    // Search for new clients
    SOCKET client;
    while ((client = ::accept(serverSocket, NULL, NULL)) != INVALID_SOCKET) {
        // Add Clients to list with Handshake State
        Connection user;
        user.socket = client;
        user.currentState = user.Handshake;
        clients.push_back(user);
    }
}

bool Server::HandleNextPacket(Connection client) {
    Packet incoming;
    int iResult;

    // Read Packet Length + ID size
    int bytesRead = 0;
    int packetSize = 0;
    char *read = static_cast<char *>(malloc(1));
    do {
        memset(read, 0, 1);
        iResult = recv(client.socket, read, 1, 0);
        if (iResult < 0) {
            std::cerr << "Error while Reading Packet Length" << std::endl;
            return false;
        }
        int value = *read & 0x7F;
        packetSize |= value << (7 * bytesRead);

        if (bytesRead++ > 5) {
            std::cerr << "Packet Length is not Valid" << std::endl;
            return false;
        }
    } while ((*read & 0x80) != 0);

    // Read Packet ID
    bytesRead = 0;
    incoming.ID = 0;
    do {
        memset(read, 0, 1);
        iResult = recv(client.socket, read, 1, 0);
        if (iResult < 0) {
            std::cerr << "Error while Reading Packet ID" << std::endl;
            return false;
        }
        int value = *read & 0x7F;
        incoming.ID |= value << (7 * bytesRead);

        if (bytesRead++ > 5) {
            std::cerr << "Packet ID is not Valid" << std::endl;
            return false;
        }
    } while ((*read & 0x80) != 0);

    // Read Packet Contents
    incoming.size = packetSize - bytesRead;
    incoming.data = static_cast<char *>(malloc(incoming.size));
    memset(incoming.data, 0, incoming.size);
    iResult = recv(client.socket, incoming.data, incoming.size, 0);
    if (iResult != incoming.size) {
        std::cerr << "Error while Reading Packet Contents" << std::endl;
        return false;
    }

    // Handle Packet
    return true;
}
