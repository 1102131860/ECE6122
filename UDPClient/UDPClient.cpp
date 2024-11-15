/*
Author: Rui Wang
Class: ECE6122
Last Date Modified: Nov 19th, 2024
Description:
This is the main function that implements the UDP connection at the client terminal.
*/

#include <SFML/Network.hpp>
#include <cstring>
#include <iostream>

#define UDP_SERVER_PORT 61000

int main(int argc, char* argv[])
{
    // Ask for the server address
    sf::IpAddress server;
    do
    {
        std::cout << "Type the address or name of the server to connect to: ";
        std::cin >> server;
        std::cin.ignore(10000, '\n');
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

    // Main loop
    while (true) {
        // Obtain input from user
        std::string input;
        std::cout << "w: move up; s: move down; a: move left; d: move right; g: speed up; h: slow down\nChoose one operation: ";
        std::getline(std::cin, input);

        // Judge client input "q"
        if (input == "q") {
            std::string quitMessage = "Quit";
            if (socket.send(quitMessage.c_str(), quitMessage.size() + 1, server, serverPort) != sf::Socket::Status::Done)
                return 1;
            std::cout << "Client quits ..." << std::endl;
            break;
        }

        // Send a message to the server
        if (socket.send(input.c_str(), input.size() + 1, server, serverPort) != sf::Socket::Status::Done)
            return 1;

        // Set the socket as the block mode (to make the receiving blocked)
        memset(in, 0, sizeof(in));
        if (socket.receive(in, sizeof(in), received, server, serverPort) == sf::Socket::Status::Done) {
            std::cout << "Message received from the server: " << in << std::endl;
        } else {
            std::cout << "Server terminates ... " << std::endl;
            break;
        }
    }

    std::cout << "Press enter to exit..." << std::endl;
    std::cin.ignore(10000, '\n');

    return 0;
}
