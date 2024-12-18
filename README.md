# ECE 6122 Advanced Programming Techniques

## General Explanation
- There are 6 labs and 1 final project in this session. 
- All the projects are implemented using C++. There are several librarys and platforms in this session:
    - SFML
    - OpenMP
    - OpenGL
    - CUDA
    - Socket
    - OpenMPI
- Make sure you have download CMake build tool and configure it into your local environment.
- To utilize these examples, please go to different branches to see the details.
- This main branch works as a general installation and user guide for each labs and project.

## Lab 1 Retro Centipede Arcade
- This lab uses SFML library to create a centipede game. User control spaceship to shot the centipede and avoid paunched by centipede or spider.
- To run the demo:
    1. Clone `Beginning-Cpp-Game-Programming-Second-Edition` repository:  
        ```bash
        git clone https://github.com/PacktPublishing/Beginning-Cpp-Game-Programming-Second-Edition
        ```
    2. Enter that repository, and clone `SFML` library:
        ```bash
        cd Beginning-Cpp-Game-Programming-Second-Edition
        git clone -b 2.6.x https://github.com/SFML/SFML.git
        ```
    3. At the same level, clone `ECE6122` repository and rename as `Lab1`:
        - Pay attention, there is a `CMakeList.txt` file in the `Lab1` folder, and it is used to build the specific Lab1 project, don't change that file.
        ```bash
        git clone -b Lab1 https://github.com/1102131860/ECE6122.git
        mv ./ECE6122 ./Lab1
        ```
    4. Copy the `CMakeList.txt` in the **main branch** under the `Beginning-Cpp-Game-Programming-Second-Edition` (out of `SFML` and `Lab1`).
        - You may comment the other folder you don't want to implement, like `add_subdirectory(Lab2)` but keep the `add_subdirectory(SFML)`. 
        ```bash
        git clone -b main https://github.com/1102131860/ECE6122.git
        mv ./ECE6122/CMakeLists.txt ./
        rm -rf ./ECE6122
        ```
    5. In the `Beginning-Cpp-Game-Programming-Second-Edition` folder, build `build` folder and build project inside it:
        ```bash
        cmake -B build
        cmake --build build
        ```
    6. Execute the program:
        ```bash
        cd build/output/bin/
        ``` 
        In Linux:
        ```
        ./Lab1
        ```
        In Windows:
        ```
        .\Lab1.exe
        ```

## Lab 2 John Conway's Game of Life
- This lab uses multithreading and OpenMP library to expediate the iteration of grid and control game life.
- To run the demo:
    1. Clone `Beginning-Cpp-Game-Programming-Second-Edition` repository:  
        ```bash
        git clone https://github.com/PacktPublishing/Beginning-Cpp-Game-Programming-Second-Edition
        ```
    2. Enter that repository, and clone `SFML` library:
        ```bash
        cd Beginning-Cpp-Game-Programming-Second-Edition
        git clone -b 2.6.x https://github.com/SFML/SFML.git
        ```
    3. At the same level, clone `ECE6122` repository and rename as `Lab2`:
        - Pay attention, there is a `CMakeList.txt` file in the `Lab2` folder, and it is used to build the specific Lab2 project, don't change that file.
        ```bash
        git clone -b Lab2 https://github.com/1102131860/ECE6122.git
        mv ./ECE6122 ./Lab2
        ```
    4. Copy the `CMakeList.txt` in the **main branch** under the `Beginning-Cpp-Game-Programming-Second-Edition` (out of `SFML` and `Lab2`).
        - You may comment the other folder you don't want to implement, like `add_subdirectory(Lab1)` but keep the `add_subdirectory(SFML)` works. 
        ```bash
        git clone -b main https://github.com/1102131860/ECE6122.git
        mv ./ECE6122/CMakeLists.txt ./
        rm -rf ./ECE6122
        ```
    5. In the `Beginning-Cpp-Game-Programming-Second-Edition` folder, build `build` folder and build project inside it:
        ```bash
        cmake -B build
        cmake --build build -j8
        ```
    6. Execute the program:
        ```bash
        cd build/output/bin/
        ``` 
        In Linux:
        ```
        ./Lab2
        ```
        In Windows:
        ```
        .\Lab2.exe
        ```

## Lab 3 OpenGL with OBJ files and Mutiple Objects
- This lab creates a dynamic 3D graphics application using lighting, shading, model transformations and keyboard inputs.
- To run the demo:
    1. Clone `ogl` repository:  
        ```bash
        git clone https://github.com/opengl-tutorials/ogl.git
        ```
    2. In the `ogl` folder, clone `ECE6122` repository:
        ```bash
        cd ogl
        git clone -b Lab3 https://github.com/1102131860/ECE6122.git
        ```
    3. Move the `CMakeLists.txt` at the `ECE6122` folder into `ogl` folder. 
        ```bash
        rm ./CMakeLists.txt
        mv ./ECE6122/CMakelists.txt ./ 
        ```
    4. Move the `common` folder at the `ECE6122` folder into `ogl` folder.
        ```bash
        rm -rf ./common
        mv ./ECE6122/common ./
        ```
    5. Move the `Lab3` folder at the `ECE6122` folder into 'ogl' folder.
        ```bash
        mv ./ECE6122/Lab3 ./
        rm -rf ./ECE61222
        ```
    6. In the `ogl` folder, build `build` folder and build project inside it:
        ```bash
        cmake -B build
        cmake --build build -j8
        ```
    7. Execute the program:
        ```bash
        cd ./Lab3
        ``` 
        In Linux:
        ```
        ./Lab3
        ```
        In Windows:
        ```
        .\Lab3.exe
        ```
    
## Lab 4 CUDA-based John Conway’s Game of Life
- This lab implements a CUDA program to run the Game of Life.
- Please ensure that you have configured cuda operation environment.
- To run the demo:
    1. Clone `Beginning-Cpp-Game-Programming-Second-Edition` repository:  
        ```bash
        git clone https://github.com/PacktPublishing/Beginning-Cpp-Game-Programming-Second-Edition
        ```
    2. Enter that repository, and clone `SFML` library:
        ```bash
        cd Beginning-Cpp-Game-Programming-Second-Edition
        git clone -b 2.6.x https://github.com/SFML/SFML.git
        ```
    3. At the same level, clone `ECE6122` repository and rename as `Lab4`:
        - Pay attention, there is a `CMakeList.txt` file in the `Lab4` folder, and it is used to build the specific Lab4 project, don't change that file.
        ```bash
        git clone -b Lab4 https://github.com/1102131860/ECE6122.git
        mv ./ECE6122 ./Lab4
        ```
    4. Copy the `CMakeList.txt` in the **main branch** under the `Beginning-Cpp-Game-Programming-Second-Edition` (out of `SFML` and `Lab4`).
        - You may comment the other folder you don't want to implement, like `add_subdirectory(Lab1)` but keep the `add_subdirectory(SFML)` works. 
        ```bash
        git clone -b main https://github.com/1102131860/ECE6122.git
        mv ./ECE6122/CMakeLists.txt ./
        rm -rf ./ECE6122
        ```
    5. Load the `cuda` module (This is an example in the PACE cloud platform, make sure you chose the NVIDIA GPU).
        ```bash
        module load cuda
        ```
    6. In the `Beginning-Cpp-Game-Programming-Second-Edition` folder, build `build` folder and build project inside it:
        ```bash
        cmake -B build
        cmake --build build -j8
        ```
    7. Execute the program:
        ```bash
        cd build/output/bin/
        ./Lab4
        ```

## Lab 5 TCP Sockets
- This lab simulates the movement of a robot on a 2D screen using SFML for graphics and UDP sockets for communication.
- The robot’s location will be updated by sending direction data over a UDP socket from a client to a server.
- The server will render the updated direction for the robot on the SFML window.
- To run the demo:
    1. Clone `Beginning-Cpp-Game-Programming-Second-Edition` repository:  
        ```bash
        git clone https://github.com/PacktPublishing/Beginning-Cpp-Game-Programming-Second-Edition
        ```
    2. Enter that repository, and clone `SFML` library:
        ```bash
        cd Beginning-Cpp-Game-Programming-Second-Edition
        git clone -b 2.6.x https://github.com/SFML/SFML.git
        ```
    3. At the same level, clone `ECE6122` repository and rename as `Lab5`:
        - Pay attention, there is a `CMakeList.txt` file in the `Lab5` folder, and it is used to build the specific Lab5 project, don't change that file.
        ```bash
        git clone -b Lab5 https://github.com/1102131860/ECE6122.git
        mv ./ECE6122 ./Lab5
        ```
    4. Copy the `CMakeList.txt` in the **main branch** under the `Beginning-Cpp-Game-Programming-Second-Edition` (out of `SFML` and `Lab5`).
        - You may comment the other folder you don't want to implement, like `add_subdirectory(Lab1)` but keep the `add_subdirectory(SFML)` works. 
        ```bash
        git clone -b main https://github.com/1102131860/ECE6122.git
        mv ./ECE6122/CMakeLists.txt ./
        rm -rf ./ECE6122
        ```
    5. In the `Beginning-Cpp-Game-Programming-Second-Edition` folder, build `build` folder and build project inside it:
        ```bash
        cmake -B build
        cmake --build build -j8
        ```
    6. Execute the program:
        ```bash
        cd build/output/bin/
        ``` 
        In Linux:
        ```
        ./UDPServer
        ./UDPClient
        ```
        In Windows:
        ```
        .\UDPServer.exe
        .\UDPClient.exe
        ```

## Lab 6 Using OpenMPI to Estimate the Value of a Definite Integral using the Monte Carlo method
- This lab uses OpenMPI to estimate the value of a definite integral using the Monte Carlo method.
- To run the demo:
    1. Clone `ECE6122` repository:
        ```bash
        git clone -b Lab6 https://github.com/1102131860/ECE6122.git
        mv ./ECE6122 ./Lab6
        ```
    2. Compile the .cpp file (working on the PACE clusters, make sure you chose multiple cores in each clusters (more than 2)):
        ```bash
        cd ./Lab6
        mpic++ ./Lab6.cpp -o Lab6
        ```
    3. Execute the program:
        ```bash
        srun ./Lab6
        ```

## Final Project 3D Chess Game Custom Classes & OpenGL
- This program uses a custom class(es) with OpenGL, a third-party library (i.e. ASSIMP), and a third-party chess engine (i.e. Komodo, see attached file) to create a fully operational 3D chess game. 
- To run the demo:
    1. Clone `ogl` repository:  
        ```bash
        git clone https://github.com/opengl-tutorials/ogl.git
        ```
    2. In the `ogl` folder, clone `ECE6122` repository:
        ```bash
        cd ogl
        git clone -b FinalProject https://github.com/1102131860/ECE6122.git
        ```
    3. Move the `CMakeLists.txt` at the `ECE6122` folder into `ogl` folder. 
        ```bash
        rm ./CMakeLists.txt
        mv ./ECE6122/CMakelists.txt ./ 
        ```
    4. Move the `common` folder at the `ECE6122` folder into `ogl` folder.
        ```bash
        rm -rf ./common
        mv ./ECE6122/common ./
        ```
    5. Move the `FinalProject` folder at the `ECE6122` folder into 'ogl' folder.
        ```bash
        mv ./ECE6122/FinalProject ./
        rm -rf ./ECE61222
        ```
    6. In the `ogl` folder, build `build` folder and build project inside it:
        ```bash
        cmake -B build
        cmake --build build -j8
        ```
    7. Execute the program:
        ```bash
        cd ./FinalProject
        ``` 
        In Linux:
        ```
        ./finalProject
        ```
        In Windows:
        ```
        .\FinalProject.exe
        ```
