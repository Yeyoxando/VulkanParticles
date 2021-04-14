/*
 *	Author: Diego Ochando Torres
 *  Date: 26/11/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

#ifndef __VULKAN_UTILS_H__
#define __VULKAN_UTILS_H__

 // ------------------------------------------------------------------------- // 

#include "common_def.h"

#include <GLFW/glfw3.h>
#include <Vulkan/vulkan.h>

#include <set>
#include <array>
#include <vector>
#include <chrono>
#include <fstream>
#include <optional>
#include <algorithm>

#include <hash.hpp>

// ------------------------------------------------------------------------- // 

#pragma warning(disable: 26812)

// ------------------------------------------------------------------------- // 
// ---------------------------- PHYSICAL DEVICE ---------------------------- // 
// ------------------------------------------------------------------------- // 

// 
struct QueueFamilyIndices {
	std::optional<uint32_t> graphics_family;
	std::optional<uint32_t> present_family;

	bool isComplete() {
		return graphics_family.has_value() && present_family.has_value();
	}
};

// ------------------------------------------------------------------------- //

// 
struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities{};
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> present_modes;
};

// ------------------------------------------------------------------------- // 

// Checks which queue families are supported on the device
static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {

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
		}

		// Check if present queue is supported
		VkBool32 present_support = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present_support);
		if (present_support) {
			indices.present_family = i;
		}

		// Break if both are found
		if (indices.isComplete()) {
			break;
		}
	}

	return indices;

}

// ------------------------------------------------------------------------- //

// Check if the extensions are supported on the device
static bool checkDeviceExtensionSupport(VkPhysicalDevice device) {

	// Get supported device properties
	uint32_t extension_count;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

	std::vector<VkExtensionProperties> available_extensions(extension_count);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());

	std::set<std::string> required_extensions(kDeviceExtensions.begin(), kDeviceExtensions.end());

	// Delete the extension from the required extension vector if they are available
	for (uint32_t i = 0; i < available_extensions.size(); i++) {
		required_extensions.erase(available_extensions[i].extensionName);
	}

	// If the required extensions vector was emptied it means that they are supported
	return required_extensions.empty();

}

// ------------------------------------------------------------------------- //

// Queries which swap chain details are supported on the device
static SwapChainSupportDetails querySwapChainSupportDetails(VkPhysicalDevice device, VkSurfaceKHR surface) {

	SwapChainSupportDetails details;

	// Request device supported capabilities
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	// Request device supported formats
	uint32_t formats_count;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formats_count, nullptr);
	if (formats_count != 0) {
		details.formats.resize(formats_count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formats_count, details.formats.data());
	}

	// Request device supported present modes
	uint32_t present_modes_count;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_modes_count, nullptr);
	if (present_modes_count != 0) {
		details.present_modes.resize(present_modes_count);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_modes_count, details.present_modes.data());
	}

	return details;

}

// ------------------------------------------------------------------------- //

// Check if the device fits with the necessary operations
static bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface) {

	// Basic device properties
	VkPhysicalDeviceProperties device_properties;
	vkGetPhysicalDeviceProperties(device, &device_properties);

	// Tex compression, 64-bit floats and multi-viewport rendering
	VkPhysicalDeviceFeatures device_features;
	vkGetPhysicalDeviceFeatures(device, &device_features);

	// Return false if a requested feature is not supported
	if (!device_features.samplerAnisotropy) {
		return false;
	}

	// Check if device supports graphic and present queues
	QueueFamilyIndices indices = findQueueFamilies(device, surface);

	// Check if the required extensions are supported
	bool extensions_supported = checkDeviceExtensionSupport(device);

	// Check if the swap chain details are adequate on the device
	bool adequate_swap_chain = false;
	if (extensions_supported) {
		SwapChainSupportDetails details = querySwapChainSupportDetails(device, surface);
		adequate_swap_chain = !details.formats.empty() && !details.present_modes.empty();
	}

	// Expand this function with necessary features

	return indices.isComplete() && extensions_supported && adequate_swap_chain;

}

// ------------------------------------------------------------------------- // 
// ---------------------------- DEBUG MESSENGER ---------------------------- // 
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

// Debug callback to interpret validation layers messages
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {

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

	pUserData;
	messageType;

	return VK_FALSE;

}

// ------------------------------------------------------------------------- //

// Fills the messenger info struct
static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& create_info) {

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
// -------------------------------- SHADERS -------------------------------- // 
// ------------------------------------------------------------------------- // 

// Read files in binary format (Spir-V shaders)
static std::vector<char> readFile(const char* file_name) {

	std::ifstream file(file_name, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("\nFailed to open shader file.");
	}

	size_t file_size = (size_t)file.tellg();
	std::vector<char> buffer(file_size);

	file.seekg(0);
	file.read(buffer.data(), file_size);

	file.close();

	return buffer;

}

// ------------------------------------------------------------------------- // 

// Create a shader module with the given bytecode
static VkShaderModule createShaderModule(VkDevice* logical_device, const std::vector<char>& bytecode) {

	VkShaderModuleCreateInfo create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	create_info.codeSize = bytecode.size();
	create_info.pCode = reinterpret_cast<const uint32_t*>(bytecode.data());

	VkShaderModule shader_module;

	if (vkCreateShaderModule(*logical_device, &create_info, nullptr, &shader_module) != VK_SUCCESS) {
		throw std::runtime_error("\nFailed to create the shader module.");
	}

	return shader_module;

}

// ------------------------------------------------------------------------- // 
// -------------------------------- MEMORY --------------------------------- // 
// ------------------------------------------------------------------------- // 

// Allocs aligned memory to use the dynamic ubo with the correct offset and alignment
// Windows specific
static void* alignedAlloc(size_t size, size_t alignment) {

	void* data = nullptr;
	data = _aligned_malloc(size, alignment);
	return data;

}

// ------------------------------------------------------------------------- // 

// Releases the aligned memory created by alignedAlloc
// Windows specific
static void alignedFree(void* data) {

	_aligned_free(data);

}

// ------------------------------------------------------------------------- // 

// Finds a valid memory type with the given parameters
static uint32_t findMemoryType(VkPhysicalDevice physical_device, uint32_t type_filter, VkMemoryPropertyFlags properties) {

	// Request supported memory properties from the graphics card
	VkPhysicalDeviceMemoryProperties mem_properties;
	vkGetPhysicalDeviceMemoryProperties(physical_device, &mem_properties);

	// Find a suitable memory type (Mask memory type bit and property flags)
	for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
		if (type_filter & (1 << i) &&
			(mem_properties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("\nFailed to find a suitable memory type.");

}

// ------------------------------------------------------------------------- // 
// ----------------------- VALIDATION AND EXTENSIONS ----------------------- // 
// ------------------------------------------------------------------------- // 

// Get GLFW required extensions, and the other defined ones
static std::vector<const char*> getRequiredExtensions() {

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

// Checks if the selected validation layers are supported
static bool checkValidationLayerSupport() {

	// Count available layers
	uint32_t layers_count = 0;
	vkEnumerateInstanceLayerProperties(&layers_count, nullptr);

	// Request available layers data
	std::vector<VkLayerProperties> layer_properties(layers_count);
	vkEnumerateInstanceLayerProperties(&layers_count, layer_properties.data());

	// Iterate over requested layers and check if they are available
	for (uint32_t i = 0; i < kVkValidationLayers.size(); ++i) {
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
// ------------------------------- SWAP CHAIN ------------------------------ //  
// ------------------------------------------------------------------------- // 

// Chooses the best swap chain surface formats from the available
static VkSurfaceFormatKHR chooseSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats) {

	// Search if any of the available formats has the desired features
	for (int i = 0; i < available_formats.size(); i++) {
		if (available_formats[i].format == VK_FORMAT_B8G8R8A8_SRGB &&
			available_formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {

			return available_formats[i];

		}
	}

	// If not fits we could tank the available formats to choose the better one
	// Just pick the first one in this case
	return available_formats[0];

}

// ------------------------------------------------------------------------- //

// Chooses the best swap chain surface present mode from the available
static VkPresentModeKHR chooseSwapChainPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes) {

	// Search if any of the available present modes are mailbox (triple buffering support)
	for (int i = 0; i < available_present_modes.size(); i++) {
		if (available_present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
			return available_present_modes[i];
		}
	}

	// Always available (double buffering)
	return VK_PRESENT_MODE_FIFO_KHR;

}

// ------------------------------------------------------------------------- //

// Choose a swap chain extent (resolution) with the device capabilities
static VkExtent2D chooseSwapChainExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities) {

	if (capabilities.currentExtent.width != UINT32_MAX) {
		// Some window managers set this by default, if is not equal to this it can be modified
		return capabilities.currentExtent;
	}
	else {
		// Choose best window resolution fit between the current and min/max image extent
		int width;
		int height;
		glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D window_extent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		window_extent.width = std::max<uint32_t>(capabilities.minImageExtent.width,
			std::min<uint32_t>(capabilities.maxImageExtent.width, window_extent.width));
		window_extent.height = std::max<uint32_t>(capabilities.minImageExtent.height,
			std::min<uint32_t>(capabilities.maxImageExtent.height, window_extent.height));

		return window_extent;
	}

}

// ------------------------------------------------------------------------- // 
// ----------------------------- FORMATS & MSAA ---------------------------- // 
// ------------------------------------------------------------------------- // 

// Find a supported image format within a list of candidates
static VkFormat findSupportedFormat(VkPhysicalDevice phys_device, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {

	for (uint32_t i = 0; i < candidates.size(); i++) {
		VkFormatProperties properties;
		vkGetPhysicalDeviceFormatProperties(phys_device, candidates[i], &properties);

		if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features) {
			return candidates[i];
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features) {
			return candidates[i];
		}
	}

	throw std::runtime_error("\nFailed to find a supported a format.");

}

// ------------------------------------------------------------------------- //

// Find a suitable depth format for the physical device
static VkFormat findDepthFormat(VkPhysicalDevice phys_device) {

	return findSupportedFormat(phys_device, { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

}

// ------------------------------------------------------------------------- //

// Return true if the selected depth format has a stencil component
static bool hasStencilComponent(VkFormat format) {

	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;

}

// ------------------------------------------------------------------------- //

// Get the maximum number of msaa samples supported by physical device
static VkSampleCountFlagBits getMaxUsableSampleCount(VkPhysicalDevice phys_device) {

	VkPhysicalDeviceProperties phys_device_properties;
	vkGetPhysicalDeviceProperties(phys_device, &phys_device_properties);

	VkSampleCountFlags counts = phys_device_properties.limits.framebufferColorSampleCounts &
		phys_device_properties.limits.framebufferDepthSampleCounts;

	if (!ENABLE_MSAA) return VK_SAMPLE_COUNT_1_BIT;

	if (counts & VK_SAMPLE_COUNT_64_BIT) return VK_SAMPLE_COUNT_64_BIT;
	if (counts & VK_SAMPLE_COUNT_32_BIT) return VK_SAMPLE_COUNT_32_BIT;
	if (counts & VK_SAMPLE_COUNT_16_BIT) return VK_SAMPLE_COUNT_16_BIT;
	if (counts & VK_SAMPLE_COUNT_8_BIT)  return VK_SAMPLE_COUNT_8_BIT;
	if (counts & VK_SAMPLE_COUNT_4_BIT)  return VK_SAMPLE_COUNT_4_BIT;
	if (counts & VK_SAMPLE_COUNT_2_BIT)  return VK_SAMPLE_COUNT_2_BIT;

	return VK_SAMPLE_COUNT_1_BIT;

}

// ------------------------------------------------------------------------- // 
// ------------------------------------------------------------------------- // 
// ------------------------------------------------------------------------- // 

// Structure representing all the vertices from the particle editor
struct Vertex {

	glm::vec3 position;
	glm::vec2 tex_coord;

	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription binding_desc{};
		binding_desc.binding = 0;
		binding_desc.stride = sizeof(Vertex);
		binding_desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return binding_desc;
	}

	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescription() {
		std::array<VkVertexInputAttributeDescription, 2> attribute_descs{};
		attribute_descs[0].binding = 0;
		attribute_descs[0].location = 0;
		attribute_descs[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute_descs[0].offset = offsetof(Vertex, position);

		attribute_descs[1].binding = 0;
		attribute_descs[1].location = 1;
		attribute_descs[1].format = VK_FORMAT_R32G32_SFLOAT;
		attribute_descs[1].offset = offsetof(Vertex, tex_coord);

		return attribute_descs;
	}

	bool operator ==(const Vertex& other) const {
		return position == other.position && tex_coord == other.tex_coord;
	}

};

// ------------------------------------------------------------------------- //

// Custom implementation for adding compatibility with Vertex struct and unordered maps
namespace std {
	template<> struct hash<Vertex> {
		size_t operator()(Vertex const& vertex) const {
			size_t pos_hash = hash<glm::vec3>()(vertex.position);
			size_t tex_coord_hash = hash<glm::vec2>()(vertex.tex_coord) << 1;
			return pos_hash ^ tex_coord_hash;
		}
	};
};

// ------------------------------------------------------------------------- // 
// ------------------------------------------------------------------------- // 
// ------------------------------------------------------------------------- // 

#endif // __VULKAN_UTILS_H__
