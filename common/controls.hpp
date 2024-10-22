/*
Author: Rui Wang
Class: ECE6122
Last Date Modified: Oct 22 2024
Description:
Update viewMatrix and ProjectionMatrix through input control and computation
*/

#ifndef CONTROLS_HPP
#define CONTROLS_HPP

void computeMatricesFromInputs();
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();
void computerMatricesFromInputsRadically(GLFWwindow* window, bool& lightingEnabled);

#endif