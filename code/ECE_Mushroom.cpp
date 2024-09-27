/*
Author: Rui Wang
Class: ECE6122
Last Date Modified: Sep 26 2024
Description:
ECE Mushroom source file
*/

#include "ECE_Mushroom.h"

// Define the static texture members
sf::Texture ECE_Mushroom::mushroom_texture;
sf::Texture ECE_Mushroom::mushroom_damaged_texture;

// Constructor with position
ECE_Mushroom::ECE_Mushroom(float x, float y) {
    setTexture(mushroom_texture);
    setPosition(x, y);
}

// Hit by laser blaster and reduce health by 1
void ECE_Mushroom::hitByLaserBlaster() {
	if (health > 0) health--;
	if (health == 1) setTexture(mushroom_damaged_texture);
}

// Hit by Spider and directly reduce health to 0
void ECE_Mushroom::hitBySpider() {
	health = 0;
}

// Check whether mushroom is dead or not (if true, delete that mushroom from mushroom list)
bool ECE_Mushroom::isDead() const {
	return health == 0;
}
