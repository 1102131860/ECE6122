/*
Author: Rui Wang
Class: ECE6122
Last Date Modified: Dec 1st, 2024

Description:
The main function includes three threads: main thread for rending, another thread for
dealing with input command from user, and the last thread for dealing with chess engine
*/

// Include standard headers
#include <vector>
#include <sstream>
#include <string>
#include <thread>
#include <atomic>
// Include GLEW
#include <GL/glew.h>
// Include GLFW
#include <GLFW/glfw3.h>
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// User supporting files
#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
// FinalProject specific chess class
#include "chessComponent.h"
#include "chessCommon.h"
#include "ChessGame.h"
#include "ECE_ChessEngine.h"
using namespace glm;

/*****************************Function Declaration********************************/
// Listen to input command in a thread
void inputThreadFunction();
// Interact with Chess Engine in a thread
void interactWithChessEngine();
// Sets up the chess board
void setupChessBoard(tModelMap& cTModelMap);
// Update the chess board
void updateChessBoard(tModelMap& cTModelMap, DualMovement& rendMovement);
// Update pawn promotion
void updatePawnPromotion(tModelMap& cTModelMap, DualChess& pawnPromotion);
// Helper function to update the chess board 
vec3 calculateIntermediatePosition(Movement& rendMovement, float elapsedTime, bool isCapture);
// Covert Mapping from Chess to Model
std::string mapChessToModel(Chess chess);
/************************************************************************************/

/*********************************Global flags***************************************/
std::atomic<bool> isUserWhite(true);
std::atomic<bool> isAtUserRound(true);
std::atomic<bool> isGameOver(false);
std::atomic<bool> enableMoveRend(false);
/************************************************************************************/

/*******************************Gloabl Pointers*****************************************/
// A point to a GLFW winodw
GLFWwindow* window;
// A point to a chess game
ChessGame* chessGame;
// A point to a ECE chessEngine
ECE_ChessEngine* komodo;
/****************************************************************************************/

int main( void )
{
    // Initialize GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make macOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 1024, 768, "Game Of Chess 3D", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version.\n" );
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited movement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it is closer to the camera than the former one
    glDepthFunc(GL_LESS); 

    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders( "StandardShading.vertexshader", "StandardShading.fragmentshader" );

    // Use our shader (Not changing the shader per chess component)
    glUseProgram(programID);

    // Get a handle for our "MVP" uniform
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
    GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

    // Get a handle for our "myTextureSampler" uniform
    GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");

    // Get a handle for our "LightPosition" uniform
    GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

    // Get a handle for our "lightToggleSwitch" uniform
    GLuint LightSwitchID = glGetUniformLocation(programID, "lightSwitch");

    // Get a handle for our "intensity" uniform
    GLuint LightPowerID = glGetUniformLocation(programID, "intensity");

    // Create a vector of chess components class
    // Each component is fully self sufficient
    std::vector<chessComponent> gchessComponents;

    // Load the OBJ files
    bool cBoard = loadAssImpLab3("FinalProject/Stone_Chess_Board/12951_Stone_Chess_Board_v1_L3.obj", gchessComponents);
    bool cComps = loadAssImpLab3("FinalProject/Chess/chess-mod.obj", gchessComponents);

    // Proceed iff OBJ loading is successful
    if (!cBoard || !cComps)
    {
        // Quit the program (Failed OBJ loading)
        std::cout << "Program failed due to OBJ loading failure, please CHECK!" << std::endl;
        return -1;
    }

    // Setup the Chess board locations
    tModelMap cTModelMap;
    setupChessBoard(cTModelMap);

    // Load it into a VBO (One time activity)
    // Run through all the components for rendering
    for (auto cit = gchessComponents.begin(); cit != gchessComponents.end(); cit++)
    {
        // Setup VBO buffers
        cit->setupGLBuffers();
        // Setup Texture
        cit->setupTextureBuffers();
    }

    /****************Initailize a new Game and a Chess engine********************/
    chessGame = new ChessGame();
    komodo = new ECE_ChessEngine(KOMODO_ADDRESS);
    if (!komodo->InitializeEngine())
    {
        // Quit the program (Fail to initailize chess engine)
        std::cout << "Fail to initilize the ChessEngine. Please check the engine address" << std::endl;
        return -1;
    }
    /***************************************************************************/

    /***Launch two separate threads for input and interact with chess engine****/
    std::thread inputThread(inputThreadFunction);
    std::thread interactChessEngineThread(interactWithChessEngine);
    /***************************************************************************/

    do {
        /**********************Dynamically Rend Movemen******************************/
        if (enableMoveRend) {
            DualMovement rendMovement = chessGame->getLastMovement();
            updateChessBoard(cTModelMap, rendMovement);
        }
        /****************************************************************************/

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Compute the VP matrix from keyboard and mouse input
        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();

        // Get light intensity
        float intensity = getLightPower();
        // Pass it to the Fragment Shader
        glUniform1f(LightPowerID, intensity);

        // Get light switch State (It's a toggle!)
        bool lightSwitch = getLightSwitch();
        // Pass it to Fragment Shader
        glUniform1i(LightSwitchID, static_cast<int>(lightSwitch));

        // Light is placed right on the top of the board
        // with a decent height for good lighting across
        // the board!
        glm::vec3 lightPos = getLightPosition();
        glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

        // Run through all the chess game components for rendering
        for (auto cit = gchessComponents.begin(); cit != gchessComponents.end(); cit++)
        {
            // Obtain the need iteration name
            std::string componentID = cit->getComponentID();
            std::string firstComponentID = componentID + "_1";
            int numOfcomponent = cTModelMap[firstComponentID].rCnt;

            for (int pit = 0; pit < numOfcomponent; pit++) {
                std::string keyComponentID = componentID + "_" + std::to_string(pit + 1);
                // Obtain the transfomation position
                tPosition cTPosition = cTModelMap[keyComponentID];
                // Pass it for Model matrix generation
                glm::mat4 ModelMatrix = cit->genModelMatrix(cTPosition);
                // Genrate the MVP matrix
                glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

                // Send our transformation to the currently bound shader, 
                // in the "MVP" uniform
                glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
                glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
                glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

                // Bind our texture (set it up)
                cit->setupTexture(TextureID);

                // Render buffers
                cit->renderMesh();
            }
        }

        // Swap buffers (This will keep move window and get command separate)
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

    /**********************Wait for threads to finish*****************************/
    inputThread.join();
    interactChessEngineThread.join();
    /*****************************************************************************/

    /*****************Delete the ChessGame and Chess engine***********************/ 
    delete chessGame;
    delete komodo;
    /*****************************************************************************/

    // Cleanup VBO, Texture (Done in class destructor) and shader 
    glDeleteProgram(programID);
    glDeleteVertexArrays(1, &VertexArrayID);
    
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    // Shake hand for exit!
    return 0;
}

// Listen to input command in a thread
void inputThreadFunction() 
{
    // Choose a type of chess
    while (true) 
    {
        std::string choice;
        std::cout << "Choose White (first) or Black (second) Chess. Please type 1 or 2: ";
        std::getline(std::cin, choice);
        if (choice == "1") 
        {
            isUserWhite = true;
            isAtUserRound = true;
            break;
        }
        else if (choice == "2") 
        {
            isUserWhite = false;
            isAtUserRound = false;
            break;
        }
        else 
            std::cout << "Invalid input. Please type 1 or 2" << std::endl;
    }
    std::cout << "You chose " << (isUserWhite ? "White" : "Black") << " Chess" << std::endl;

    // At the user's round
    while (true) 
    {
        /*****************************Thread Terminates*********************************/
        if (isGameOver) break;
        /*******************************************************************************/

        // When it is at user's round and not rend chess movement
        if (isAtUserRound && !enableMoveRend) 
        {
            /*****************************Game Terminate Situation***************************/
            if (chessGame->doesCheck(isUserWhite)) {
                std::cout << "Check White King" << std::endl;
                if (chessGame->isDeadlyChecked(isUserWhite)) {
                    std::cout << "***********************Game Result**********************" << std::endl;
                    std::cout << "Checkmate White King" << std::endl;
                    std::cout << (isUserWhite ? "You lose!!" : "Congratulation!! You win!!") << std::endl;
                    std::cout << "********************************************************" << std::endl;
                    std::cout << "Thanks for playing!!!" << std::endl;
                    std::cout << "Close Window to exit..." << std::endl;
                    isGameOver = true;
                    break;
                }
            }
            /*******************************************************************************/

            std::string cmd;
            std::cout << "Please enter a command: ";
            std::getline(std::cin, cmd);

            // Set the global variable
            if (cmd == "quit") {
                // Display the end of game
                std::cout << "Thanks for playing!!!" << std::endl;
                std::cout << "Close Window to exit..." << std::endl;
                isGameOver = true;
                break;
            }

            // Split the string to deal with input command
            std::vector<std::string> splitedCmd = split(cmd, ' ');

            // Check input is movement
            if (splitedCmd.size() == 2 && splitedCmd[0] == "move")
            {
                // Move chess is successful
                if (chessGame->moveChess(splitedCmd[1], isUserWhite))
                {
                    // Enable render update
                    enableMoveRend = true;

                    // Record the user's movement and turn to Chess Engine
                    komodo->addMovement(splitedCmd[1]);
                    isAtUserRound = false;
                }
                else
                    std::cout << "Invalid command or move!!" << std::endl;
            }
            else // check other command through controller
            {
                if (!commandControl(splitedCmd))
                    std::cout << "Invalid command or move!!" << std::endl;
            }
        }
    }
}

// Interact with Chess Engine in a thread
void interactWithChessEngine()
{
    // At the chess engine's round
    while (true)
    {
        /**************************Thread Terminates**************************************/
        if (isGameOver) break;
        /*********************************************************************************/

        // Start interact with chess engine 
        if (!isAtUserRound && !enableMoveRend)
        {
            /*****************************Game Terminate Situation***************************/
            if (chessGame->doesCheck(!isUserWhite)) {
                std::cout << "Check Black King" << std::endl;
                if (chessGame->isDeadlyChecked(!isUserWhite)) {
                    std::cout << "***********************Game Result**********************" << std::endl;
                    std::cout << "Checkmate Black King" << std::endl;
                    std::cout << (!isUserWhite ? "You lose!!" : "Congratulation!! You win!!") << std::endl;
                    std::cout << "********************************************************" << std::endl;
                    std::cout << "Thanks for playing!!!" << std::endl;
                    std::cout << "Close Window to exit..." << std::endl;
                    isGameOver = true;
                    break;
                }
            }
            /*******************************************************************************/

            // Send history movement to Chess Engine
            if (!komodo->sendMove())
            {
                // Fail to send movement to Komodo
                std::cout << "Failed to send movement to Komodo" << std::endl;
                isGameOver = true;
                break;
            }

            // Get response movement from Chess Engine
            std::string responseMove;
            if (!komodo->getResponseMove(responseMove))
            {
                // Fail to get response movement from Komodo
                std::cout << "Failed to get movement to Komodo" << std::endl;
                isGameOver = true;
                break;
            }

            // Move chess is successful
            if (chessGame->moveChess(responseMove, !isUserWhite))
            {
                std::cout << "The Komodo's response: " << responseMove << std::endl;

                // Enable render update
                enableMoveRend = true;

                // Record engine's movement and turn to user
                komodo->addMovement(responseMove);
                isAtUserRound = true;  // Now it is user's round
            }
            else {
                // Komodo will moves invalid if it captures user's king
                std::cout << "***********************Game Result**********************" << std::endl;
                std::cout << "Checkmate " << (isUserWhite ? "White" : "Black") << " King!!!" << std::endl;
                std::cout << "You lose!!" << std::endl;
                std::cout << "********************************************************" << std::endl;
                std::cout << "Thanks for playing!!!" << std::endl;
                std::cout << "Close Window to exit..." << std::endl;
                isGameOver = true;
                break;
            }
        }
    }
}

// Sets up the chess board
void setupChessBoard(tModelMap& cTModelMap)
{
    // Target spec Hash
    cTModelMap =
    {
        // Chess board              Count  Angle      Axis             Scale                          Position (X, Y, Z)
        {"12951_Stone_Chess_Board_1", {1,   0.f,    {1, 0, 0},    glm::vec3(CBSCALE), {0.f,     0.f,                             PHEIGHT}}},
        // First player             Count  Angle      Axis             Scale                          Position (X, Y, Z)
        {"TORRE3_1",                  {2,   90.f,   {1, 0, 0},    glm::vec3(CPSCALE), {-3.5 * CHESS_BOX_SIZE, -3.5 * CHESS_BOX_SIZE, PHEIGHT}}},
        {"Object3_1",                 {2,   90.f,   {1, 0, 0},    glm::vec3(CPSCALE), {-2.5 * CHESS_BOX_SIZE, -3.5 * CHESS_BOX_SIZE, PHEIGHT}}},
        {"ALFIERE3_1",                {2,   90.f,   {1, 0, 0},    glm::vec3(CPSCALE), {-1.5 * CHESS_BOX_SIZE, -3.5 * CHESS_BOX_SIZE, PHEIGHT}}},
        {"REGINA2_1",                 {1,   90.f,   {1, 0, 0},    glm::vec3(CPSCALE), {-0.5 * CHESS_BOX_SIZE, -3.5 * CHESS_BOX_SIZE, PHEIGHT}}},
        {"RE2_1",                     {1,   90.f,   {1, 0, 0},    glm::vec3(CPSCALE), { 0.5 * CHESS_BOX_SIZE, -3.5 * CHESS_BOX_SIZE, PHEIGHT}}},
        {"TORRE3_2",                  {2,   90.f,   {1, 0, 0},    glm::vec3(CPSCALE), {3.5 * CHESS_BOX_SIZE, -3.5 * CHESS_BOX_SIZE, PHEIGHT}}},
        {"Object3_2",                 {2,   90.f,   {1, 0, 0},    glm::vec3(CPSCALE), {2.5 * CHESS_BOX_SIZE, -3.5 * CHESS_BOX_SIZE, PHEIGHT}}},
        {"ALFIERE3_2",                {2,   90.f,   {1, 0, 0},    glm::vec3(CPSCALE), {1.5 * CHESS_BOX_SIZE, -3.5 * CHESS_BOX_SIZE, PHEIGHT}}}
    };

    // Second player derived from first player!!
    // Second Player (TORRE02_1)
    cTModelMap["TORRE02_1"] = cTModelMap["TORRE3_1"]; cTModelMap["TORRE02_1"].tPos.y = -cTModelMap["TORRE3_1"].tPos.y;
    // Second Player (Object02_1)
    cTModelMap["Object02_1"] = cTModelMap["Object3_1"]; cTModelMap["Object02_1"].tPos.y = -cTModelMap["Object3_1"].tPos.y;
    // Second Player (ALFIERE02_1)
    cTModelMap["ALFIERE02_1"] = cTModelMap["ALFIERE3_1"]; cTModelMap["ALFIERE02_1"].tPos.y = -cTModelMap["ALFIERE3_1"].tPos.y;
    // Second Player (REGINA01_1)
    cTModelMap["REGINA01_1"] = cTModelMap["REGINA2_1"]; cTModelMap["REGINA01_1"].tPos.y = -cTModelMap["REGINA2_1"].tPos.y;
    // Second Player (RE01_1)
    cTModelMap["RE01_1"] = cTModelMap["RE2_1"]; cTModelMap["RE01_1"].tPos.y = -cTModelMap["RE2_1"].tPos.y;
    // Second Player (TORRE02_2)
    cTModelMap["TORRE02_2"] = cTModelMap["TORRE3_2"]; cTModelMap["TORRE02_2"].tPos.y = -cTModelMap["TORRE3_2"].tPos.y;
    // Second Player (Object02_2)
    cTModelMap["Object02_2"] = cTModelMap["Object3_2"]; cTModelMap["Object02_2"].tPos.y = -cTModelMap["Object3_2"].tPos.y;
    // Second Player (ALFIERE02_2)
    cTModelMap["ALFIERE02_2"] = cTModelMap["ALFIERE3_2"]; cTModelMap["ALFIERE02_2"].tPos.y = -cTModelMap["ALFIERE3_2"].tPos.y;

    // Add 16 pawns for both white and black
    for (int i = 0; i < 8; ++i) { // 8 pawns for each player
        std::string whitePawnID = "PEDONE13_" + std::to_string(i + 1);
        cTModelMap[whitePawnID] = { 8, 90.f, {1, 0, 0}, glm::vec3(CPSCALE),
                                   {-3.5 * CHESS_BOX_SIZE + i * CHESS_BOX_SIZE, -2.5 * CHESS_BOX_SIZE, PHEIGHT} };

        std::string blackPawnID = "PEDONE12_" + std::to_string(i + 1);
        cTModelMap[blackPawnID] = { 8, 90.f, {1, 0, 0}, glm::vec3(CPSCALE),
                                   {-3.5 * CHESS_BOX_SIZE + i * CHESS_BOX_SIZE, 2.5 * CHESS_BOX_SIZE, PHEIGHT} };
    }
}

// Update the chess board
void updateChessBoard(tModelMap& cTModelMap, DualMovement& rendMovement)
{
    static double lastTime = glfwGetTime();  // Store the last update time
    static float elapsedTime = 0.f;        // Track elapsed time for the movement

    // Once elapsedTime is rest, then reset lastTime as well
    if (elapsedTime == 0.f)
        lastTime = glfwGetTime();

    double currentTime = glfwGetTime();     // Get the current time
    float deltaTime = float(currentTime - lastTime);  // Compute time difference
    lastTime = currentTime;                 // Update last time
    elapsedTime += deltaTime;               // Update elapsed time

    // Interpolate source position
    std::string srcKeyStr = mapChessToModel(rendMovement.src.chess);
    cTModelMap[srcKeyStr].tPos = calculateIntermediatePosition(rendMovement.src, elapsedTime, false);

    // Interpolate destination position if capturing opponent chess
    std::string destKeyStr = mapChessToModel(rendMovement.dest.chess);
    // Only when the destination is a chess
    if (destKeyStr != "NoChess") {
        // Enable capture
        cTModelMap[destKeyStr].tPos = calculateIntermediatePosition(rendMovement.dest, elapsedTime, true);
    }

    // if movement is complete
    if (elapsedTime >= TOTAL_MOVE_TIME) {
        // Check whether the pawn promotion
        if (chessGame->checkPawnPromotion()) {
            DualChess pawnPromotion = chessGame->getPawnPromotion();
            updatePawnPromotion(cTModelMap, pawnPromotion);
        }

        // Reset elapsed time
        elapsedTime = 0.f;
        enableMoveRend = false;
    }
}

// Update pawn promotion
void updatePawnPromotion(tModelMap& cTModelMap, DualChess& pawnPromotion)
{    
    std::string pawnKeyStr = mapChessToModel(pawnPromotion.pawn);
    std::string promotionKeyStr = mapChessToModel(pawnPromotion.promotedChess);

    // Copy pawn's tPosition value to Promotion Chess
    cTModelMap[promotionKeyStr] = cTModelMap[pawnKeyStr];
    // Erase pawn from cTModelMap
    cTModelMap.erase(pawnKeyStr);

    // Add the promotion chess number and subtract pawn number
    std::string splitPromotionKeyStr = split(promotionKeyStr, '_')[0];
    std::string splitPawnKeyStr = split(pawnKeyStr, '_')[0];
    for (auto& map : cTModelMap) {
        if (map.first == splitPromotionKeyStr) {
            map.second.rCnt++;
        }
        if (map.first == splitPawnKeyStr) {
            map.second.rCnt--;
        }
    }
}

// Helper function to update the chess board 
vec3 calculateIntermediatePosition(Movement& rendMovement, float elapsedTime, bool isCapture)
{
    // Calculate interpolation factor (clamped between 0 and 1)
    float alpha = std::min(elapsedTime / TOTAL_MOVE_TIME, 1.f);

    // Extract start and end positions
    glm::vec3 start = {
        (float)(rendMovement.fromCol - 3.5) * CHESS_BOX_SIZE,
        (float)(rendMovement.fromRow - 3.5) * CHESS_BOX_SIZE,
        PHEIGHT
    };
    glm::vec3 end = {
        (float)(rendMovement.toCol - 3.5) * CHESS_BOX_SIZE,
        (float)(rendMovement.toRow - 3.5) * CHESS_BOX_SIZE,
        PHEIGHT
    };

    // Calculate interpolation factor (clamped between 0 and 1)
    glm::vec3 intermediate = start * (1 - alpha) + end * alpha;
    if (rendMovement.chess.cType == Knight || isCapture) {
        float zOffset = (float)(MAXIMUM_MOVEMENT_HEIGHT * (1 - std::pow(2 * alpha - 1, 2))); // h * (1 - (2 * alpha - 1))^2
        intermediate.z += zOffset;  // Apply the same parabolic offset
    }

    return intermediate;
}

// Covert Mapping from Chess to Model
std::string mapChessToModel(Chess chess)
{
    switch (chess.cType) 
    {
    case Pawn:
        return (chess.isWhite ? "PEDONE13_" : "PEDONE12_") + std::to_string(chess.sequence + 1);
    case Rook:
        return (chess.isWhite ? "TORRE3_" : "TORRE02_") + std::to_string(chess.sequence + 1);
    case Knight:
        return (chess.isWhite ? "Object3_" : "Object02_") + std::to_string(chess.sequence + 1);
    case Bishop:
        return (chess.isWhite ? "ALFIERE3_" : "ALFIERE02_") + std::to_string(chess.sequence + 1);
    case Queen:
        return (chess.isWhite ? "REGINA2_" : "REGINA01_") + std::to_string(chess.sequence + 1);
    case King:
        return (chess.isWhite ? "RE2_" : "RE01_") + std::to_string(chess.sequence + 1);
    default:
        return "NoChess";
    }
}