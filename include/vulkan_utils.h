/*
 *	Author: Diego Ochando Torres
 *  Date: 26/11/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

/*
 * Helper file to provide extra functionality
 */

#ifndef __VULKAN_UTILS_H__
#define __VULKAN_UTILS_H__

#include <Vulkan/vulkan.h>
#include <fstream>

// ------------------------------------------------------------------------- // 

// Extension function that is not loaded by default, so it has to be created specifically
static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
  const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
  const VkAllocationCallbacks* pAllocator,
  VkDebugUtilsMessengerEXT* pDebugMessenger) {

  // Get function from memory
  auto function = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

  if (function != nullptr) {
    return function(instance, pCreateInfo, pAllocator, pDebugMessenger);
  }
  else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }

}

// ------------------------------------------------------------------------- // 

// Extension function that is not loaded by default, so it has to be created specifically
static void DestroyDebugUtilsMessengerEXT(VkInstance instance, 
  VkDebugUtilsMessengerEXT debugMessenger, 
  const VkAllocationCallbacks* pAllocator) {

  auto function = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
  if (function != nullptr) {
    function(instance, debugMessenger, pAllocator);
  }

}

// ------------------------------------------------------------------------- // 

// Read files in binary format (Useful for spir-v shaders) (Would be good to do it with C file management (C++ sucks a bit))
static std::vector<char> readFile(const char* file_name) {

  std::ifstream file(file_name, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("Failed to open shader file.");
  }

  size_t file_size = (size_t)file.tellg();
  std::vector<char> buffer(file_size);

  file.seekg(0);
  file.read(buffer.data(), file_size);

  file.close();

  return buffer;

}

// ------------------------------------------------------------------------- // 

#endif // __VULKAN_UTILS_H__