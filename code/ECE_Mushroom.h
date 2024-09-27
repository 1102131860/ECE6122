/*
Author: Rui Wang
Class: ECE6122
Last Date Modified: Sep 26 2024
Description:
ECE Mushroom head file
*/

#pragma once
#include "Config.h"
#include <algorithm> // For min()

class ECE_Mushroom : public sf::Sprite
{
public:
	static sf::Texture mushroom_texture;
	static sf::Texture mushroom_damaged_texture;
	
	// Constructor with position
	ECE_Mushroom(float x, float y);

	// Hit by laser blaster and reduce health by 1
	void hitByLaserBlaster();

	// Hit by Spider and directly reduce health to 0
	void hitBySpider();

	// Check whether mushroom is dead or not (if true, delete that mushroom from mushroom list)
	bool isDead() const;

private:
	int health = MUSHROOM_HEALTH;
};