/*
Author: Rui Wang
Class: ECE6122
Last Date Modified: Sep 26 2024
Description:
The main function works here
*/

#include <iostream>
#include "ECE_Centipede.h"

enum GameStatus
{
	startGame,
	restoreGame,
	endGame
};

int score = 0;
GameStatus gameStatus = GameStatus::endGame;
std::list<ECE_Mushroom> mushrooms;
std::list<ECE_Centipede> centipedes;
std::list<ECE_LaserBlast> laserBlasts;

int main()
{
	// Create a window
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Retro Centipede Arcade Game");

	// Load an image into a texture
	sf::Texture startUpScreenTexture;
	sf::Font font;
	if (!startUpScreenTexture.loadFromFile(STARTUP_SCREEN_TEXTURE) ||
		!ECE_Centipede::centipedeHeadTexture.loadFromFile(CENTIPEDE_HEAD_TEXTURE) ||
		!ECE_Centipede::centipedeBodyTexture.loadFromFile(CENTIPEDE_BODY_TEXTURE) ||
		!ECE_Mushroom::mushroom_texture.loadFromFile(MUSHROOM_TEXTURE) ||
		!ECE_Mushroom::mushroom_damaged_texture.loadFromFile(MUSHROOM_DAMAGED_TEXTURE) ||
		!ECE_Spaceship::spaceship_texture.loadFromFile(SPACESHIP_TEXTURE) ||
		!ECE_Spider::spider_texture.loadFromFile(SPIDER_TEXTURE) ||
		!font.loadFromFile(FONT)) {
		return -1;
	}

	// Create a sprite and set the texture
	sf::Sprite startUpScreenSprite;
	startUpScreenSprite.setTexture(startUpScreenTexture);
	startUpScreenSprite.setPosition(0, 0);

	// Create a scoretext and set the texture
	sf::Text scoreText;
	scoreText.setFont(font);
	scoreText.setString(std::to_string(score));
	scoreText.setCharacterSize(SCORE_SIZE);
	scoreText.setFillColor(sf::Color::White);
	scoreText.setPosition(SCORE_POSITION_X, SCORE_POSITION_Y);

	// Create one spider
	ECE_Spider spider(SPIDER_INITIAL_POSITION_X, SPIDER_INITIAL_POSITION_Y);

	// Create one spaceship
	ECE_Spaceship spaceship(SPACESHIP_INITIAL_POSITION_X, SPACESHIP_INITIAL_POSTIION_Y);

	// Create a clock to track time between frames
	sf::Clock clock;

	// Main loop
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}

		/*//////////////////////////////////////////////////////////////////////*/
		/*								END GAME								*/
		/*//////////////////////////////////////////////////////////////////////*/
		if (gameStatus == GameStatus::endGame) {
			window.draw(startUpScreenSprite);
			window.display();

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
				// Start scaore
				score = 0;

				// Start Mushroom
				mushrooms.clear();
				for (int i = 0; i < INITIAL_NUMBER_OF_MUSHROOM; i++) {
					ECE_Mushroom newmushroom(static_cast<float>(rand() % WINDOW_WIDTH), static_cast<float>(INFO_AREA_HEIGHT + rand() % (WINDOW_HEIGHT - INFO_AREA_HEIGHT - MUSHROOM_FREE_AREA_HEIGHT)));
					bool mushroomCollidision = false;
					for (auto& mushroom : mushrooms) {
						if (newmushroom.getGlobalBounds().intersects(mushroom.getGlobalBounds())) {
							mushroomCollidision = true;
							i--;
						}
					}
					if (!mushroomCollidision)
						mushrooms.push_back(newmushroom);
				}

				// Start Spider
				spider.revise();
				spider.setPosition(SPIDER_INITIAL_POSITION_X, SPIDER_INITIAL_POSITION_Y);

				// Start laserBlasts
				laserBlasts.clear();

				// Start spaceship
				spaceship.revise();
				spaceship.setPosition(SPACESHIP_INITIAL_POSITION_X, SPACESHIP_INITIAL_POSTIION_Y);

				// Start Centipede
				centipedes.clear();
				centipedes.emplace_back(SEGMENTS_OF_CENTIPEDE, CENTIPEDE_INITIAL_POSITION_X, CENTIPECE_INITIAL_POSITION_Y);

				// Shift to start game
				gameStatus = GameStatus::startGame;
			}
		}

		/*//////////////////////////////////////////////////////////////////////*/
		/*								RESTORE GAME							*/
		/*//////////////////////////////////////////////////////////////////////*/
		if (gameStatus == GameStatus::restoreGame) {
			// Restore laserBlasts
			laserBlasts.clear();

			// Restore Spaceship without revising it
			spaceship.setPosition(SPACESHIP_INITIAL_POSITION_X, SPACESHIP_INITIAL_POSTIION_Y);

			// Shift to start game
			gameStatus = GameStatus::startGame;
		}

		/*//////////////////////////////////////////////////////////////////////*/
		/*								START GAME								*/
		/*//////////////////////////////////////////////////////////////////////*/
		if (gameStatus == GameStatus::startGame) {
			// Calculate the time elapsed since the last frame
			sf::Time elapsed = clock.restart();  // Reset clock and get elapsed time
			float deltaTime = elapsed.asSeconds();  // Convert to seconds for movement calculations

			/*********************************************************************
			*							Spider									 *
			**********************************************************************/
			if (!spider.isDead()) {
				// move
				spider.move(deltaTime);

				// check whether collides with mushroom and destory it
				spider.checkCollisionWithMushrooms(mushrooms);
			}

			/*********************************************************************
			*							Spaceship								 *
			**********************************************************************/
			// move
			spaceship.move(deltaTime, mushrooms);

			// generate laserblasts
			spaceship.updateFireStatus();
			spaceship.fireBlast(laserBlasts);

			// check collision with Spider
			if (!spider.isDead()) {
				if (spaceship.checkCollisionWithSpider(spider)) {
					if (spaceship.isDead()) {
						std::cout << "Spaceship is dead!\nYou losed!\nYou got " << score << " points" << std::endl;
						gameStatus = GameStatus::endGame;
					}
					else {
						std::cout << "Hurt by Spider one time!\nYou still have " << spaceship.getHealth() << " health" << std::endl;
						gameStatus = GameStatus::restoreGame;
					}
				}
			}

			/*********************************************************************
			*							Laserblast								 *
			**********************************************************************/
			// move
			for (auto& laserBlast : laserBlasts) {
				laserBlast.move(deltaTime);
			}

			// remove laser blasts out of winodw
			for (auto laserBlastIt = laserBlasts.begin(); laserBlastIt != laserBlasts.end();) {
				if (laserBlastIt->isOutOfWindow()) {
					laserBlastIt = laserBlasts.erase(laserBlastIt);
				}
				else {
					++laserBlastIt;
				}
			}

			// Check collision with mushroom, remove laserBlast from its list and get bonus score
			for (auto laserBlastIt = laserBlasts.begin(); laserBlastIt != laserBlasts.end();) {
				if (laserBlastIt->checkCollisionWithMushrooms(mushrooms)) {		// this function will change mushroom
					laserBlastIt = laserBlasts.erase(laserBlastIt);
					score += HIT_MUSHROOM_SCORE;
				}
				else {
					++laserBlastIt;
				}
			}

			// Check collision with spider, remove laserBlast from its list, and get bonus score
			if (!spider.isDead()) {
				for (auto laserBlastIt = laserBlasts.begin(); laserBlastIt != laserBlasts.end();) {
					if (laserBlastIt->checkCollisionWithSpider(spider)) {
						laserBlastIt = laserBlasts.erase(laserBlastIt);
						score += HIT_SPIDER_SCORE;
						break;
					}
					else {
						++laserBlastIt;
					}
				}
			}
			
			/*********************************************************************
			*							Centipede								 *
			**********************************************************************/
			// move
			for (auto& centipede : centipedes) {
				centipede.moveWithCheckingCollisionWithMushrooms(deltaTime, mushrooms);
			}

			// check collision with laserblasts and increase scores
			for (auto centipedeIt = centipedes.begin(); centipedeIt != centipedes.end();) {
				CentipedeHitStatus hitByLaserBlaster = centipedeIt->checkCollisionWithLaserBlasts(laserBlasts, mushrooms, centipedes);
				if (hitByLaserBlaster != CentipedeHitStatus::NotHit)
					score += hitByLaserBlaster == CentipedeHitStatus::HitBody ? HIT_CENTIPEDE_BODY_SCORE : HIT_CENTIPEDE_HEAD_SCORE;
				// remove those dead centipedes
				if (!centipedeIt->isDead()) {
					++centipedeIt;
				}
				else {	// remove dead centipede from centipede lists
					centipedeIt = centipedes.erase(centipedeIt);
				}
			}
			if (centipedes.empty()) {
				std::cout << "All centipedes are dead \nYou Win!\nYou got " << score << " points" << std::endl;
				gameStatus = GameStatus::endGame;
			}

			// check collision with spaceship
			for (auto& centipede : centipedes) {
				if (centipede.checkCollisionWithSpaceship(spaceship)) {
					if (spaceship.isDead()) {
						std::cout << "Spaceship died\nYou lose!\nYou got " << score << " points" << std::endl;
						gameStatus = GameStatus::endGame;
					}
					else {
						std::cout << "Hurt by centipede one time!\nYou still have " << spaceship.getHealth() << " health" << std::endl;
						gameStatus = GameStatus::restoreGame;
					}
				}
			}

			/*********************************************************************
			*							Draw									 *
			**********************************************************************/
			window.clear(sf::Color::Black);

			// Draw the score
			scoreText.setString(std::to_string(score));
			window.draw(scoreText);

			// Draw spaceship health
			for (int i = 0; i < spaceship.getHealth(); i++) {
				sf::Sprite spaceshipSprite;
				spaceshipSprite.setTexture(ECE_Spaceship::spaceship_texture);
				spaceshipSprite.setPosition(SPACESHIP_HEALTH_POSITION_X + i * spaceshipSprite.getLocalBounds().width, SPACESHIP_HEALTH_POSITION_Y);
				window.draw(spaceshipSprite);
			}

			// Draw all the mushrooms
			for (auto& mushroom : mushrooms) {
				window.draw(mushroom);
			}

			// Draw the spider
			if (!spider.isDead()) {
				window.draw(spider);
			}

			// Draw the spaceship
			window.draw(spaceship);

			// Draw all the laserBlasts
			for (auto& laserBlast : laserBlasts) {
				window.draw(laserBlast.getShape());
			}

			// Draw all the centipedes
			for (auto& centipede : centipedes) {
				centipede.draw(window);
			}
			
			// Display
			window.display();
		}
	}

	return 0;
}
