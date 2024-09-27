/*
Author: Rui Wang
Class: ECE6122
Last Date Modified: Sep 26 2024
Description:
ECE Spaceship head file
*/

#pragma once
#include "ECE_LaserBlast.h"

class ECE_Spaceship : public sf::Sprite {
public:
    static sf::Texture spaceship_texture;

    // Constructor with position
    ECE_Spaceship(float x, float y);

    // Move the spaceship based on arrow key inputs and check boundary
    void move(float deltaTime, std::list<ECE_Mushroom>& mushrooms);

    // Fire a laser blast with checking space key
    void fireBlast(std::list<ECE_LaserBlast>& laserBalsts);

    // Method to update the fire status based on key state
    void updateFireStatus();

    // Check collision with Spider (if true, a new round or game over)
    bool checkCollisionWithSpider(ECE_Spider& spider);

    // Check with collision with Mushrooms
    bool checkCollisionWithOtherSprite(sf::Sprite sprite) const;

    // Hurt due to collision with centipede or spider
    void hurt();

    // Check whether spaceship dies or not
    bool isDead() const;

    // Get health value to draw the spaceship health
    const int getHealth() const;

    // Reset health to SPACESHIP_HEALTH
    void revise();

private:
    float speed = SPACESHIP_SPEED;
    int health = SPACESHIP_HEALTH;
    bool canFire = true;
};
