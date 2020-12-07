/*
 *	Author: Diego Ochando Torres
 *  Date: 25/11/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

#ifndef __COMMON_DEF_H__
#define __COMMON_DEF_H__

#include <vector>
#include <Vulkan/vulkan.h>

// ------------------------------------------------------------------------- // 

// Comment and uncomment those defines to select which main test will be executed

//#define MAIN_WINDOW_TEST
//#define MAIN_HELLO_TRIANGLE
#define MAIN_BILLBOARDS

// ------------------------------------------------------------------------- // 

// Vulkan validation layers
const std::vector<const char*> kVkValidationLayers = {
  "VK_LAYER_KHRONOS_validation",
  "VK_LAYER_LUNARG_monitor"
};

#ifdef DEBUG
const bool kEnableValidationLayers = true;
#else
const bool kEnableValidationLayers = false;
#endif

// Vulkan extensions
const std::vector<const char*> kDeviceExtensions = {
  VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

// Vulkan runtime
const int MAX_FRAMES_IN_FLIGHT = 2;

// ------------------------------------------------------------------------- // 

#endif //__COMMON_DEF_H__