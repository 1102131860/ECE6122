/*
Author: Rui Wang
Class: ECE6122
Last Date Modified: Sep 26 2024
Description:
ECE LaserBlast source file
*/

#include "ECE_LaserBlast.h"

// Constructor with position
ECE_LaserBlast::ECE_LaserBlast(float x, float y) {
    laserShape.setSize(LASERBLAST_SIZE);  // Set laser size
    laserShape.setFillColor(LASERBLAST_COLOR);  // Set laser color
    laserShape.setPosition(x, y);  // Set initial position
}

// Move the laser blast
void ECE_LaserBlast::move(float deltaTime) {
     laserShape.move(0, -speed * deltaTime);  // Move the laser upward
}

// Check whether a laser blast flies out of window (erase from its list)
bool ECE_LaserBlast::isOutOfWindow() {
    return laserShape.getPosition().y < INFO_AREA_HEIGHT;
}

// Check collision with mushroom (if true, remove laserBlast from its list)
bool ECE_LaserBlast::checkCollisionWithMushrooms(std::list<ECE_Mushroom>& mushrooms) {
	for (auto mushroomItem = mushrooms.begin(); mushroomItem != mushrooms.end();) {
		if (checkCollisionWithOtherSprite(*mushroomItem)) {
			mushroomItem->hitByLaserBlaster();
			if (mushroomItem->isDead()) {
				mushroomItem = mushrooms.erase(mushroomItem);
			} 
			return true;
		}
		else {
			++mushroomItem;
		}
	}
	return false;
}

// Check collision with spider (if true, remove laserBlast from its list, get bonus on score)
bool ECE_LaserBlast::checkCollisionWithSpider(ECE_Spider& spider) const {
	if (checkCollisionWithOtherSprite(spider)) { 
		spider.hurt(); 
		return true;
	}
	return false;
}

// Check whether collide with other sprite
bool ECE_LaserBlast::checkCollisionWithOtherSprite(sf::Sprite thatSprite) const {
	return getGlobalBounds().intersects(thatSprite.getGlobalBounds());
}

// Accessor to get the shape for rendering
const sf::RectangleShape& ECE_LaserBlast::getShape() const {
    return laserShape;
}

// Access the global bounds of the laser
sf::FloatRect ECE_LaserBlast::getGlobalBounds() const {
    return laserShape.getGlobalBounds();  // Return the boundary of the laserShape
}

