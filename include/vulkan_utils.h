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

// ------------------------------------------------------------------------- // 

// Extension function that is not loaded by default, so it has to be created specifically
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
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
void DestroyDebugUtilsMessengerEXT(VkInstance instance, 
  VkDebugUtilsMessengerEXT debugMessenger, 
  const VkAllocationCallbacks* pAllocator) {

  auto function = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
  if (function != nullptr) {
    function(instance, debugMessenger, pAllocator);
  }

}

// ------------------------------------------------------------------------- // 

#endif // __VULKAN_UTILS_H__