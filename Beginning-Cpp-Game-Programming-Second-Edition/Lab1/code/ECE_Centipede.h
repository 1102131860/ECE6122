/*
Author: Rui Wang
Class: ECE6122
Last Date Modified: Sep 26 2024
Description:
ECE Centipede head file
*/

#pragma once
#include <cmath>
#include "ECE_Spaceship.h"

enum class CentipedeHitStatus {
    NotHit = 0,
    HitBody = 1,
    HitHead = 2
};

class ECE_Centipede : public sf::Sprite {
public:
    // Static textures for all centipede segments
    static sf::Texture centipedeHeadTexture;
    static sf::Texture centipedeBodyTexture;

    // Constructor declaration
    ECE_Centipede() {};                                // no thing to do here, just for split function;
    ECE_Centipede(int numSegments, float startX, float startY);

    // Update centipede movement with checking collision with mushrooms
    void moveWithCheckingCollisionWithMushrooms(float deltaTime, std::list<ECE_Mushroom>& mushrooms);

    // Method to check collisions with laser blasts (if true, remove laserBlast from its list, and get score)
    CentipedeHitStatus checkCollisionWithLaserBlasts(std::list<ECE_LaserBlast>& laserBlasts, std::list<ECE_Mushroom>& mushrooms, std::list<ECE_Centipede>& centipedes);

    // Method to check collisions with spaceship (if true, a new round or game end)
    bool checkCollisionWithSpaceship(ECE_Spaceship& spaceship);

    // Function to calculate distance between two points
    float distance(sf::Vector2f a, sf::Vector2f b);

    // Function to normalize a vector
    sf::Vector2f normalize(sf::Vector2f v);

    // Check centipede is dead or not
    bool isDead() const;

    // Draw the centipede
    void draw(sf::RenderWindow& window) const;

private:
    // Nested Segment class
    class Segment :public sf::Sprite {
    public:
        sf::Vector2f direction = { -1.f, 0.f };   // initially moves to the left;
        bool isMovingUp = false;                // initially moves to the lower;

        // Constructor
        Segment(sf::Texture& texture, float x, float y);

        // Move function
        void moveWithCheckingMushroomCollision(float speed, float deltaTime, bool collideWithMushroom);

        // Check if intersect with other spirt
        bool checkCollisionWithOtherSprit(sf::Sprite thatSpirt);
    };

    // List of centipede segments (head + body)
    std::list<Segment> segments;
    float speed = CENTIPEDE_SPEED;

    // Private method for splitting the centipede
    void splitCentipede(std::list<ECE_Centipede>& centipedes, std::list<Segment>::iterator hitSegment);
};
