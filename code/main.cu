#include <SFML/Graphics.hpp>
#include <cuda_runtime.h>
#include <iostream>
#include <vector>
#include <string>

#define DEFAULT_NUM_THREAD_PER_BLOCK 32
#define DEFAULT_CELL_SIZE 5
#define DEFAUlT_WINDOW_WEIGHT 800
#define DEFAULT_WINDOW_HEIGHT 600
#define RAND_SEED 42

enum class MemoryType {
	NORMAL = 0,
	PINNED = 1,
	MANAGED = 2
};

int numThreadPerBlock = DEFAULT_NUM_THREAD_PER_BLOCK;
int cellSize = DEFAULT_CELL_SIZE;
int windowWeight = DEFAUlT_WINDOW_WEIGHT;
int windowHeight = DEFAULT_WINDOW_HEIGHT;
MemoryType memoryType = MemoryType::NORMAL;

int columnLength = windowWeight / cellSize;
int rowLength = windowHeight / cellSize;
int totalGrid = columnLength * rowLength;
int numBlocks = (totalGrid + numThreadPerBlock - 1) / numThreadPerBlock;

std::vector<int> hostGrid;
int* d_grid_current, *d_grid_next;

// Error checking macro for CUDA calls
#define cudaCheckError(ans) { cudaAssert((ans), __FILE__, __LINE__); }
inline void cudaAssert(cudaError_t code, const char *file, int line, bool abort=true) {
    if (code != cudaSuccess) {
        std::cerr << "CUDA Error: " << cudaGetErrorString(code) << " in " << file << " at line " << line << std::endl;
        if (abort) exit(code);
    }
}

// Update 1-D Grid
__global__ void updateSingleGridKernelWithHalo(int* grid, int* newGrid, int columnLength, int rowLength) {
    // Calculate the global 1D index for the current thread
    int globalIndex = blockIdx.x * blockDim.x + threadIdx.x;

    // Map the 1D index to 2D coordinates (globalX and globalY)
    int globalX = globalIndex % columnLength; // X-coordinate
    int globalY = globalIndex / columnLength; // Y-coordinate

    // Check if the current thread is within the grid boundaries
    if (globalX < columnLength && globalY < rowLength) {
        // Calculate the number of live neighbors
        int numOfLiveNeighbours = 0;

        // Iterate over all neighbors (including diagonals)
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                // Skip the center cell itself
                if (dx == 0 && dy == 0) continue;

                int neighborX = globalX + dx;
                int neighborY = globalY + dy;

                // Check if the neighbor is within the grid boundaries
                if (neighborX >= 0 && neighborX < columnLength && neighborY >= 0 && neighborY < rowLength) {
                    numOfLiveNeighbours += grid[neighborY * columnLength + neighborX];
                }
            }
        }

        // Apply the rules of the game and write to the new grid
        if (grid[globalY * columnLength + globalX] == 1) {
            newGrid[globalY * columnLength + globalX] = (numOfLiveNeighbours == 2 || numOfLiveNeighbours == 3) ? 1 : 0;
        } else {
            newGrid[globalY * columnLength + globalX] = (numOfLiveNeighbours == 3) ? 1 : 0;
        }
    }
}

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
            } else if (argValue == "PINNED") {
                memoryType = MemoryType::PINNED;
            } else if (argValue == "MANAGED") {
                memoryType = MemoryType::MANAGED;
            } else {
                std::cout << "Invalid argument (-t): select a type from NORMAL, PINNED and MANAGED" << std::endl;
                memoryType = MemoryType::NORMAL;
            }
        }
    }
}

// Draw the gird using RectangleShape
void drawGrid(sf::RenderWindow& window, int* grid) {
    window.clear(sf::Color::Black);
    for (int x = 0; x < columnLength; ++x) {
        for (int y = 0; y < rowLength; ++y) {
            if (grid[y * columnLength + x] == 1) {
                sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));
                cell.setPosition(x * cellSize, y * cellSize);
                cell.setFillColor(sf::Color::White);
                window.draw(cell);
            }
        }
    }
    window.display();
}

// Normal memory transfer
void normalMemory(sf::RenderWindow& window) {
    // Create CUDA streams
    cudaStream_t stream1, stream2;
    cudaCheckError(cudaStreamCreate(&stream1));
    cudaCheckError(cudaStreamCreate(&stream2));

    // Allocate device memory
    cudaCheckError(cudaMalloc(&d_grid_current, columnLength * rowLength * sizeof(int)));
    cudaCheckError(cudaMalloc(&d_grid_next, columnLength * rowLength * sizeof(int)));

    // Asynchronously copy the initial state to the device using stream1
    cudaCheckError(cudaMemcpyAsync(d_grid_current, hostGrid.data(), columnLength * rowLength * sizeof(int), cudaMemcpyHostToDevice, stream1));

    cudaEvent_t start, stop;
    cudaCheckError(cudaEventCreate(&start));
    cudaCheckError(cudaEventCreate(&stop));

    float elapsedTime = 0.0f;
    int iterationCount = 0;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                break;
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            window.close();
            break;
        }

        // Record the start time of kernel execution
        cudaCheckError(cudaDeviceSynchronize());
        cudaCheckError(cudaEventRecord(start, stream1));

        // Launch the kernel using stream1
        updateSingleGridKernelWithHalo<<<numBlocks, numThreadPerBlock, 0, stream1>>>(d_grid_current, d_grid_next, columnLength, rowLength);

        // Asynchronously copy the result back to host memory using stream2
        cudaCheckError(cudaMemcpyAsync(hostGrid.data(), d_grid_current, columnLength * rowLength * sizeof(int), cudaMemcpyDeviceToHost, stream2));

        // Record the stop time and synchronize
        cudaCheckError(cudaEventRecord(stop, stream1));
        cudaCheckError(cudaEventSynchronize(stop));

        float iterationTime = 0.0f;
        cudaCheckError(cudaEventElapsedTime(&iterationTime, start, stop));
        elapsedTime += iterationTime;

        // Print the elapsed time every 100 iterations
        if (++iterationCount % 100 == 0) {
            std::cout << "100 generations took " << elapsedTime << " ms with " << numThreadPerBlock << " threads per block using Normal memory allocation with streams\n";
            elapsedTime = 0.0f;
        }

        // Draw the grid using the data from hostGrid
        drawGrid(window, hostGrid.data());

        // Swap the grid buffers
        std::swap(d_grid_current, d_grid_next);
    }

    // Free device memory and destroy streams and events
    cudaFree(d_grid_current);
    cudaFree(d_grid_next);
    cudaStreamDestroy(stream1);
    cudaStreamDestroy(stream2);
    cudaEventDestroy(start);
    cudaEventDestroy(stop);
}

// Pinned memory transfer
void pinnedMemory(sf::RenderWindow& window) {
    cudaStream_t stream1, stream2;
    cudaCheckError(cudaStreamCreate(&stream1));
    cudaCheckError(cudaStreamCreate(&stream2));

    // Allocate pinned host memory and device memory
    int* pinnedHostGrid;
    cudaCheckError(cudaMallocHost(&pinnedHostGrid, columnLength * rowLength * sizeof(int)));
    memcpy(pinnedHostGrid, hostGrid.data(), columnLength * rowLength * sizeof(int));

    cudaCheckError(cudaMalloc(&d_grid_current, columnLength * rowLength * sizeof(int)));
    cudaCheckError(cudaMalloc(&d_grid_next, columnLength * rowLength * sizeof(int)));

    // Asynchronously copy the initial state to the device using stream1
    cudaCheckError(cudaMemcpyAsync(d_grid_current, pinnedHostGrid, columnLength * rowLength * sizeof(int), cudaMemcpyHostToDevice, stream1));

    cudaEvent_t start, stop;
    cudaCheckError(cudaEventCreate(&start));
    cudaCheckError(cudaEventCreate(&stop));

    float elapsedTime = 0.0f;
    int iterationCount = 0;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                break;
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            window.close();
            break;
        }

        // Record the start time of kernel execution
        cudaCheckError(cudaDeviceSynchronize());
        cudaCheckError(cudaEventRecord(start, stream1));

        // Launch the kernel using stream1
        updateSingleGridKernelWithHalo<<<numBlocks, numThreadPerBlock, 0, stream1>>>(d_grid_current, d_grid_next, columnLength, rowLength);

        // Asynchronously copy the result back to pinned host memory using stream2
        cudaCheckError(cudaMemcpyAsync(pinnedHostGrid, d_grid_current, columnLength * rowLength * sizeof(int), cudaMemcpyDeviceToHost, stream2));
        
        // Record the stop time and synchronize
        cudaCheckError(cudaEventRecord(stop, stream1));
        cudaCheckError(cudaEventSynchronize(stop));

        float iterationTime = 0.0f;
        cudaCheckError(cudaEventElapsedTime(&iterationTime, start, stop));
        elapsedTime += iterationTime;

        if (++iterationCount % 100 == 0) {
            std::cout << "100 generations took " << elapsedTime << " ms with " << numThreadPerBlock << " threads per block using Pinned memory allocation and streams\n";
            elapsedTime = 0.0f;
        }

        // Directly draw the latest grid using the pinned host memory
        drawGrid(window, pinnedHostGrid);

        // Swap the grid buffers
        std::swap(d_grid_current, d_grid_next);
    }

    // Free pinned host memory, device memory, destroy streams and events
    cudaFreeHost(pinnedHostGrid);
    cudaFree(d_grid_current);
    cudaFree(d_grid_next);
    cudaStreamDestroy(stream1);
    cudaStreamDestroy(stream2);
    cudaEventDestroy(start);
    cudaEventDestroy(stop);
}

// Managed memory transfer
void managedMemory(sf::RenderWindow& window) {
    // Allocate managed memory
    cudaCheckError(cudaMallocManaged(&d_grid_current, columnLength * rowLength * sizeof(int)));
    cudaCheckError(cudaMallocManaged(&d_grid_next, columnLength * rowLength * sizeof(int)));

    // Copy the initial state to managed memory
    cudaCheckError(cudaMemcpy(d_grid_current, hostGrid.data(), columnLength * rowLength * sizeof(int), cudaMemcpyHostToDevice));

    cudaEvent_t start, stop;
    cudaCheckError(cudaEventCreate(&start));
    cudaCheckError(cudaEventCreate(&stop));

    float elapsedTime = 0.0f;
    int iterationCount = 0;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                break;
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            window.close();
            break;
        }

        // Record the start time of kernel execution
        cudaCheckError(cudaEventRecord(start, 0));

        // Launch the kernel using the default stream
        updateSingleGridKernelWithHalo<<<numBlocks, numThreadPerBlock>>>(d_grid_current, d_grid_next, columnLength, rowLength);

        // Synchronize to ensure kernel execution is complete
        cudaCheckError(cudaDeviceSynchronize());

        // Record the stop time
        cudaCheckError(cudaEventRecord(stop, 0));
        cudaCheckError(cudaEventSynchronize(stop));

        float iterationTime = 0.0f;
        cudaCheckError(cudaEventElapsedTime(&iterationTime, start, stop));
        elapsedTime += iterationTime;

        if (++iterationCount % 100 == 0) {
            std::cout << "100 generations took " << elapsedTime << " ms with " << numThreadPerBlock << " threads per block using Managed memory allocation\n";
            elapsedTime = 0.0f;
        }

        // Draw the grid using the updated host data
        drawGrid(window, d_grid_current);

        // Swap the grid buffers for the next iteration
        std::swap(d_grid_current, d_grid_next);
    }

    // Free managed memory and destroy events
    cudaFree(d_grid_current);
    cudaFree(d_grid_next);
    cudaEventDestroy(start);
    cudaEventDestroy(stop);
}

int main(int argc, char* argv[]) {
    // Obtain the basic arguments
    argumentsParse(argc, argv);

    // Create a window
    sf::RenderWindow window(sf::VideoMode(windowWeight, windowHeight), "John Conway\'s Game of Life");
    
    // Initialize grid size and number of blocks
    columnLength = windowWeight / cellSize;
    rowLength = windowHeight / cellSize;
    totalGrid = columnLength * rowLength;
    numBlocks = (totalGrid + numThreadPerBlock - 1) / numThreadPerBlock;

    // Use randon seed to initialize grid
    std::srand(RAND_SEED);
    hostGrid.reserve(totalGrid);
    for (int i = 0; i < totalGrid; i++) {
        hostGrid[i] = std::rand() % 2;
    }

    // Call the different types of memory transfer
    switch (memoryType) {
        case MemoryType::NORMAL:
            normalMemory(window);
            break;
        case MemoryType::PINNED:
            pinnedMemory(window);
            break;
        case MemoryType::MANAGED:
            managedMemory(window);
            break;
        default:
            std::cout << "Undefined Memory Type. Please choose one from NOMRAL, PINNED, MANAGED" << std::endl;
    }
    
    return 0;
}
