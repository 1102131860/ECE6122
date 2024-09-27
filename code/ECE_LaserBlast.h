/*
Author: Rui Wang
Class: ECE6122
Last Date Modified: Sep 26 2024
Description:
ECE LaserBlast head file
*/

#pragma once
#include "ECE_Spider.h"

class ECE_LaserBlast : public sf::Sprite {
public:
	// Constructor with position
	ECE_LaserBlast(float x, float y);

	// Move the laser blast
	void move(float deltaTime);

	// Check whether a laser blast flies out of window (erase from its list)
	bool isOutOfWindow();

	// Check collision with mushroom (if true, remove laserBlast from its list, get bonus score)
	bool checkCollisionWithMushrooms(std::list<ECE_Mushroom>& mushrooms);

	// Check collision with spider (if true, remove laserBlast from its list, get bonus score)
	bool checkCollisionWithSpider(ECE_Spider& spider) const;

	// Check whether collide with other sprite
	bool checkCollisionWithOtherSprite(sf::Sprite thatSprite) const;

	// Access the shape for drawing
	const sf::RectangleShape& getShape() const;

	// Access the global bounds of the laser
	sf::FloatRect getGlobalBounds() const;
	
private:
	sf::RectangleShape laserShape;
	float speed = LASERBLAST_SPEED;
};