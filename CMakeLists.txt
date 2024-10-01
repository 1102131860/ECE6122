# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(Lab2 VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(OPENAL_LIBRARY ${PROJECT_SOURCE_DIR}/../SFML/extlibs/libs-msvc/x64/openal32.lib)

# Add source files
file(GLOB SOURCES 
    "${PROJECT_SOURCE_DIR}/code/*.h"
    "${PROJECT_SOURCE_DIR}/code/*.cpp")

# Add the executable
add_executable(Lab2 ${SOURCES})

include_directories(${PROJECT_SOURCE_DIR}/../SFML/include)

link_directories(${PROJECT_SOURCE_DIR}/../SFML/lib)

# Link the executable to the libraries in the lib directory
target_link_libraries(Lab2 PUBLIC sfml-graphics sfml-system sfml-window ${OPENAL_LIBRARY})

set_target_properties(
    Lab2 PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${COMMON_OUTPUT_DIR}/bin"
)

# message(STATUS "COMMON_OUTPUT_DIR is: ${COMMON_OUTPUT_DIR}")
