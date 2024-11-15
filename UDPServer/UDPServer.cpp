/*
Author: Rui Wang
Class: ECE6122
Last Date Modified: Nov 14th, 2024
Description:
This is the main function that implements UDP connection at the server terminal.
*/

#include <SFML/Network.hpp>
#include "Robot.h"

int main(int argc, char* argv[])
{
    // Initialize a blank black SFML window
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Server Window");
    window.clear(sf::Color::Black);
    window.display();

    // Create a initial socket to receive a message from client
    sf::UdpSocket initialSocket;
    if (initialSocket.bind(UDP_SERVER_PORT) != sf::Socket::Status::Done)
        return 1;
    std::cout << "Server is initially listening to port " << UDP_SERVER_PORT << " to tell the client a dedicated port" << std::endl;

    char                in[RECEIVE_BUFFER_SIZE] = {};       // recieve buffer
    std::size_t         received = 0;                       // number of received bytes
    sf::IpAddress       client;                             // store the client's IP address
    unsigned short      clientPort = 0;                     // store the client's port

    // Wait for the initial message from client
    if (initialSocket.receive(in, sizeof(in), received, client, clientPort) != sf::Socket::Status::Done)
        return 1;
    std::cout << "Connected to client: " << client << " on port " << clientPort << "\nReceived Message (" << received << " bytes): " << in << std::endl;
    
    // Close initial socket (default port not user-define port)
    initialSocket.unbind();

    // User defined port
    unsigned short port = UDP_SERVER_PORT;
    if (argc == 3 && strcmp(argv[1], "-p") == 0) {
        try {
            port = std::stoi(argv[2]);
        } catch (const std::invalid_argument& e) {
            std::cout << "Invalid argument (-p): " << e.what() << std::endl;
            port = UDP_SERVER_PORT;
        } catch (const std::out_of_range& e) {
            std::cout << "Out of range (-p): " << e.what() << std::endl;
            port = UDP_SERVER_PORT;
        }
    }

    // Now change to the communication socket
    sf::UdpSocket socket;
    if (socket.bind(port) != sf::Socket::Status::Done)
        return 1;
    std::cout << "Server now changes to listen a dedicated port " << port << " and waiting for message" << std::endl;

    // Send the dedicated port to client
    std::string greeting = "Hi, this is server. Now we can communicate at port " + std::to_string(port);
    if (socket.send(greeting.c_str(), greeting.size() + 1, client, clientPort) != sf::Socket::Status::Done)
        return 1;

    // After receving the initial message from client, create a new robot
    Robot* robot;
    sf::Texture robotTexture;
    std::string textureAddress = "./graphics/robot.png";
    robot = robotTexture.loadFromFile(textureAddress) ? new Robot(robotTexture) : new Robot();

    // Set the socket to non-blocking mode (to make the receiving not blocked)
    socket.setBlocking(false);

    // Check whether window is closed
    bool windowIsClosed = false;

    // Main loop
    while (true) {
        // Poll for window events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                windowIsClosed = true;
            }
        }
        // If press esc, window exits
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            window.close();
            windowIsClosed = true;
        }

        // Tell client that server terminates
        if (windowIsClosed) {
            delete robot;   // delete robot
            std::cout << "Server terminates ..." << std::endl;
            std::string response = "Terminates";
            if (socket.send(response.c_str(), response.size() + 1, client, clientPort) != sf::Socket::Status::Done)
                return 1;
            break;
        }

        // Draw robot
        window.clear(sf::Color::Black);
        // Move robot
        if (!robot->isDead()) {
            robot->move();
            robot->draw(window);
        }
        window.display();

        // Try to receive a message
        std::memset(in, 0, sizeof(in));                    
        if (socket.receive(in, sizeof(in), received, client, clientPort) == sf::Socket::Status::Done) {
            std::string cmd(in);  // Convert char array to string message
            std::cout << "Received command from client: " << cmd << std::endl;

            // Send confirmation of receiving to the client
            std::string response = "Server received: " + cmd;
            if (socket.send(response.c_str(), response.size() + 1, client, clientPort) != sf::Socket::Status::Done) 
                return 1;

            // Judge whether the client quits or not
            if (cmd == "Quit") {
                robot->dies();
                std::cout << "Client quits ..." << std::endl;
            } else {
                robot->update(cmd);
            }
        }
    }

    std::cout << "Press enter to exit..." << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    return 0;
}
