# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(UDPServer VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# set(OPENAL_LIBRARY ${PROJECT_SOURCE_DIR}/../SFML/extlibs/libs-msvc/x64/openal32.lib) # for open Ardio dll

set(SOURCE ${PROJECT_SOURCE_DIR}/UDPServer.cpp)

# Add the executable
add_executable(UDPServer ${SOURCE})

include_directories(${PROJECT_SOURCE_DIR}/../SFML/include)

link_directories(${PROJECT_SOURCE_DIR}/../SFML/lib)

# Link the executable to the libraries in the lib directory
target_link_libraries(UDPServer PUBLIC sfml-graphics sfml-system sfml-window sfml-network)

set_target_properties(
    UDPServer PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${COMMON_OUTPUT_DIR}/bin"
)

# Copy DLLs using file(COPY ...)
# if(NOT EXISTS "${COMMON_OUTPUT_DIR}/bin/openal32.dll")
#     file(COPY "${PROJECT_SOURCE_DIR}/../SFML/extlibs/bin/x64/openal32.dll"
#          DESTINATION "${COMMON_OUTPUT_DIR}/bin")
# endif()
