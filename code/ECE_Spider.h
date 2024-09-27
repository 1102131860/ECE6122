/*
Author: Rui Wang
Class: ECE6122
Last Date Modified: Sep 26 2024
Description:
ECE Spider head file
*/

#pragma once
#include <ctime>    // For time()
#include <cstdlib>  // For rand() and srand()
#include <list>
#include "ECE_Mushroom.h"

class ECE_Spider : public sf::Sprite {
public:
	static sf::Texture spider_texture;

	// Constructor with position
	ECE_Spider(float x, float y);

	// Move the spider
	void move(float deltaTime);

	// Set random direction
	void setRandomDirection();

	// Check whether collides with mushroom and destory it
	bool checkCollisionWithMushrooms(std::list<ECE_Mushroom>& mushrooms);

	// Check whether spider collides with other spirte
	bool checkCollisionWithOtherSprite(sf::Sprite thatSprite);

	// Hurt spider
	void hurt();

	// Check whether spider is dead or not
	const bool isDead() const;

	// Set the health to SPIDER_HEALTH
	void revise();

private:
	sf::Vector2f direction;
	sf::Clock timer;
	float speed = SPIDER_SPEED;
	int health = SPIDER_HEALTH;
};