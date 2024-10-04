/*
Author: Rui Wang
Class: ECE6122
Last Date Modified: Oct 4th 2024
Description:
Apply the principles of multithreading using std::thread and OpenMP for parallel
calculations in a computationally-intensive problem related playing John Conway¡¯s Game of Life.
*/

#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <SFML/Graphics.hpp>	// SFML
#include <omp.h>				// OpenMP 

#define DEFAULT_NUM_OF_THREAD 8
#define DEFAULT_WINDOW_WEIGHT 800
#define DEFAULT_WINDOW_HEIGHT 600
#define DEFAULT_CELL_SIZE 5
#define RAND_SEED 42

enum class ExecutionType
{
	SEQ = 0,
	THRD = 1,
	OMP = 2
};

typedef struct {
	bool pastIsLive;
	bool nowIsLive;
} Grid;

int numOfThread = DEFAULT_NUM_OF_THREAD;
int windowWeight = DEFAULT_WINDOW_WEIGHT;
int windowHeight = DEFAULT_WINDOW_HEIGHT;
int cellSize = DEFAULT_CELL_SIZE;
ExecutionType executionType = ExecutionType::THRD;

std::vector<std::vector<Grid>> twoDimenGrid;
std::vector<std::vector<sf::RectangleShape>>  twoDimenSquare;

void updateSingleGrid(Grid& grid, int column, int row, int columnLength, int rowLength) {
	int numOfLiveNeighbour = 0;

	if (column - 1 >= 0 && row - 1 >= 0 && twoDimenGrid[column - 1][row - 1].pastIsLive) {
		numOfLiveNeighbour++;		// left up corner exists and is live
	}
	if (column - 1 >= 0 && row + 1 < rowLength && twoDimenGrid[column - 1][row + 1].pastIsLive) {
		numOfLiveNeighbour++;		// left down corner exists and is live
	}
	if (column + 1 < columnLength && row - 1 >= 0 && twoDimenGrid[column + 1][row - 1].pastIsLive) {
		numOfLiveNeighbour++;		// right up corner exists and is live
	}
	if (column + 1 < columnLength && row + 1 < rowLength && twoDimenGrid[column + 1][row + 1].pastIsLive) {
		numOfLiveNeighbour++;		// right down corner exits and is live
	}
	if (column - 1 >= 0 && twoDimenGrid[column - 1][row].pastIsLive) {
		numOfLiveNeighbour++;		// left grid exists and is live
	}
	if (column + 1 < columnLength && twoDimenGrid[column + 1][row].pastIsLive) {
		numOfLiveNeighbour++;		// right grid exits and is live
	}
	if (row - 1 >= 0 && twoDimenGrid[column][row - 1].pastIsLive) {
		numOfLiveNeighbour++;		// up grid exits and is live
	}
	if (row + 1 < rowLength && twoDimenGrid[column][row + 1].pastIsLive) {
		numOfLiveNeighbour++;		// down grid exits and is live
	}

	// if it is live and number of neighbours is not 2 or 3, then change to dead
	if (grid.pastIsLive && numOfLiveNeighbour != 2 && numOfLiveNeighbour != 3) {
		grid.nowIsLive = false;
	}
	// if it is dead and number of neighbours is 3, then change to live
	if (!grid.pastIsLive && numOfLiveNeighbour == 3) {
		grid.nowIsLive = true;
	}
}

void sequentiallyGridUpdate(int columnLength, int rowLength) {
	// Update the grid Sequentially
	for (int i = 0; i < columnLength; i++) {
		for (int j = 0; j < rowLength; j++) {
			updateSingleGrid(twoDimenGrid[i][j], i, j, columnLength, rowLength);
		}
	}
}

void threadUpdateGridWithChunk(int start, int end, int columnLength, int rowLength) {
	// Update the grid by chunk
	for (int i = start; i < end; i++) {
		for (int j = 0; j < rowLength; j++) {
			updateSingleGrid(twoDimenGrid[i][j], i, j, columnLength, rowLength);
		}
	}
}

void threadGridUpdate(int columnLength, int rowLength) {
	// Create thread pool
	int chunkSize = columnLength / numOfThread;
	std::vector<std::thread> threadPool;

	// Divide the work into chunks and assign each chunk to a thread
	for (int i = 0; i < numOfThread; i++) {
		int start = i * chunkSize;
		int end = (i == numOfThread - 1) ? columnLength : start + chunkSize;
		threadPool.push_back(std::thread(threadUpdateGridWithChunk, start, end, columnLength, rowLength));
	}

	// Join threads
	for (auto& thread : threadPool) {
		thread.join();
	}
}

void openMPGridUpdate(int columnLength, int rowLength) {
	// OpenMP parallel for loop
#pragma omp parallel num_threads(numOfThread) 
	{
#pragma omp for
		for (int i = 0; i < columnLength; i++) {
			for (int j = 0; j < rowLength; j++) {
				updateSingleGrid(twoDimenGrid[i][j], i, j, columnLength, rowLength);
			}
		}
	}
}

int main(int argc, char** args)
{
	// Obtain through arguments
	for (int i = 1; i < argc; i += 2) {
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
				numOfThread = DEFAULT_NUM_OF_THREAD;
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
				cellSize = DEFAULT_CELL_SIZE;
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
				windowWeight = DEFAULT_WINDOW_WEIGHT;
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
				windowHeight = DEFAULT_WINDOW_HEIGHT;
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
				executionType = ExecutionType::THRD;
			}
		}
	}
	std::cout << "numOfThread: " << numOfThread << " cellSize: " << cellSize << " windowWeight: "
		<< windowWeight << " windowHeight: " << windowHeight << " execuationType: "
		<< (executionType == ExecutionType::SEQ ? "SEQ" : (executionType == ExecutionType::THRD ? "THRD" : "OMP"))
		<< std::endl;

	// Create a window
	sf::RenderWindow window(sf::VideoMode(windowWeight, windowHeight), "John Conway\'s Game of Life");

	// Srand initialization
	std::srand(RAND_SEED);			// std::srand(static_cast<unsigned>(std::time(0)));
	for (int i = 0; i < windowWeight; i += cellSize) {
		std::vector<Grid> gridColumn;
		std::vector<sf::RectangleShape> squareColumn;
		for (int j = 0; j < windowHeight; j += cellSize) {
			Grid newGrid;
			newGrid.nowIsLive = newGrid.pastIsLive = rand() % 2;
			gridColumn.push_back(newGrid);

			sf::RectangleShape square(sf::Vector2f(static_cast<float>(cellSize), static_cast<float>(cellSize)));
			square.setPosition(static_cast<float>(i), static_cast<float>(j));
			square.setFillColor(sf::Color::White);
			squareColumn.push_back(square);
		}
		twoDimenGrid.push_back(gridColumn);
		twoDimenSquare.push_back(squareColumn);
	}

	// Grid size
	int columnLength = windowWeight / cellSize;
	int rowLength = windowHeight / cellSize;
	// Count the generation iteration and time
	int count = 0;
	std::chrono::time_point<std::chrono::high_resolution_clock> start;
	std::chrono::time_point<std::chrono::high_resolution_clock> end;
	long long duration = 0;

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

		// Update newGrid with different types
		start = std::chrono::high_resolution_clock::now();
		if (executionType == ExecutionType::SEQ) {
			sequentiallyGridUpdate(columnLength, rowLength);		// Sequentially update grid
		}
		else if (executionType == ExecutionType::THRD) {
			threadGridUpdate(columnLength, rowLength);				// Thread update grid
		}
		else {
			openMPGridUpdate(columnLength, rowLength);				// OpenMP update grid
		}
		// Record acculumated time
		end = std::chrono::high_resolution_clock::now();
		duration += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

		// Update grid after a generation
		for (auto& col : twoDimenGrid) {
			for (auto& e : col) {
				e.pastIsLive = e.nowIsLive;
			}
		}

		// One generation finishes
		count++;
		// Output the execution time
		if (count % 100 == 0) {
			if (executionType == ExecutionType::SEQ) {
				std::cout << "(Total generations: " << count << ") 100 generations took " << duration
					<< " microseconds with single thread" << std::endl;
			}
			else if (executionType == ExecutionType::THRD) {
				std::cout << "(Total generations: " << count << ") 100 generations took " << duration
					<< " microseconds with " << numOfThread << " std::threads" << std::endl;
			}
			else {
				std::cout << "(Total generations: " << count << ") 100 generations took "
					<< duration << " microseconds with " << numOfThread << " OMP threads" << std::endl;
			}
			// Clear up duration for next 100 generations
			duration = 0;
		}

		// Draw the graph
		window.clear(sf::Color::Black);
		// draw
		for (int i = 0; i < columnLength; i++) {
			for (int j = 0; j < rowLength; j++) {
				if (twoDimenGrid[i][j].nowIsLive) {
					window.draw(twoDimenSquare[i][j]);
				}
			}
		}
		window.display();
	}

	return 0;
}
