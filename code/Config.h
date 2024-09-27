/*
Author: Rui Wang
Class: ECE6122
Last Date Modified: Sep 26 2024
Description:
Global configuration files
*/

#pragma once
#include <string>
#include <SFML/Graphics.hpp>

// Textures address
const std::string CENTIPEDE_BODY_TEXTURE = "graphics/CentipedeBody.png";
const std::string CENTIPEDE_HEAD_TEXTURE = "graphics/CentipedeHead.png";
const std::string MUSHROOM_TEXTURE = "graphics/Mushroom0.png";
const std::string MUSHROOM_DAMAGED_TEXTURE = "graphics/Mushroom1.png";
const std::string SPIDER_TEXTURE = "graphics/spider.png";
const std::string SPACESHIP_TEXTURE = "graphics/StarShip.png";
const std::string STARTUP_SCREEN_TEXTURE = "graphics/Startup Screen BackGround.png";
// LaserBlast configuration (RectangleShape)
const sf::Vector2f LASERBLAST_SIZE = { 5.0f, 15.0f };	// no laser texture in the given source
const sf::Color	LASERBLAST_COLOR = sf::Color::Red;
// Front address
const std::string FONT = "fonts/KOMIKAP_.ttf";

// Window size and game areas
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int INFO_AREA_HEIGHT = 70;
const int MUSHROOM_FREE_AREA_HEIGHT = 50;
const int SCORE_SIZE = 30;

// Segments of centipede
const int SEGMENTS_OF_CENTIPEDE = 12;
// Initial number of mushroom
const int INITIAL_NUMBER_OF_MUSHROOM = 30;

// Initial positions
const int SPACESHIP_INITIAL_POSITION_X = WINDOW_WIDTH / 2;
const int SPACESHIP_INITIAL_POSTIION_Y = WINDOW_HEIGHT - MUSHROOM_FREE_AREA_HEIGHT;
const int CENTIPEDE_INITIAL_POSITION_X = WINDOW_WIDTH - SEGMENTS_OF_CENTIPEDE * 23;
const int CENTIPECE_INITIAL_POSITION_Y = INFO_AREA_HEIGHT;
const int SPIDER_INITIAL_POSITION_X = WINDOW_WIDTH / 2;
const int SPIDER_INITIAL_POSITION_Y = WINDOW_HEIGHT / 2;
const int SPACESHIP_HEALTH_POSITION_X = 3 * WINDOW_WIDTH / 4;
const int SPACESHIP_HEALTH_POSITION_Y = INFO_AREA_HEIGHT / 4;
const int SCORE_POSITION_X = WINDOW_WIDTH / 2;
const int SCORE_POSITION_Y = INFO_AREA_HEIGHT / 10;

// Health
const int MUSHROOM_HEALTH = 2;
const int SPACESHIP_HEALTH = 3;
const int SPIDER_HEALTH = 1;

// Moving Speed
const float CENTIPEDE_SPEED = 100.0f;
const float LASERBLAST_SPEED = 400.0f;
const float SPACESHIP_SPEED = 200.0f;
const float SPIDER_SPEED = 50.0f;

// Score 
const int HIT_CENTIPEDE_BODY_SCORE = 10;
const int HIT_CENTIPEDE_HEAD_SCORE = 100;
const int HIT_SPIDER_SCORE = 300;
const int HIT_MUSHROOM_SCORE = 4;

// Following Distance Threshold
const int FOLLOW_DISTANCE_THRESHOLD = 22;
