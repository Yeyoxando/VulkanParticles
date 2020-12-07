/*
 *	Author: Diego Ochando Torres
 *  Date: 05/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

 // ------------------------------------------------------------------------- // 

#ifndef __BILLBOARDS_APP_H__
#define __BILLBOARDS_APP_H__

#include "common_def.h"
#include <GLFW/glfw3.h>
#include <optional>
#include <array>
#include <glm/glm.hpp>

struct QueueFamilyIndices {
  std::optional<uint32_t> graphics_family;
  std::optional<uint32_t> present_family;

  bool isComplete() {
    return graphics_family.has_value() && present_family.has_value();
  }
};

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> present_modes;
};

struct Vertex {
  glm::vec2 position;
  glm::vec3 color;

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
    attribute_descs[0].format = VK_FORMAT_R32G32_SFLOAT;
    attribute_descs[0].offset = offsetof(Vertex, position);
    attribute_descs[1].binding = 0;
    attribute_descs[1].location = 1;
    attribute_descs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attribute_descs[1].offset = offsetof(Vertex, color);

    return attribute_descs;
  }
};

class BillboardsApp {
public:
  BillboardsApp();
  ~BillboardsApp();

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
  // Creates a surface handle for the current window
  void createSurface();
  // Select a physical device that supports the requested features
  void pickPhysicalDevice();
  // Check if the device fits with the necessary operations
  bool isDeviceSuitable(VkPhysicalDevice device);
  // Check if the extensions are supported on the device
  bool checkDeviceExtensionSupport(VkPhysicalDevice device);
  // Checks which queue families are supported on the device
  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
  // Queries which swap chain details are supported on the device
  SwapChainSupportDetails querySwapChainSupportDetails(VkPhysicalDevice device);
  // Chooses the best swap chain surface formats from the available
  VkSurfaceFormatKHR chooseSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats);
  // Chooses the best swap chain surface present mode from the available
  VkPresentModeKHR  chooseSwapChainPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes);
  // Choose a swap chain extent (resolution) with the device capabilities
  VkExtent2D chooseSwapChainExtent(const VkSurfaceCapabilitiesKHR& capabilities);
  // Creates a logical device for the selected physical device
  void createLogicalDevice();
  // Creates the swap chain
  void createSwapChain();
  // Create the image views for he swap chain images
  void createImageViews();
  // Creates the render pass for the graphics pipeline
  void createRenderPass();
  // Creates a default graphic pipeline for opaque objects with vertex and fragment shaders
  void createGraphicsPipeline();
  // Create a shader module with the given bytecode
  VkShaderModule createShaderModule(const std::vector<char>& bytecode);
  // Creates the framebuffers used for rendering
  void createFramebuffers();
  // Creates a command pool for manage the memory of the command buffers 
  void createCommandPool();
  // Creates the vertex buffers for the app
  void createVertexBuffers();
  // Creates the command buffers for each swap chain framebuffer
  void createCommandBuffers();
  // Creates the semaphores needed for rendering
  void createSyncObjects();

  // Draw using the recorded command buffers
  void drawFrame();

  // Recreate the swap chain to make it compatible with the current requirements
  void recreateSwapChain();
  // Cleans all the swapchain objects
  void cleanupSwapChain();
  // GLFW callback for window resize
  static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

  uint32_t findMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties);

  // VARIABLES
  // Window variables
  GLFWwindow* window_;

  int window_width_;
  int window_height_;

  // Vulkan variables
  VkInstance instance_;
  VkDebugUtilsMessengerEXT debug_messenger_;
  VkPhysicalDevice physical_device_; // GPU
  VkDevice logical_device_;
  VkQueue graphics_queue_;
  VkQueue present_queue_;
  VkSurfaceKHR surface_; //Abstract window
  VkSwapchainKHR swap_chain_;
  std::vector<VkImage> swap_chain_images_;
  VkFormat swap_chain_image_format_;
  VkExtent2D swap_chain_extent_;
  std::vector<VkImageView> swap_chain_image_views_;
  VkRenderPass render_pass_;
  VkPipelineLayout pipeline_layout_;
  VkPipeline  graphics_pipeline_;
  std::vector<VkFramebuffer> swap_chain_framebuffers_;
  VkCommandPool command_pool_;
  std::vector<VkCommandBuffer> command_buffers_;
  std::vector<VkSemaphore> available_image_semaphores_;
  std::vector<VkSemaphore> finished_render_semaphores_;
  std::vector<VkFence> in_flight_fences_;
  std::vector<VkFence> images_in_flight_;
  int current_frame_;
  bool resized_framebuffer_;

  std::vector<Vertex> vertices_;
  VkBuffer vertex_buffer_;
  VkDeviceMemory vertex_buffer_memory_;

};

#endif // __HELLO_TRIANGLE_APP_H__