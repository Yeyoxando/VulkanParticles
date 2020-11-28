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

  instance_ = VK_NULL_HANDLE;
  debug_messenger_ = VK_NULL_HANDLE;
  physical_device_ = VK_NULL_HANDLE;

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
  setupDebugMessenger();
  pickPhysicalDevice();
  createLogicalDevice();

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
  vkDestroyDevice(logical_device_, nullptr);
  
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

void HelloTriangleApp::pickPhysicalDevice() {

  // Request available physical devices (graphics cards)
  uint32_t device_count = 0;
  vkEnumeratePhysicalDevices(instance_, &device_count, nullptr);
  
  if (device_count == 0) {
    throw std::runtime_error("The device does not have any GPUs supporting Vulkan.");
  }

  std::vector<VkPhysicalDevice> devices(device_count);
  vkEnumeratePhysicalDevices(instance_, &device_count, devices.data());

  for (uint32_t i = 0; i < devices.size(); i++) {
    if (isDeviceSuitable(devices[i])) {
      physical_device_ = devices[i];
      break;
    }
  }

  if (physical_device_ == VK_NULL_HANDLE) {
    throw std::runtime_error("The device's GPUs are not suitable for the requested functionality.");
  }

}

// ------------------------------------------------------------------------- // 

bool HelloTriangleApp::isDeviceSuitable(VkPhysicalDevice device) {

  // Basic device properties
  VkPhysicalDeviceProperties device_properties;
  vkGetPhysicalDeviceProperties(device, &device_properties);

  // Tex compression, 64-bit floats and multi-viewport rendering
  VkPhysicalDeviceFeatures device_features;
  vkGetPhysicalDeviceFeatures(device, &device_features);

  // Example if card don't support geometry shader
  /*if (!device_features.geometryShader) {
    return false;
  }*/

  // Check if device supports graphic queues
  QueueFamilyIndices indices = findQueueFamilies(device);
  if (!indices.graphics_family.has_value()) {
    return false;
  }

  // Expand this function with necessary features

  return true;

}

// ------------------------------------------------------------------------- // 

QueueFamilyIndices HelloTriangleApp::findQueueFamilies(VkPhysicalDevice device) {
  
  QueueFamilyIndices indices;

  // Get device family queue properties
  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

  std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

  for (uint32_t i = 0; i < queue_families.size(); i++) {
    // Check if graphics queue is supported
    if (queue_families[i].queueFlags && VK_QUEUE_GRAPHICS_BIT) {
      indices.graphics_family = i;
      break;
    }
  }

  return indices;

}

// ------------------------------------------------------------------------- // 

void HelloTriangleApp::createLogicalDevice() {

  // Create needed queues
  QueueFamilyIndices indices = findQueueFamilies(physical_device_);
  float queue_priority = 1.0f;

  VkDeviceQueueCreateInfo queue_create_info{};
  queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queue_create_info.queueFamilyIndex = indices.graphics_family.value();
  queue_create_info.queueCount = 1;
  queue_create_info.pQueuePriorities = &queue_priority;

  // Set needed vulkan features
  VkPhysicalDeviceFeatures device_features{};

  // Create the device
  VkDeviceCreateInfo device_create_info{};
  device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  device_create_info.pQueueCreateInfos = &queue_create_info;
  device_create_info.queueCreateInfoCount = 1;
  device_create_info.pEnabledFeatures = &device_features;
  device_create_info.enabledExtensionCount = 0;

  // Only for older Vulkan apps, layers don't need to be set in the device on actual versions
  if (kEnableValidationLayers) {
    device_create_info.enabledLayerCount = static_cast<uint32_t>(kVkValidationLayers.size());
    device_create_info.ppEnabledLayerNames = kVkValidationLayers.data();
  }
  else {
    device_create_info.enabledLayerCount = 0;
  }

  if (vkCreateDevice(physical_device_, &device_create_info, nullptr, &logical_device_) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create Vulkan logical device.");
  }

  // Store queue handle
  vkGetDeviceQueue(logical_device_, indices.graphics_family.value(), 0, &graphics_queue_);

}

// ------------------------------------------------------------------------- // 


