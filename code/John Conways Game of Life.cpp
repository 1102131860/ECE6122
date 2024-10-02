/*
Author: Rui Wang
Class: ECE6122
Last Date Modified: Oct 1st 2024
Description:
Apply the principles of multithreading using std::thread and OpenMP for parallel
calculations in a computationally-intensive problem related playing John Conway¡¯s Game of Life.
*/

#include <iostream>
#include <string>
#include <vector>
#include <ctime>    // For time()
#include <cstdlib>  // For rand() and srand()
#include <chrono>	// For time functions
#include <thread>	// For thread and thread pool
#include <SFML/Graphics.hpp>
#include <omp.h>	// OpenMP 

enum class ExecutionType
{
	SEQ = 0,
	THRD = 1,
	OMP = 2
};

int numOfThread = 8;
int windowWeight = 800;
int windowHeight = 600;
int cellSize = 5;
ExecutionType executionType = ExecutionType::THRD;

std::vector<std::vector<sf::RectangleShape>>  twoDimensionalGrid;
std::vector<std::vector<sf::RectangleShape>>  newGrid;

void updateGrid(sf::RectangleShape& grid) {
	int numoOfLiveNeighbour = 0;
	int columnLength = windowWeight / cellSize;
	int rowLength = windowHeight / cellSize;
	int column = static_cast<int>(grid.getPosition().x) / cellSize;
	int row = static_cast<int>(grid.getPosition().y) / cellSize;

	if (column - 1 >= 0 && row - 1 >= 0 && twoDimensionalGrid[column - 1][row - 1].getFillColor() == sf::Color::White) {
		numoOfLiveNeighbour++;		// left up corner exists and is live
	}
	if (column - 1 >= 0 && row + 1 < rowLength && twoDimensionalGrid[column - 1][row + 1].getFillColor() == sf::Color::White) {
		numoOfLiveNeighbour++;		// left down corner exists and is live
	}
	if (column + 1 < columnLength && row - 1 >= 0 && twoDimensionalGrid[column + 1][row - 1].getFillColor() == sf::Color::White) {
		numoOfLiveNeighbour++;		// right up corner exists and is live
	}
	if (column + 1 < columnLength && row + 1 < rowLength && twoDimensionalGrid[column + 1][row + 1].getFillColor() == sf::Color::White) {
		numoOfLiveNeighbour++;		// right down corner exits and is live
	}
	if (column - 1 >= 0 && twoDimensionalGrid[column - 1][row].getFillColor() == sf::Color::White) {
		numoOfLiveNeighbour++;		// left grid exists and is live
	}
	if (column + 1 < columnLength && twoDimensionalGrid[column + 1][row].getFillColor() == sf::Color::White) {
		numoOfLiveNeighbour++;		// right grid exits and is live
	}
	if (row - 1 >= 0 && twoDimensionalGrid[column][row - 1].getFillColor() == sf::Color::White) {
		numoOfLiveNeighbour++;		// up grid exits and is live
	}
	if (row + 1 < rowLength && twoDimensionalGrid[column][row + 1].getFillColor() == sf::Color::White) {
		numoOfLiveNeighbour++;		// down grid exits and is live
	}
	
	// if it is live and number of neighbours is not 2 or 3, then change to dead
	if (grid.getFillColor() == sf::Color::White && numoOfLiveNeighbour != 2 && numoOfLiveNeighbour != 3) {
		grid.setFillColor(sf::Color::Black);
	}
	// if it is dead and number of neighbours is 3, then change to live
	if (grid.getFillColor() == sf::Color::Black && numoOfLiveNeighbour == 3) {
		grid.setFillColor(sf::Color::White);
	}
}

void sequentiallyGridUpdate() {
	// Update the new Grid accodring to the original twoDimensionalGrid
	for (auto& column : newGrid) {
		for (auto& grid : column) {
			updateGrid(grid);
		}
	}
}

void threadGridUpdateWithChunk(int start, int end) {
	int rowLength = windowHeight / cellSize;
	for (int i = start; i < end; i++) {
		for (int j = 0; j < rowLength; j++) {
			updateGrid(newGrid[i][j]);
		}
	}
}

void threadGridUpdate() {
	// Create thread pool
	std::vector<std::thread> threadPool;
	int columnLength = windowWeight / cellSize;
	int chunkSize = columnLength / numOfThread;

	// Divide the work into chunks and assign each chunk to a thread
	for (int i = 0; i < numOfThread; i++) {
		int start = i * chunkSize;
		int end = (i == numOfThread - 1) ? columnLength : start + chunkSize;	// Handle last chunk
		threadPool.push_back(std::thread(threadGridUpdateWithChunk, start, end));
	}

	// Join threads
	for (auto& thread : threadPool) {
		thread.join();
	}
}

void openMPGridUpdate() {
	int columnLength = windowWeight / cellSize;
	int rowLength = windowHeight / cellSize;
	#pragma omp parallel num_threads(numOfThread)
	{
		#pragma omp for
		for (int i = 0; i < columnLength; i++) {
			for (int j = 0; j < rowLength; j++) {
				updateGrid(newGrid[i][j]);
			}
		}
	}
}

int main(int argc, char** args)
{
	// Obtain through arguments
	/*
	*	need argument validation checking
	*/
	for (int i = 1; i < argc; i+=2) {
		std::string argKey = args[i];
		std::string argValue = (i + 1 < argc) ? args[i + 1] : "";

		if (argKey == "-n" && !argValue.empty()) {
			try {
				numOfThread = std::stoi(argValue);
			}
			catch (const std::invalid_argument& e) {
				std::cout << "Invalid argument (-n): " << e.what() << std::endl;
			}
			catch (const std::out_of_range& e) {
				std::cout << "Out of range (-n): " << e.what() << std::endl;
			}
			if (numOfThread < 2) {
				std::cout << "The number of threads must be larger than 2" << std::endl;
				numOfThread = 8;
			}
		}
		else if (argKey == "-c" && !argValue.empty()) {
			try {
				cellSize = std::stoi(argValue);
			}
			catch (const std::invalid_argument& e) {
				std::cout << "Invalid argument (-c): " << e.what() << std::endl;
			}
			catch (const std::out_of_range& e) {
				std::cout << "Out of range (-c): " << e.what() << std::endl;
			}
			if (cellSize < 1) {
				std::cout << "Cell size must be larger than or equal to 1" << std::endl;
				cellSize = 5;
			}
		}
		else if (argKey == "-x" && !argValue.empty()) {
			try {
				windowWeight = std::stoi(argValue);
			}
			catch (const std::invalid_argument& e) {
				std::cout << "Invalid argument (-x): " << e.what() << std::endl;
			}
			catch (const std::out_of_range& e) {
				std::cout << "Out of range (-x): " << e.what() << std::endl;
			}
			if (windowWeight < 1) {
				std::cout << "Winodw weight mush be larger than or equal to 1" << std::endl;
				windowWeight = 800;
			}
		}
		else if (argKey == "-y" && !argValue.empty()) {
			try {
				windowHeight = std::stoi(argValue);
			}
			catch (const std::invalid_argument& e) {
				std::cout << "Invalid argument (-y): " << e.what() << std::endl;
			}
			catch (const std::out_of_range& e) {
				std::cout << "Out of range (-y): " << e.what() << std::endl;
			}
			if (windowHeight < 1) {
				std::cout << "Winodw height mush be larger than or equal to 1" << std::endl;
				windowHeight = 600;
			}
		}
		else if (argKey == "-t" && !argValue.empty()) {
			if (argValue == "SEQ") {
				executionType = ExecutionType::SEQ;
			}
			else if (argValue == "THRD") {
				executionType = ExecutionType::THRD;
			}
			else if (argValue == "OMP") {
				executionType = ExecutionType::OMP;
			}
			else {
				std::cout << "Invalid argument (-t): select a type from SEQ, THRD and OMP" << std::endl;
			}
		}
	}
	std::cout << "numOfThread: " << numOfThread << " cellSize: " << cellSize << " windowWeight: " << windowWeight << " windowHeight: " << windowHeight 
		<< " execuationType: "  << (executionType == ExecutionType::SEQ ? "SEQ" : (executionType == ExecutionType::THRD ? "THRD" : "OMP")) << std::endl;

	// Create a window
	sf::RenderWindow window(sf::VideoMode(windowWeight, windowHeight), "John Conway¡¯s Game of Life");

	// Create a sprite and set the texture
	sf::Sprite sprite;

	// Set the position of the sprite to the target rectangle's position
	sprite.setPosition(0, 0);

	// Srand initialization
	std::srand(42);			// std::srand(static_cast<unsigned>(std::time(0)));
	for (int i = 0; i < windowWeight; i += cellSize) {
		std::vector<sf::RectangleShape> columns;
		for (int j = 0; j < windowHeight; j += cellSize) {
			sf::RectangleShape grid(sf::Vector2f(static_cast<float>(cellSize), static_cast<float>(cellSize)));
			grid.setPosition(static_cast<float>(i), static_cast<float>(j));
			grid.setFillColor(rand() % 2 ? sf::Color::White : sf::Color::Black);
			columns.push_back(grid);
		}
		twoDimensionalGrid.push_back(columns);
	}
	// Copy newGrid from twoDimensionalGrid
	newGrid = twoDimensionalGrid;

	// Count the generation iteration and time
	int count = 0;
	std::chrono::time_point<std::chrono::high_resolution_clock> start;
	std::chrono::time_point<std::chrono::high_resolution_clock> end;

	// Main loop
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}
		// If press esc, window exits
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
			window.close();
		}

		// Start counting time
		if (count % 100 == 0) {
			start = std::chrono::high_resolution_clock::now();
		}

		// Update newGrid with different types
		if (executionType == ExecutionType::SEQ) {
			sequentiallyGridUpdate();		// Sequentially update grid
		}
		else if (executionType == ExecutionType::THRD) {
			threadGridUpdate();				// Thread update grid
		}
		else {
			openMPGridUpdate();				// OpenMP update grid
		}
		// Update two DimensionalGrid after a generation
		twoDimensionalGrid = newGrid;

		// One generation finishes
		count++;
	    // Output the execution time
		if (count % 100 == 0) {
			// Record the end time
			end = std::chrono::high_resolution_clock::now();
			// Calculate the duration in microseconds
			auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
			if (executionType == ExecutionType::SEQ) {
				std::cout << "(Total generations: " << count << ") 100 generations took " << duration << " microseconds with single thread" << std::endl;
			}
			else if (executionType == ExecutionType::THRD) {
				std::cout << "(Total generations: " << count << ") 100 generations took " << duration << " microseconds with " << numOfThread << " std::threads" << std::endl;
			}
			else {
				std::cout << "(Total generations: " << count << ") 100 generations took " << duration << " microseconds with " << numOfThread << " OMP threads" << std::endl;
			}
		}

		// Draw the graph
		window.clear(sf::Color::Black);
		window.draw(sprite);
		// draw
		for (auto& column : twoDimensionalGrid) {
			for (auto& grid : column) {
				if (grid.getFillColor() == sf::Color::White) {
					window.draw(grid);
				}
			}
		}
		window.display();
	}

	return 0;
}
