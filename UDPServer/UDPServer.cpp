
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

#define UDP_SERVER_PORT 61000
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define MOVING_SPEED 3.f
#define DIAMETER 10.f 
#define RECEIVE_BUFFER_SIZE 256

class Robot : public sf::Sprite {
public:
    // Default constructor
    Robot() {
        usingTexture = false;
        circleShape.setRadius(DIAMETER / 2);    // set radius
        circleShape.setFillColor(sf::Color::Red); // set color
        circleShape.setOrigin(circleShape.getRadius(), circleShape.getRadius()); // set origin
        circleShape.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);   // initial position
    }

    // Constructor with loading texture
    Robot(const sf::Texture& texture) {
        this->setTexture(texture);  // set texture
        usingTexture = true;
        float scaleX = DIAMETER / texture.getSize().x;  // set it as given size
        float scaleY = DIAMETER / texture.getSize().y;
        this->setScale(scaleX, scaleY);
        this->setOrigin(texture.getSize().x / 2.f, texture.getSize().y / 2.f);  // set origin
        this->setPosition(WINDOW_WIDTH / 2 , WINDOW_HEIGHT / 2); // initial position
    }

    // Deconstructor
    ~Robot() { std::cout << "delete Robot" << std::endl; }

    // Draw robot with diffferent types
    void draw(sf::RenderWindow& window) {
        if (usingTexture) {
            window.draw(*this);
        } else {
            window.draw(circleShape);
        }
    }

    // Update robot
    void update(std::string input) {
        if (input == "w") {
            velocity = sf::Vector2f(0, -1.f);       // decrease y
        } else if (input == "s") {
            velocity = sf::Vector2f(0, 1.f);        // increase y
        } else if (input == "a") {
            velocity = sf::Vector2f(-1.f, 0);       // decrease x
        } else if (input == "d") {
            velocity = sf::Vector2f(1.f, 0);        // increase x
        } else if (input == "g") {
            speed *= 2.0f;                          // speed up by 2 times
        } else if (input == "h") {
            speed /= 2.0f;                          // slow down by 2 times
        } else {
            velocity = sf::Vector2f(0.f, 0.f);      // remains original position
        }
        rotate();
        move();
    }

private:
    // Move robot
    void move() {
        sf::Vector2f position = usingTexture ? this->getPosition() : circleShape.getPosition();
        sf::Vector2f distance = sf::Vector2f(velocity.x * speed, velocity.y * speed);            
        sf::Vector2f nextPosition = position + distance;     // the next move position should be inside

        // Check and handle boundary collisions
        if (nextPosition.x <= 0 && velocity.x < 0) {
            nextPosition.x = DIAMETER;     // remain at the left edge
        }
        if (nextPosition.x + DIAMETER >= WINDOW_WIDTH && velocity.x > 0) {
            nextPosition.x = WINDOW_WIDTH - DIAMETER;     // remain at the right edge
        }
        if (nextPosition.y <= 0 && velocity.y < 0) {
            nextPosition.y = DIAMETER;     // remain at the upper edge
        }
        if (nextPosition.y + DIAMETER >= WINDOW_HEIGHT && velocity.y > 0) {
            nextPosition.y = WINDOW_HEIGHT - DIAMETER;  // remain at the lower edge
        }

        if (usingTexture) {
            sf::Sprite::setPosition(nextPosition);   // use the father class's move
        } else {
            circleShape.setPosition(nextPosition);
        }
    }

    // Rotate robot
    void rotate() {
        if (!usingTexture)
            return;                     // circle don't need rotate

        if (velocity.x < 0) {
            this->setRotation(180.f);   // turn left
        } else if (velocity.x > 0) {
            this->setRotation(0.f);     // turn right
        } else if (velocity.y < 0) {
            this->setRotation(-90.f);   // turn up
        } else if (velocity.y > 0) {
            this->setRotation(90.f);    // turn down
        }
    }

    sf::CircleShape circleShape;
    sf::Vector2f velocity;
    float speed = MOVING_SPEED;
    bool usingTexture;
};

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

    // After receving the initial message from client, draw the robot at the center of window
    Robot robot;
    window.clear(sf::Color::Black);
    robot.draw(window);
    window.display();

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
            std::cout << "Server terminates ..." << std::endl;
            break;
        }

        // Try to receive a message
        std::memset(in, 0, sizeof(in));                    
        if (socket.receive(in, sizeof(in), received, client, clientPort) == sf::Socket::Status::Done) {
            std::string cmd(in);  // Convert char array to string message
            std::cout << "Received command from client: " << cmd << std::endl;

            // Send confirmation of receiving to the client
            std::string response = "Server received: " + cmd;
            if (socket.send(response.c_str(), response.size() + 1, client, clientPort) != sf::Socket::Status::Done) 
                return 1;

            // Update the robot position
            robot.update(cmd);

            // Draw robot
            window.clear(sf::Color::Black);
            robot.draw(window);
            window.display();

            // Judge whether the client quits or not
            if (cmd == "Quit") {
                window.clear(sf::Color::Black);
                window.display();
                std::cout << "Client quits ..." << std::endl;
                break;
            }
        }

    }

    std::cout << "Press enter to exit..." << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    return 0;
}
