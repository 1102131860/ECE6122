/*
Author: Rui Wang
Class: ECE6122
Last Date Modified: Sep 26 2024
Description:
ECE Spider source file
*/

#include "ECE_Spider.h"

// Define the static texture member
sf::Texture ECE_Spider::spider_texture;

// Constructor with position
ECE_Spider::ECE_Spider(float x, float y) {
	setTexture(spider_texture);
	setPosition(x, y);
	std::srand(static_cast<unsigned>(std::time(0)));
	setRandomDirection();
	timer.restart();
}

// Move the spider
void ECE_Spider::move(float deltaTime) {
	sf::Sprite::move(deltaTime * speed * direction.x, deltaTime * speed * direction.y);
	
	// Check if 1 second has passed
	if (timer.getElapsedTime().asSeconds() >= 1.0f) {
		setRandomDirection();
		timer.restart(); // Reset the timer
	}

	// Check with collision with boundary
	if (getPosition().x <= 0 || getPosition().x + getGlobalBounds().width >= WINDOW_WIDTH)
		direction.x *= -1;
	if (getPosition().y <= INFO_AREA_HEIGHT || getPosition().y + getGlobalBounds().height >= WINDOW_HEIGHT)
		direction.y *= -1;
}

// Set random direction
void ECE_Spider::setRandomDirection() {
	int randomX = rand() % 3 - 1;	// -1, 0, 1 
	int randomY = rand() % 3 - 1;	// -1, 0, 1

	if (randomX == 0 && randomY == 0)	// avoid remaining without moving
		randomY = 1;

	direction.x = static_cast<float>(randomX);
	direction.y = static_cast<float>(randomY);
}

// Check whether collides with mushroom and destory it
bool ECE_Spider::checkCollisionWithMushrooms(std::list<ECE_Mushroom>& mushrooms) {
	for (auto mushroomItem = mushrooms.begin(); mushroomItem != mushrooms.end();) {
		if (checkCollisionWithOtherSprite(*mushroomItem)) {
			mushroomItem->hitBySpider();
			mushroomItem = mushrooms.erase(mushroomItem);
			return true;
		}
		else {
			++mushroomItem;
		}
	}
	return false;
}

// Check whether spider collides with other spirte
bool ECE_Spider::checkCollisionWithOtherSprite(sf::Sprite thatSprit) {
	return getGlobalBounds().intersects(thatSprit.getGlobalBounds());
}

// Hurt spider
void ECE_Spider::hurt() {
	if (health > 0) health--;
}

// Check whether spider is dead or not
const bool ECE_Spider::isDead() const {
	return health == 0;
}

// Set the health to SPIDER_HEALTH
void ECE_Spider::revise() {
	health = SPIDER_HEALTH;
}