/*
Author: Rui Wang
Class: ECE6122
Last Date Modified: Oct 22 2024
Description:
This is a altered version of origin controls in the tutorial.
A new function here, dealing with lightEnabled toggle, uses spherical coordinate system.
*/

// Include GLFW
#include <GLFW/glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.


// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "controls.hpp"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}

// Initial position : on +Z
glm::vec3 position = glm::vec3( 0, 0, 5 ); 
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.005f;

/* Initial distance from the origin*/
float radius = 5.0f;

void computeMatricesFromInputs(){

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse position for next frame
	glfwSetCursorPos(window, 1024/2, 768/2);

	// Compute new orientation
	horizontalAngle += mouseSpeed * float(1024/2 - xpos );
	verticalAngle   += mouseSpeed * float( 768/2 - ypos );

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle), 
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);
	
	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f/2.0f), 
		0,
		cos(horizontalAngle - 3.14f/2.0f)
	);
	
	// Up vector
	glm::vec3 up = glm::cross( right, direction );

	// Move forward
	if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
		position += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
		position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS){
		position -= right * deltaTime * speed;
	}

	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	ViewMatrix       = glm::lookAt(
								position,           // Camera is here
								position+direction, // and looks here : at the same position, plus "direction"
								up                  // Head is up (set to 0,-1,0 to look upside-down)
						   );

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}

void computerMatricesFromInputsRadically(GLFWwindow* window, bool& lightingEnabled) {
	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();
	// Track the previous state of the 'L' key
	static bool lKeyPressedLastFrame = false;

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Movement parameters
	float radiusSpeed = 3.0f; // Speed for radial movement
	float angularSpeed = 1.0f; // Speed for rotational movement

	// Radial distance change (using w and s keys)
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		radius -= deltaTime * radiusSpeed;
		if (radius < 0.1f) // Prevent the camera from going too close to the origin
			radius = 0.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		radius += deltaTime * radiusSpeed;
	}

	// Horizontal rotation (using a and d keys)
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		horizontalAngle -= deltaTime * angularSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		horizontalAngle += deltaTime * angularSpeed;
	}

	// Vertical rotation (using arrow keys)
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		verticalAngle += deltaTime * angularSpeed;
		if (verticalAngle > glm::radians(179.0f)) // Prevent going over the pole
			verticalAngle = glm::radians(179.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		verticalAngle -= deltaTime * angularSpeed;
		if (verticalAngle < glm::radians(-179.0f)) // Prevent going under the pole
			verticalAngle = glm::radians(-179.0f);
	}

	// Check if the 'L' key is pressed
	bool lKeyPressedThisFrame = glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS;

	if (lKeyPressedThisFrame && !lKeyPressedLastFrame) {
		// Toggle lighting
		lightingEnabled = !lightingEnabled;
	}

	// Spherical to Cartesian conversion
	position = glm::vec3(
		radius * cos(verticalAngle) * sin(horizontalAngle),
		radius * sin(verticalAngle),
		radius * cos(verticalAngle) * cos(horizontalAngle)
	);

	// Direction vector: from the camera position towards the origin
	glm::vec3 direction = -position; // Looking towards the origin
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f)
	);

	// Down vector
	glm::vec3 up = -glm::cross(right, direction);

	// Projection matrix: 45 degrees Field of View, 4:3 ratio, display range: 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(glm::radians(initialFoV), 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	ViewMatrix = glm::lookAt(
		position,             // Camera is here
		glm::vec3(0, 0, 0),   // and looks towards the origin
		up                    // Head is up
	);

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;

	// Update the last frame's state
	lKeyPressedLastFrame = lKeyPressedThisFrame;
}