/*
 *	Author: Diego Ochando Torres
 *  Date: 25/11/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

// ------------------------------------------------------------------------- // 

#include "hello_triangle_app.h"
#include "vulkan_utils.h"

#include <stdexcept>
#include <cstring>
#include <vector>

// ------------------------------------------------------------------------- // 

HelloTriangleApp::HelloTriangleApp() {

  window_ = nullptr;
  window_width_ = 0;
  window_height_ = 0;

  instance_ = nullptr;
  debug_messenger_ = nullptr;

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

  if (kEnableValidationLayers) {
    setupDebugMessenger();
  }

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
  if (kEnableValidationLayers) {
    DestroyDebugUtilsMessengerEXT(instance_, debug_messenger_, nullptr);
  }

  vkDestroyInstance(instance_, nullptr);

  // Window close
  glfwDestroyWindow(window_);
  glfwTerminate();

}

// ------------------------------------------------------------------------- // 

void HelloTriangleApp::createInstance() {

  // Check for validation layers
  if (kEnableValidationLayers && !checkValidationLayerSupport()) {
    throw std::runtime_error("Requested validation layers are not available.");
  }

  // Custom application info
  VkApplicationInfo app_info{};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "Hello Triangle";
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.pEngineName = "No Engine";
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.apiVersion = VK_API_VERSION_1_0;

  auto extensions = getRequiredExtensions();

  // Instance create info
  VkInstanceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &app_info;
  create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  create_info.ppEnabledExtensionNames = extensions.data();
  // Specific for debug instance creation, destroyed in cleanup
  VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
  if (kEnableValidationLayers) {
    create_info.enabledLayerCount = static_cast<uint32_t>(kVkValidationLayers.size());
    create_info.ppEnabledLayerNames = kVkValidationLayers.data();
    
    // Enable debug messenger for the instance
    populateDebugMessengerCreateInfo(debug_create_info);
    create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debug_create_info;
  }
  else {
    create_info.enabledLayerCount = 0;
  }

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
  for (uint32_t i = 0; i < available_extensions; i++) {
    printf("\n\t. %s", extension_properties[i].extensionName);
  }

}

// ------------------------------------------------------------------------- // 

std::vector<const char*> HelloTriangleApp::getRequiredExtensions() {

  // Get supported extensions
  uint32_t glfw_extension_count = 0;
  const char** glfw_extensions;
  glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

  std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

  // Be sure to add EXT_DEBUG_UTILS if validation layers are enabled
  if (kEnableValidationLayers) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  return extensions;

}

// ------------------------------------------------------------------------- // 

bool HelloTriangleApp::checkValidationLayerSupport() {

  // Count available layers
  uint32_t layers_count = 0;
  vkEnumerateInstanceLayerProperties(&layers_count, nullptr);

  // Request available layers data
  std::vector<VkLayerProperties> layer_properties(layers_count);
  vkEnumerateInstanceLayerProperties(&layers_count, layer_properties.data());

  // Iterate over requested layers and check if they are available
  for (uint32_t i = 0; i < kVkValidationLayers.size(); ++i){
    bool found_layer = false;

    for (uint32_t l = 0; l < layers_count; ++l) {
      if (strcmp(kVkValidationLayers[i], layer_properties[l].layerName) == 0) {
        found_layer = true;
        break;
      }
    }

    if (!found_layer) {
      return false;
    }
  }

  return true;

}

// ------------------------------------------------------------------------- // 

void HelloTriangleApp::setupDebugMessenger() {

  if (!kEnableValidationLayers) return;

  VkDebugUtilsMessengerCreateInfoEXT messenger_info;
  populateDebugMessengerCreateInfo(messenger_info);

  if (CreateDebugUtilsMessengerEXT(instance_, &messenger_info, nullptr, &debug_messenger_) != VK_SUCCESS) {
    throw std::runtime_error("Debug Messenger creation has failed.");
  }

}

// ------------------------------------------------------------------------- // 

void HelloTriangleApp::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& create_info) {
  
  create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  create_info.pfnUserCallback = debugCallback;
  create_info.pUserData = nullptr;

}

// ------------------------------------------------------------------------- // 

VKAPI_ATTR VkBool32 VKAPI_CALL HelloTriangleApp::debugCallback(
  VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, 
  VkDebugUtilsMessageTypeFlagsEXT messageType, 
  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, 
  void* pUserData) {

  // Not finished, it could show the information of other lot of things
  // it could even show it trough a log in the app, which can be divided in different message sections

  if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
    //printf("\nValidation layer (VERBOSE): %s.", pCallbackData->pMessage);
  }
  if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
    //printf("\nValidation layer (INFO): %s.", pCallbackData->pMessage);
  }
  if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    printf("\nValidation layer (WARNING): %s.", pCallbackData->pMessage);
  }
  if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
    printf("\nValidation layer (ERROR): %s.", pCallbackData->pMessage);
  }

  return VK_FALSE;

}

// ------------------------------------------------------------------------- // 


