/*
 *	Author: Diego Ochando Torres
 *  Date: 25/11/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

// ------------------------------------------------------------------------- // 

#include "hello_triangle_app.h"
#include <stdexcept>
#include <vector>

// ------------------------------------------------------------------------- // 

HelloTriangleApp::HelloTriangleApp() {

  window_ = nullptr;
  window_width_ = 0;
  window_height_ = 0;

  instance_ = nullptr;

}

// ------------------------------------------------------------------------- // 

HelloTriangleApp::~HelloTriangleApp() {

}

// ------------------------------------------------------------------------- // 

void HelloTriangleApp::run() {

  initWindow();

  initVulkan();
  renderLoop();
  close();

}

// ------------------------------------------------------------------------- // 

void HelloTriangleApp::initWindow(int width /*= 800*/, int height /*= 600*/) {

  glfwInit();

  // Tell GLFW to not use OpenGL API
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  // No resize allowed for now
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window_ = glfwCreateWindow(width, height, "Hello Triangle", nullptr, nullptr);
  window_width_ = width;
  window_height_ = height;

}

// ------------------------------------------------------------------------- // 

void HelloTriangleApp::initVulkan() {

  createInstance();

}

// ------------------------------------------------------------------------- // 

void HelloTriangleApp::renderLoop() {

  while (!glfwWindowShouldClose(window_)) {
    
    glfwPollEvents();

    // Draw things

  }

}

// ------------------------------------------------------------------------- // 

void HelloTriangleApp::close() {

  // Vulkan cleanup
  vkDestroyInstance(instance_, nullptr);

  // Window close
  glfwDestroyWindow(window_);
  glfwTerminate();

}

// ------------------------------------------------------------------------- // 

void HelloTriangleApp::createInstance() {

  // Custom application info
  VkApplicationInfo app_info{};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "Hello Triangle";
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.pEngineName = "No Engine";
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.apiVersion = VK_API_VERSION_1_0;

  // Get supported extensions
  uint32_t glfw_extension_count = 0;
  const char** glfw_extensions;
  glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

  // Instance create info
  VkInstanceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &app_info;
  create_info.enabledExtensionCount = glfw_extension_count;
  create_info.ppEnabledExtensionNames = glfw_extensions;
  create_info.enabledLayerCount = 0;

  // Create the instance
  if (vkCreateInstance(&create_info, nullptr, &instance_) != VK_SUCCESS) {
    throw std::runtime_error("Vulkan instance creation failed.");
  }

  // Report available vulkan extensions
  uint32_t available_extensions = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &available_extensions, nullptr);
  std::vector<VkExtensionProperties> extension_properties(available_extensions);

  vkEnumerateInstanceExtensionProperties(nullptr, &available_extensions, extension_properties.data());

  printf("Available Vulkan extensions:");
  for (int i = 0; i < available_extensions; i++) {
    printf("\n\t. %s", extension_properties[i].extensionName);
  }

}

// ------------------------------------------------------------------------- // 


