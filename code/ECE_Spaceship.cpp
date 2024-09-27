/*
Author: Rui Wang
Class: ECE6122
Last Date Modified: Sep 26 2024
Description:
ECE Spaceship source file
*/

#include "ECE_Spaceship.h"

// Define the static texture member
sf::Texture ECE_Spaceship::spaceship_texture;

// Constructor with position
ECE_Spaceship::ECE_Spaceship(float x, float y) {
	setTexture(spaceship_texture);
	setPosition(x, y);
}

// Move the spaceship based on arrow key inputs and check boundary 
void ECE_Spaceship::move(float deltaTime, std::list<ECE_Mushroom>& mushrooms) {
	bool collisionWithMushroomLeftSide = false;
	bool collisionWithMushroomRightSide = false;
	bool collisionWithMushroomUpSide = false;
	bool collisionWithMushroomDownSide = false;
	for (auto& mushroom : mushrooms) {
		if (checkCollisionWithOtherSprite(mushroom)) {
			if (getPosition().x <= mushroom.getPosition().x + mushroom.getGlobalBounds().width) {
				collisionWithMushroomLeftSide = true;
			}
			if (getPosition().x + getGlobalBounds().width >= mushroom.getPosition().x) {
				collisionWithMushroomRightSide = true;
			}
			if (getPosition().y >= mushroom.getPosition().y + mushroom.getGlobalBounds().height) {
				collisionWithMushroomUpSide = true;
			}
			if (getPosition().y + getGlobalBounds().height <= mushroom.getPosition().y) {
				collisionWithMushroomDownSide = true;
			}
		}
	}
	if (getPosition().x >= 0 && sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && !collisionWithMushroomLeftSide)
		sf::Sprite::move(-speed * deltaTime, 0);
	if (getPosition().x + getGlobalBounds().width <= WINDOW_WIDTH && sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && !collisionWithMushroomRightSide)
		sf::Sprite::move(speed * deltaTime, 0);
	if (getPosition().y >= INFO_AREA_HEIGHT && sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && !collisionWithMushroomUpSide)
		sf::Sprite::move(0, -speed * deltaTime);
	if (getPosition().y + getGlobalBounds().height <= WINDOW_HEIGHT && sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && !collisionWithMushroomDownSide)
		sf::Sprite::move(0, speed * deltaTime);
}

// Fire a laser blast with checking space key
void ECE_Spaceship::fireBlast(std::list<ECE_LaserBlast>& laserBlasts) {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && canFire) {
		laserBlasts.emplace_back(getPosition().x, getPosition().y);
		canFire = false;
	}
}

void ECE_Spaceship::updateFireStatus() {
	// If the space key is not pressed, allow firing again
	if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		canFire = true;
}

// Check collision with Spider (if true, a new round or game over)
bool ECE_Spaceship::checkCollisionWithSpider(ECE_Spider& spider) {
	if (checkCollisionWithOtherSprite(spider)) {
		hurt();
		return true;
	}
	return false;
}

bool ECE_Spaceship::checkCollisionWithOtherSprite(sf::Sprite thatSprite) const {
	return getGlobalBounds().intersects(thatSprite.getGlobalBounds());
}

// Hurt due to collision with centipede or spider
void ECE_Spaceship::hurt() {
	if (health > 0) health--;
}

// Check whether spaceship dies or not
bool ECE_Spaceship::isDead() const {
	return health == 0;
}

// Get health value to draw the spaceship health
const int ECE_Spaceship::getHealth() const {
	return health;
}

// Reset health to SPACESHIP_HEALTH
void ECE_Spaceship::revise() {
	health = SPACESHIP_HEALTH;
	canFire = true;
}