/*
Author: Rui Wang
Class: ECE6122
Last Date Modified: Dec 1st, 2024

Description:
Add a final project control in this file to deal with the light power,
light position, and camera position from keyboard. There are also additional
get methods to get controllers and use them to control rending object in shaders.
*/

#ifndef CONTROLS_HPP
#define CONTROLS_HPP

#include <iostream>
#include <vector>
#include <sstream>

void computeMatricesFromInputs();
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();
bool getLightSwitch();
const float getLightPower();
const glm::vec3 getLightPosition();

// A custom function for Lab3
// Creates the view and Projection matrix based on following custom key definitions
// keyboard inputs definitions
//1) ‘w?key moves the camera radially closer to the origin.
//2) ‘s?key moves the camera radially farther from the origin.
//3) ‘a?key rotates the camera to the left maintaining the radial distance from the origin.
//4) ‘d?key rotates to camera to the right maintaining the radial distance from the origin.
//5) The up arrow key radially rotates the camera up.
//6) The down arrow radially rotates the camera down.
//7) The ‘L?key toggles the specular and diffuse components of the light on and off but leaves the ambient component unchanged.
//8) Pressing the escape key closes the window and exits the program

void computeMatricesFromInputsLab3();

// Final project methods
bool commandControl(std::vector<std::string> splitedCmd);

#endif