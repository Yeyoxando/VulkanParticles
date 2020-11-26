/*
 *	Author: Diego Ochando Torres
 *  Date: 25/11/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

 // ------------------------------------------------------------------------- // 

#ifndef __HELLO_TRIANGLE_APP_H__
#define __HELLO_TRIANGLE_APP_H__

#include "common_def.h"
#include <GLFW/glfw3.h>

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
  // Creates a Vulkan instance and checks for extensions support
  void createInstance();
  // Get GLFW required extensions
  std::vector<const char*> getRequiredExtensions();
  // Checks if the selected validation layers are supported
  bool checkValidationLayerSupport();
  // Creates a custom debug messenger
  void setupDebugMessenger();
  // Fills the messenger info struct
  void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& create_info);
  // Debug callback to interpret validation layers messages
  static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData);

  // VARIABLES
  // Window variables
  GLFWwindow* window_;

  int window_width_;
  int window_height_;

  // Vulkan variables
  VkInstance instance_;
  VkDebugUtilsMessengerEXT debug_messenger_;

};

#endif // __HELLO_TRIANGLE_APP_H__