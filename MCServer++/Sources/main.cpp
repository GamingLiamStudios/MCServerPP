#include "Server.hpp"

int main() {
    Server server(25565);
    if (server.Start() != 0) {
        std::cout << "Unable to start server" << std::endl;
        return 1;
    }
    while (server.running) {
        // Console Commands
        server.Tick();
    }
    return 0;
}