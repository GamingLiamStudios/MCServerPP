#include "Server.hpp"

int main() {
    gls::TCP::Server server("25565");
    if (server.start() != 0) {
        std::cout << "Unable to start server" << std::endl;
        return 1;
    }
    SOCKET client;
    while ((client = server.accept()) == INVALID_SOCKET)
        ;
    server.releaseClient(client);
    server.stop();

    return 0;
}