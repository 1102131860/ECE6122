# ECE 6122 Advanced Programming Techniques Labs and Projects

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
        ```bash
        git clone -b Lab1 https://github.com/1102131860/ECE6122.git
        mv ./ECE6122 ./Lab1
        ```
    4. Copy the `CMakeList.txt` in the **main branch** under the `Beginning-Cpp-Game-Programming-Second-Edition` (out of `SFML` and `Lab1`).
        - You may comment the other folder you don't want to implement, like `add_subdirectory(Lab2)` but keep the `add_subdirectory(SFML)` works. 
        - Pay attention, there is also a CMakeList.txt file in the Lab1 folder. That file is used to build the specific Lab1 project, don't change that file.
        ```bash
        git clone -b main https://github.com/1102131860/ECE6122.git
        mv ./ECE6122/CMakeLists.txt ./
        rm -rf ./ECE6122
        ```
    5. In the Beginning-Cpp-Game-Programming-Second-Edition folder, build `build` folder and build project inside it:
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
        ```bash
        git clone -b Lab2 https://github.com/1102131860/ECE6122.git
        mv ./ECE6122 ./Lab2
        ```
    4. Copy the `CMakeList.txt` in the **main branch** under the `Beginning-Cpp-Game-Programming-Second-Edition` (out of `SFML` and `Lab1`).
        - You may comment the other folder you don't want to implement, like `add_subdirectory(Lab1)` but keep the `add_subdirectory(SFML)` works. 
        - Pay attention, there is also a CMakeList.txt file in the Lab1 folder. That file is used to build the specific Lab2 project, don't change that file.
        ```bash
        git clone -b main https://github.com/1102131860/ECE6122.git
        mv ./ECE6122/CMakeLists.txt ./
        rm -rf ./ECE6122
        ```
    5. In the Beginning-Cpp-Game-Programming-Second-Edition folder, build `build` folder and build project inside it:
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
    2. In the `ogl` folder, clone `ECE6122` repository and rename as `Lab3`:
        ```bash
        cd ogl
        git clone -b Lab3 https://github.com/1102131860/ECE6122.git
        mv ./ECE6122 ./Lab3
        ```
    3. Move the `CMakeLists.txt` at the `Lab3` folder into `ogl` folder. 
        ```bash
        rm ./CMakeLists.txt
        mv ./Lab3/CMakelists.txt ./ 
        ```
    4. Move the `common` folder at the `Lab3` folder into `ogl` folder.
        ```bash
        rm -rf ./common
        mv ./Lab3/common ./common
        ```
    5. In the Beginning-Cpp-Game-Programming-Second-Edition folder, build `build` folder and build project inside it:
        ```bash
        cmake -B build
        cmake --build build -j8
        ```
    6. Execute the program:
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
