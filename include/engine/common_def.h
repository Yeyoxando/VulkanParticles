/*
 *	Author: Diego Ochando Torres
 *  Date: 25/11/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

#ifndef __COMMON_DEF_H__
#define __COMMON_DEF_H__

// ------------------------------------------------------------------------- // 

#include <vector>
#include <Vulkan/vulkan.h>

// ------------------------------------------------------------------------- // 

#pragma warning(disable: 26812)

// ------------------------------------------------------------------------- // 

/// @brief Enabled Vulkan validation layers, only enabled in Debug.
#ifdef DEBUG
const bool kEnableValidationLayers = true;
#else
const bool kEnableValidationLayers = false;
#endif

/// @brief Enabled Vulkan validation layers, only enabled in Debug.
const std::vector<const char*> kVkValidationLayers = {
	"VK_LAYER_KHRONOS_validation",
	"VK_LAYER_LUNARG_monitor"
};

/// @brief Vulkan extensions.
const std::vector<const char*> kDeviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

/// @brief Vulkan init variables.
const int MAX_FRAMES_IN_FLIGHT = 2;
const bool ENABLE_MSAA = true;

// ------------------------------------------------------------------------- // 

/// @brief returns a random float number between min and max provided values.
static float randFloat(float min, float max) {
	return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
}

// ------------------------------------------------------------------------- // 

#endif //__COMMON_DEF_H__