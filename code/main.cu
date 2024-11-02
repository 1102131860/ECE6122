// main.cpp
#include <SFML/Graphics.hpp>
#include <cuda_runtime.h>
#include "cuda_kernels.cuh"
#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <chrono>
#include <cstdlib>

#define DEFAULT_NUM_THREAD_PER_BLOCK 32
#define DEFAULT_CELL_SIZE 5
#define DEFAUlT_WINDOW_WEIGHT 800
#define DEFAULT_WINDOW_HEIGHT 600
#define RAND_SEED 42
#define RADIUS 1

enum class MemoryType {
	NORMAL = 0,
	PINNED = 1,
	MANAGED = 2
};

int numThreadPerBlock, cellSize, windowWeight, windowHeight;
MemoryType memoryType;

int columnLength, rowLength, totalGrid;
std::vector<int> hostGrid;
int* d_grid_current, d_grid_next;

// Obtain arguments through inputs
void argumentsParse(int argc, char* argv[]) {
    for (int i = 1; i < argc; i += 2) {
        std::string argKey = argv[i];
        std::string argValue = (i + 1 < argc) ? argv[i + 1] : "";

        if (argKey == "-n" && !argValue.empty()) {
            try {
                numThreadPerBlock = std::stoi(argValue);
            } catch (const std::invalid_argument& e) {
                std::cout << "Invalid argument (-n): " << e.what() << std::endl;
            } catch (const std::out_of_range& e) {
                std::cout << "Out of range (-n): " << e.what() << std::endl;
            }
            if (numThreadPerBlock <= 0 || numThreadPerBlock % 32 != 0) {
                std::cout << "The number of threads per block must be a multiple of 32" << std::endl;
                numThreadPerBlock = DEFAULT_NUM_THREAD_PER_BLOCK;
            }
        } else if (argKey == "-c" && !argValue.empty()) {
            try {
	            cellSize = std::stoi(argValue);
            } catch (const std::invalid_argument& e) {
                std::cout << "Invalid argument (-c): " << e.what() << std::endl;
            } catch (const std::out_of_range& e) {
	            std::cout << "Out of range (-c): " << e.what() << std::endl;
            }
            if (cellSize < 1) {
                std::cout << "Cell size must be larger than or equal to 1" << std::endl;
                cellSize = DEFAULT_CELL_SIZE;
            }
        } else if (argKey == "-x" && !argValue.empty()) {
            try {
                windowWeight = std::stoi(argValue);
            } catch (const std::invalid_argument& e) {
                std::cout << "Invalid argument (-x): " << e.what() << std::endl;
            } catch (const std::out_of_range& e) {
                std::cout << "Out of range (-x): " << e.what() << std::endl;
            } if (windowWeight < cellSize) {
                std::cout << "Winodw weight mush be larger than or equal to cell size" << std::endl;
                windowWeight = DEFAUlT_WINDOW_WEIGHT;
            }
        } else if (argKey == "-y" && !argValue.empty()) {
            try {
                windowHeight = std::stoi(argValue);
            } catch (const std::invalid_argument& e) {
                std::cout << "Invalid argument (-y): " << e.what() << std::endl;
            } catch (const std::out_of_range& e) {
                std::cout << "Out of range (-y): " << e.what() << std::endl;
            }
            if (windowHeight < cellSize) {
                std::cout << "Winodw height mush be larger than or equal to cell size" << std::endl;
                windowHeight = DEFAULT_WINDOW_HEIGHT;
            }
        } else if (argKey == "-t" && !argValue.empty()) {
            if (argValue == "NORMAL"){
                memoryType = MemoryType::NORMAL;
            } else if (argValue == "THRD") {
                memoryType = MemoryType::PINNED;
            } else if (argValue == "MANAGED") {
                memoryType == MemoryType::MANAGED;
            } else {
                std::cout << "Invalid argument (-t): select a type from NORMAL, PINNED and MANAGED" << std::endl;
                memoryType = MemoryType::NORMAL;
            }
        }
    }
}

// Update 1-D Grid with Halo padding (share memory) 
__global__ void updateSingleGridKernelWithHalo(int* grid, int* newGrid, int columnLength, int rowLength) {
    // Use extern shared memory for dynamic allocation with halo padding
    extern __shared__ int sharedGrid[];

    // Calculate the dimensions of the block and the shared memory size including halo
    int sharedWidth = blockDim.x + 2 * RADIUS;

    // Calculate global and local indices
    int blockColumn = blockIdx.x * blockDim.x;
    int blockRow = blockIdx.y * blockDim.y;

    int localX = threadIdx.x;
    int localY = threadIdx.y;

    int globalX = blockColumn + localX;
    int globalY = blockRow + localY;

    int sharedX = localX + RADIUS;
    int sharedY = localY + RADIUS;

    // Load the main cell and halo cells into shared memory using a unified loop
    for (int dy = -RADIUS; dy <= RADIUS; dy++) {
        for (int dx = -RADIUS; dx <= RADIUS; dx++) {
            // Calculate the global and shared memory indices for each cell to be loaded
            int haloGlobalX = globalX + dx;
            int haloGlobalY = globalY + dy;
            int haloSharedX = sharedX + dx;
            int haloSharedY = sharedY + dy;

            // Only load the cells if the current thread is responsible for them
            if (localX + dx >= -RADIUS && localX + dx < blockDim.x + RADIUS &&
                localY + dy >= -RADIUS && localY + dy < blockDim.y + RADIUS) {

                // Check if within the bounds of the global grid and load appropriately
                if (haloGlobalX >= 0 && haloGlobalX < columnLength && haloGlobalY >= 0 && haloGlobalY < rowLength) {
                    sharedGrid[haloSharedY * sharedWidth + haloSharedX] = grid[haloGlobalY * columnLength + haloGlobalX];
                } else {
                    sharedGrid[haloSharedY * sharedWidth + haloSharedX] = 0; // Handle out-of-bounds by setting to 0
                }
            }
        }
    }

    // Synchronize to ensure all cells, including halo, are loaded into shared memory
    __syncthreads();

    // Calculate the number of live neighbors
    int numOfLiveNeighbours = 0;

    // Count neighbors
    for (int dy = -RADIUS; dy <= RADIUS; dy++) {
        for (int dx = -RADIUS; dx <= RADIUS; dx++) {
            if (dx == 0 && dy == 0) {
                continue; // Skip the center cell itself
            }
            numOfLiveNeighbours += sharedGrid[(sharedY + dy) * sharedWidth + (sharedX + dx)];
        }
    }

    // Apply the rules of the game and write to the new grid
    if (sharedGrid[sharedY * sharedWidth + sharedX] == 1) {
        newGrid[globalY * columnLength + globalX] = (numOfLiveNeighbours == 2 || numOfLiveNeighbours == 3) ? 1 : 0;
    } else {
        newGrid[globalY * columnLength + globalX] = (numOfLiveNeighbours == 3) ? 1 : 0;
    }
}

// Draw the gird using RectangleShape
void drawGrid(sf::RenderWindow window) {
    window.clear(sf::Color::Black);
    for (int x = 0; x < columnLength; ++x) {
        for (int y = 0; y < rowLength; ++y) {
            if (hostGrid[y * columnLength + x] == 1) {
                sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));
                cell.setPosition(x * cellSize, y * cellSize);
                cell.setFillColor(sf::Color::White);
                window.draw(cell);
            }
        }
    }
    window.display();
}

void runIterations() {
    // Create a window
    sf::RenderWindow window(sf::VideoMode(windowWeight, windowHeight), "John Conway\'s Game of Life");

    // Allocate device memory
    cudaMalloc(&d_grid_current, columnLength * rowLength * sizeof(int));
    cudaMalloc(&d_grid_next, columnLength * rowLength * sizeof(int));

    // Copy the initial state to the device
    cudaMemcpy(d_grid_current, hostGrid.data(), columnLength * rowLength * sizeof(int), cudaMemcpyHostToDevice);

    // Define the block and grid size
    int blockSizeX = 32;  // Example block size
    int blockSizeY = numThreadPerBlock / blockSizeX;
    dim3 blockSize(blockSizeX, blockSizeY);
    dim3 gridSize((columnLength + blockSizeX - 1) / blockSizeX, (rowLength + blockSizeY - 1) / blockSizeY);
    size_t sharedMemSize = (blockSizeX + 2 * RADIUS) * (blockSizeY + 2 * RADIUS) * sizeof(int);

    // CUDA events for timing
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    float elapsedTime = 0.0f;  // Accumulator for kernel execution time
    int iterationCount = 0;

    // Main loop to update the grid while the window is open
    while (window.isOpen()) {
        // Handle window events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            }
        }

        // If the Esc key is pressed, close the window
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            window.close();
            return;
        }

        // Start timing for kernel execution
        cudaEventRecord(start, 0);

        // Launch the kernel
        updateSingleGridKernelWithHalo<<<gridSize, blockSize, sharedMemSize>>>(d_grid_current, d_grid_next, columnLength, rowLength);
        cudaDeviceSynchronize();

        // Stop timing for kernel execution
        cudaEventRecord(stop, 0);
        cudaEventSynchronize(stop);
        float iterationTime = 0.0f;
        cudaEventElapsedTime(&iterationTime, start, stop);
        elapsedTime += iterationTime;

        // Swap the buffers
        std::swap(d_grid_current, d_grid_next);

        // Print elapsed time for every 100 iterations
        if (++iterationCount % 100 == 0) {
            std::cout << "Time for 100 iterations: " << elapsedTime << " ms" << std::endl;
            elapsedTime = 0.0f; // Reset elapsed time for the next 100 iterations
        }

        // Copy the current grid to host memory for visualization
        cudaMemcpy(hostGrid.data(), d_grid_current, columnLength * rowLength * sizeof(int), cudaMemcpyDeviceToHost);

        // Draw the grid using RectangleShape
        drawGrid(window);
    }

    // Free device memory
    cudaFree(d_grid_current);
    cudaFree(d_grid_next);

    // Destroy CUDA events
    cudaEventDestroy(start);
    cudaEventDestroy(stop);
}

int main(int agrc, char* argv[]) {
    // Obtain the basic arguments
    argumentsParse(argc, agrv);
    std::cout << "numThreadPerBlock: " << numThreadPerBlock << " cellSize: " << cellSize << " windowWeight: "
	    << windowWeight << " windowHeight: " << windowHeight << " MemoryType: "
	    << (MemoryType == MemoryType::NORMAL ? "NORMAL" : (executionType == MemoryType::PINNED ? "PINNED" : "MANAGED"))
	    << std::endl;
    
    // Create a window
    sf::RenderWindow window(sf::VideoMode(windowWeight, windowHeight), "John Conway\'s Game of Life");

    // Initilize grid
    columnLength = windowWeight / cellSize;
    rowLength = windowHeight / cellSize;
    totalGrid = columnLength * rowLength;

    // Use randon seed to initial grid
    std::srand(RAND_SEED);
    hostGrid.reserve(totalGrid);
    for (int i = 0; i < totalGrid; i++) {
        hostGrid[i] = std::rand() % 2;
    }
    
    return 0;
}
