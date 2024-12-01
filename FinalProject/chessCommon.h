/*
Author: Rui Wang
Class: ECE6122
Last Date Modified: Dec 1st, 2024

Description:
Game of Chess common file
*/

#ifndef COMMON_H
#define COMMON_H

#include <unordered_map>
// Include GLM
#include <glm/glm.hpp>
#include <sstream>
#include <vector>

// Mesh properties has table
typedef struct 
{
    bool hasBones;
    bool hasFaces;
    bool hasNormals;
    bool hasPositions;
    bool hasTangentsAndBitangents;
    bool hasTextureCoords;
    bool hasVertexColors;
    unsigned int numOfUVChannels;
} meshPropsT;

// Structure to hold target
// model matrix generation
typedef struct
{
    unsigned int rCnt;
    float rAngle;
    glm::vec3 rAxis;
    glm::vec3 cScale;
    glm::vec3 tPos;
} tPosition;

// Total movement time in seconds
const float TOTAL_MOVE_TIME = 2.f;
// Maximum movement height
const float MAXIMUM_MOVEMENT_HEIGHT = 10.f;

// Chess board scaling
const float CBSCALE = 0.6f;
// Chess board square box size (per side)
//const float CHESS_BOX_SIZE = 3.f;
const float CHESS_BOX_SIZE = (float)(CBSCALE * 5.4);
// Chess pieces scaling
const float CPSCALE = 0.015f;
// Platform height
const float PHEIGHT = -3.0f;
// Hash to hold the target Model matrix spec for each Chess component
typedef std::unordered_map <std::string, tPosition> tModelMap;

// Helper function -- Split string cmd
inline std::vector<std::string> split(const std::string& str, char delimiter)
{
    std::vector<std::string> tokens;
    std::istringstream stream(str);
    std::string token;

    while (std::getline(stream, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

#endif
