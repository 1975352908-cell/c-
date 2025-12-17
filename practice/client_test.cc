#include "udp_client.hpp"
#include <thread>
#include <chrono>

int main()
{
    UdpClient client(8082, "127.0.0.1");

    std::cout << "Input text, type 'quit' to exit\n";
    while (true) {
        std::string line;
        std::getline(std::cin, line);
        if (!std::cin) break;
        if (line == "quit") break;

        client.send(line);
        std::string resp;
        client.recv(resp);
    }
}