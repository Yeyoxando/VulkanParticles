/*
 *	Author: Diego Ochando Torres
 *  Date: 25/11/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

 // ------------------------------------------------------------------------- // 

#ifndef __HELLO_TRIANGLE_APP_H__
#define __HELLO_TRIANGLE_APP_H__

#include "GLFW/glfw3.h"

class HelloTriangleApp {
public:
  HelloTriangleApp();
  ~HelloTriangleApp();

	// Executes the application
	void run();

private:

  // GENERAL FUNCTIONS

  // Initializes GLFW and creates a window
  void initWindow(int width = 800, int height = 600);

	// Allocates all the necessary vulkan resources
	void initVulkan();

	// Executes the main render loop
	void renderLoop();

	// Frees all the allocated resources and close the app
	void close();

  // VULKAN FUNCTIONS
  void createInstance();

  // VARIABLES
  // Window variables
  GLFWwindow* window_;

  int window_width_;
  int window_height_;

  // Vulkan variables
  VkInstance instance_;

};

#endif // __HELLO_TRIANGLE_APP_H__