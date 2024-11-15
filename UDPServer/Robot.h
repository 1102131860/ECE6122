/*
Author: Rui Wang
Class: ECE6122
Last Date Modified: Nov 14th, 2024
Description:
This is the robot class header file, declaring the attributes and methods of Robot.
*/

#include <SFML/Graphics.hpp>
#include <iostream>
#include "Config.h"

class Robot : public sf::Sprite {
public:
    // Default constructor
    Robot();

    // Constructor with loading texture
    Robot(const sf::Texture& texture);

    // Deconstructor
    ~Robot() { std::cout << "delete Robot" << std::endl; }

    // Set robot as dead
    void dies() { isLive = false; std::cout << "Robot dies" << std::endl; }

    // Judge robot is live or not
    bool isDead() const { return !isLive; }

    // Draw robot with diffferent types
    void draw(sf::RenderWindow& window);

    // Move robot
    void move();

    // Update robot movement direction
    void update(std::string input);

private:
    sf::Vector2f velocity = sf::Vector2f(0.f, 0.f);
    float speed = MOVING_SPEED;
    bool isLive = true;
    bool usingTexture;
    sf::CircleShape circleShape;
    sf::Clock timer;
};
