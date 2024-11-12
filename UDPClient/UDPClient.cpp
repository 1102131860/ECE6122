
#include <SFML/Network.hpp>
#include <iostream>
#include <string>

#define UDP_SERVER_PORT 61000

int main(int argc, char* argv[])
{
    // Ask for the server address
    sf::IpAddress server;
    do
    {
        std::cout << "Type the address or name of the server to connect to: ";
        std::cin >> server;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    } while (server == sf::IpAddress::None);

    // Create a socket for communicating with the server
    sf::UdpSocket socket;

    // Ask the server's dedicated UDP port, the initial port is pre-defined
    const char greeting[] = "Hi, this is client. Please tell me your dedicated port";
    if (socket.send(greeting, sizeof(greeting), server, UDP_SERVER_PORT) != sf::Socket::Status::Done)
        return 1;

    char                in[128] = {};                       // recieve buffer
    std::size_t         received = 0;                       // number of received bytes
    unsigned short      serverPort = 0;                     // store the server's port

    // Receive the dedicated port from the server
    if (socket.receive(in, sizeof(in), received, server, serverPort) != sf::Socket::Status::Done)
        return 1;
    std::cout << "Message received from the server: " << in << std::endl;
    std::cout << "The new UDP port is: " << serverPort << std::endl;

    while (true) {
        std::string input;
        std::cout << "Please choose one operation from (w: move up; s: move down; a: move left, d: move right, g: speed up, h: slow down): ";
        std::getline(std::cin, input);

        if (input == "q") {
            std::string quitMessage = "quit";
            socket.send(quitMessage.c_str(), quitMessage.size() + 1, server, serverPort);
            std::cout << "Client quits ..." << std::endl;
            break;
        }

        // Send a message to the server
        if (socket.send(input.c_str(), input.size() + 1, server, serverPort) != sf::Socket::Status::Done)
            return 1;
        
        // clear receive buffer
        std::memset(in, 0, sizeof(in));
        
        // Receive a message from the server
        if (socket.receive(in, sizeof(in), received, server, serverPort) != sf::Socket::Status::Done)
            return 1;
        std::cout << "Message received from the server: " << in << std::endl;
    }
    
    // Wait until the user presses 'enter' key
    std::cout << "Press enter to exit..." << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
