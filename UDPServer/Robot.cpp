/*
Author: Rui Wang
Class: ECE6122
Last Date Modified: Nov 14th, 2024
Description:
The definition of Robot Class.
*/

#include "Robot.h"

// Default constructor
Robot::Robot() {
    usingTexture = false;
    circleShape.setRadius(DIAMETER / 2.f);    // set radius
    circleShape.setFillColor(sf::Color::Red); // set color
    circleShape.setOrigin(circleShape.getRadius(), circleShape.getRadius()); // set origin
    circleShape.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f);   // initial position
    timer.restart();
}

// Constructor with loading texture
Robot::Robot(const sf::Texture& texture) {
    this->setTexture(texture);  // set texture
    usingTexture = true;
    this->setOrigin(texture.getSize().x / 2.f, texture.getSize().y / 2.f);  // set origin
    this->setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f); // initial position
    timer.restart();
}

// Draw robot with diffferent types
void Robot::draw(sf::RenderWindow& window) {
    if (usingTexture) {
        window.draw(*this);
    }
    else {
        window.draw(circleShape);
    }
}

// Move robot
void Robot::move() {
    // Calculate the elapsing time
    float elapedSecond = timer.getElapsedTime().asSeconds();
    // Check if 0.1 second has passed
    if (elapedSecond <= 0.1f) {
        return;     // don't move
    }
    // Reset the timer
    timer.restart();

    sf::Vector2f position = usingTexture ? this->getPosition() : circleShape.getPosition();
    sf::Vector2f distance = sf::Vector2f(velocity.x * speed * elapedSecond, velocity.y * speed * elapedSecond);
    sf::Vector2f nextPosition = position + distance;     // the next move position should be inside

    float textureWidth = usingTexture ? this->getGlobalBounds().width / 2.f : DIAMETER;
    float textureHeight = usingTexture ? this->getGlobalBounds().height / 2.f : DIAMETER;

    // Check and handle boundary collisions
    if (nextPosition.x <= 0 && velocity.x < 0) {
        nextPosition.x = textureWidth;     // remain at the left edge
    }
    if (nextPosition.x + DIAMETER >= WINDOW_WIDTH && velocity.x > 0) {
        nextPosition.x = WINDOW_WIDTH - textureWidth;     // remain at the right edge
    }
    if (nextPosition.y <= 0 && velocity.y < 0) {
        nextPosition.y = textureHeight;     // remain at the upper edge
    }
    if (nextPosition.y + DIAMETER >= WINDOW_HEIGHT && velocity.y > 0) {
        nextPosition.y = WINDOW_HEIGHT - textureHeight;  // remain at the lower edge
    }

    if (usingTexture) {
        sf::Sprite::setPosition(nextPosition);   // use the father class's move
    }
    else {
        circleShape.setPosition(nextPosition);
    }
}

// Update robot movement direction
void Robot::update(std::string input) {
    if (input == "w") {
        velocity = sf::Vector2f(0, -1.f);       // decrease y
        if (usingTexture) this->setRotation(0.f);   // turn up
    }
    else if (input == "s") {
        velocity = sf::Vector2f(0, 1.f);        // increase y
        if (usingTexture) this->setRotation(180.f); // turn down
    }
    else if (input == "a") {
        velocity = sf::Vector2f(-1.f, 0);       // decrease x
        if (usingTexture) this->setRotation(-90.f);  // turn left
    }
    else if (input == "d") {
        velocity = sf::Vector2f(1.f, 0);        // increase x
        if (usingTexture) this->setRotation(90.f); // turn right
    }
    else if (input == "g") {
        speed *= 2.f;                          // speed up by 2 times
    }
    else if (input == "h") {
        speed /= 2.f;                          // slow down by 2 times
    }
}