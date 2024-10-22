/*
Author: Rui Wang
Class: ECE6122
Last Date Modified: Oct 22 2024
Description:
This is the main function, including loadOBJ, initialize chesses and chessboard, and renderation.
*/

// Include standard headers
#include <iostream>
#include <string>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/controls.hpp>

// Include our loading methods
#include "LoadObj.h"

int main( void )
{
    // Initialize GLFW and create a window
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make macOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //This is very stupid problem here, because using extern
    GLFWwindow* window = glfwCreateWindow(800, 600, "Chess Render", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to open GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Enable the mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 800 / 2, 600 / 2);

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // Enable depth test for proper 3D rendering
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Disable face culling to see all faces
    glDisable(GL_CULL_FACE);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Load shaders
    GLuint programID = LoadShaders("StandardShading.vertexshader", "StandardShading.fragmentshader");

    // Get a handle for our MVP uniform
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
    GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

    // Get a handle for our "LightPosition" uniform
    glUseProgram(programID);
    GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

    // Get uniform locations for the new material properties
    GLuint KaID = glGetUniformLocation(programID, "Ka");
    GLuint KdID = glGetUniformLocation(programID, "Kd");
    GLuint KsID = glGetUniformLocation(programID, "Ks");
    GLuint NsID = glGetUniformLocation(programID, "Ns");

    // Get the location of the "lightingEnabled" uniform in the shader
    GLuint LightingEnabledID = glGetUniformLocation(programID, "lightingEnabled");

    // Texture uniform location
    GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");

    // Vectors to store ObjectData, Materials, and GLuint textures
    std::vector<ObjectData> objectDataList;

    // Load ObjectData
    loadObjectData("chess.obj", "./Chess/", objectDataList);
    loadObjectData("12951_Stone_Chess_Board_v1_L3.obj", "./Stone_Chess_Board/", objectDataList);

    // Know let's loading the 18 chesses and 1 chessboard
    std::vector<Object> chesses;
    Object chessBoard;
    initializeChesses(chesses, objectDataList);
    initializeChessBoard(chessBoard, objectDataList[objectDataList.size() - 1]);

    // initial status is toggle on
    bool lightingEnabled = false;

    // For speed computation
    double lastTime = glfwGetTime();
    int nbFrames = 0;

    // Main rendering loop
    do {

        // Measure speed
        double currentTime = glfwGetTime();
        nbFrames++;
        if (currentTime - lastTime >= 1.0) { // If last print was more than 1sec ago
            // print and reset
            std::cout << 1000.0 / double(nbFrames) << " ms/frame" << std::endl;
            nbFrames = 0;
            lastTime += 1.0;
        }

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID);

        // Compute the MVP matrix from keyboard and mouse input
        computerMatricesFromInputsRadically(window, lightingEnabled);
        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();

        // Set the light position in the world space
        glm::vec3 lightPos = glm::vec3(0, 0, 0);
        glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

        // Set the uniform for lighting enable/disable
        glUniform1i(LightingEnabledID, lightingEnabled ? 1 : 0);

        /************** Render ChessBoard firstly **************/
        glm::mat4 ModelMatrix = chessBoard.transformMatrix;
        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

        // Send our transformation to the currently bound shader, in the "MVP" uniform
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

        Material& chessBoardMaterial = chessBoard.objData.material;
        // Set the material properties in the shader
        glUniform3f(KaID, chessBoardMaterial.Ka.x, chessBoardMaterial.Ka.y, chessBoardMaterial.Ka.z);
        glUniform3f(KdID, chessBoardMaterial.Kd.x, chessBoardMaterial.Kd.y, chessBoardMaterial.Kd.z);
        glUniform3f(KsID, chessBoardMaterial.Ks.x, chessBoardMaterial.Ks.y, chessBoardMaterial.Ks.z);
        glUniform1f(NsID, chessBoardMaterial.Ns);

        // Bind the texture for the current object's material
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, chessBoardMaterial.textureID);
        glUniform1i(TextureID, 0);

        ObjectData& chessBoardObjectData = chessBoard.objData;
        // Bind the vertex buffer
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, chessBoardObjectData.vertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // Bind the UV buffer
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, chessBoardObjectData.uvbuffer);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // Bind the normal buffer
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, chessBoardObjectData.normalbuffer);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // Bind the index buffer and draw the object
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chessBoardObjectData.elementbuffer);
        glDrawElements(GL_TRIANGLES, chessBoardObjectData.indices.size(), GL_UNSIGNED_SHORT, (void*)0);


        /************** Render Chesses Secondly **************/
        // Loop over all chesses and render them
        for (int i = 0; i < chesses.size(); i++) {
            Object& chess = chesses[i];
            glm::mat4 ModelMatrix = chess.transformMatrix;
            glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

            // Send our transformation to the currently bound shader, in the "MVP" uniform
            glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

            Material& chessMaterial = chess.objData.material;
            // Set the material properties in the shader
            glUniform3f(KaID, chessMaterial.Ka.x, chessMaterial.Ka.y, chessMaterial.Ka.z);
            glUniform3f(KdID, chessMaterial.Kd.x, chessMaterial.Kd.y, chessMaterial.Kd.z);
            glUniform3f(KsID, chessMaterial.Ks.x, chessMaterial.Ks.y, chessMaterial.Ks.z);
            glUniform1f(NsID, chessMaterial.Ns);

            // Bind the texture for the current object's material
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, chessMaterial.textureID);
            glUniform1i(TextureID, 0);

            ObjectData& chessData = chess.objData;
            // Bind the vertex buffer
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, chessData.vertexbuffer);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

            // Bind the UV buffer
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, chessData.uvbuffer);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

            // Bind the normal buffer
            glEnableVertexAttribArray(2);
            glBindBuffer(GL_ARRAY_BUFFER, chessData.normalbuffer);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

            // Bind the index buffer and draw the object
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chessData.elementbuffer);
            glDrawElements(GL_TRIANGLES, chessData.indices.size(), GL_UNSIGNED_SHORT, (void*)0);
        }

        // Disable the attribute arrays
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&  // Check if the ESC key was pressed or the window was closed
       glfwWindowShouldClose(window) == 0);

    // Cleanup and exit (same as before)
    for (auto& obj : objectDataList) {
        glDeleteTextures(1, &obj.material.textureID);
        glDeleteBuffers(1, &obj.vertexbuffer);
        glDeleteBuffers(1, &obj.uvbuffer);
        glDeleteBuffers(1, &obj.normalbuffer);
        glDeleteBuffers(1, &obj.elementbuffer);
    }

    // Delete the shader program
    glDeleteProgram(programID);
    glDeleteVertexArrays(1, &VertexArrayID);

    glfwTerminate();
    return 0;
}
