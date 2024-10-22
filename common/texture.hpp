/*
Author: OpenGL.org
Class: ECE6122
Last Date Modified: Oct 17 2024
Description:
This function comes from the official document, and is used here to load texture
*/

#ifndef TEXTURE_HPP
#define TEXTURE_HPP

// Load a .BMP file using our custom loader
GLuint loadBMP_custom(const char * imagepath);

//// Since GLFW 3, glfwLoadTexture2D() has been removed. You have to use another texture loading library, 
//// or do it yourself (just like loadBMP_custom and loadDDS)
//// Load a .TGA file using GLFW's own loader
//GLuint loadTGA_glfw(const char * imagepath);

// Load a .DDS file using GLFW's own loader
GLuint loadDDS(const char * imagepath);

#endif