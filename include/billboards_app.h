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
#include <glm/gtx/hash.hpp>

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

  glm::vec3 position;
  glm::vec3 color;
  glm::vec2 tex_coord;

  static VkVertexInputBindingDescription getBindingDescription() {
    VkVertexInputBindingDescription binding_desc{};
    binding_desc.binding = 0;
    binding_desc.stride = sizeof(Vertex);
    binding_desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return binding_desc;
  }

  static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescription() {
    std::array<VkVertexInputAttributeDescription, 3> attribute_descs{};
    attribute_descs[0].binding = 0;
    attribute_descs[0].location = 0;
    attribute_descs[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attribute_descs[0].offset = offsetof(Vertex, position);

    attribute_descs[1].binding = 0;
    attribute_descs[1].location = 1;
    attribute_descs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attribute_descs[1].offset = offsetof(Vertex, color);

    attribute_descs[2].binding = 0;
    attribute_descs[2].location = 2;
    attribute_descs[2].format = VK_FORMAT_R32G32_SFLOAT;
    attribute_descs[2].offset = offsetof(Vertex, tex_coord);

    return attribute_descs;
  }

  bool operator ==(const Vertex& other) const {
    return position == other.position && color == other.color && tex_coord == other.tex_coord;
  }

};

// Custom implementation for adding compatibility with Vertex struct and unordered maps
namespace std {
  template<> struct hash<Vertex> {
    size_t operator()(Vertex const& vertex) const {
      return ((hash<glm::vec3>()(vertex.position) ^
        (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
        (hash<glm::vec2>()(vertex.tex_coord) << 1);
    }
  };
};

// Example for Uniforms for the shaders
struct UniformBufferObject {
  glm::mat4x4 model;
  glm::mat4x4 view;
  glm::mat4x4 projection;

  /*
   * Scalars have to be aligned by N (= 4 bytes given 32 bit floats).
   * A vec2 must be aligned by 2N (= 8 bytes)
   * A vec3 or vec4 must be aligned by 4N (= 16 bytes)
   * A nested structure must be aligned by the base alignment of its members rounded up to a multiple of 16.
   * A mat4 matrix must have the same alignment as a vec4.
   * alignas(bytes) could be used to fix this but better nope
   */

  /*
   * For binding multiple descriptors, for example per object or shared descriptors
   * look at the final section of the Vulkan tutorial chapter 'descriptor pool and sets'
   */

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
  // create a image view resource
  VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspect_flags);
  // Create the image views for he swap chain images
  void createImageViews();
  // Creates the render pass for the graphics pipeline
  void createRenderPass();
  // Creates the descriptor layout to upload uniforms to the shader
  void createDescriptorSetLayout();
  // Creates a default graphic pipeline for opaque objects with vertex and fragment shaders
  void createGraphicsPipeline();
  // Create a shader module with the given bytecode
  VkShaderModule createShaderModule(const std::vector<char>& bytecode);
  // Creates the framebuffers used for rendering
  void createFramebuffers();
  // Creates a command pool for manage the memory of the command buffers 
  void createCommandPool();
  // Creates the depth resources for depth testing
  void createDepthResources();
  // Creates an image from a texture
  void createTextureImage(const char* texture_path);
  // Creates the image view for the texture
  void createTextureImageView();
  // Creates the sampler for texture filtering and shader reading
  void createTextureSampler();
  // Creates a vulkan image
  void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
    VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& image_memory);
  // Creates a buffer and allocate its memory
  void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, 
    VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& memory);
  // Loads a OBJ model
  void loadModel(const char* model_path);
  // Creates the vertex buffers for the app and map their memory to the GPU
  void createVertexBuffers();
  // Creates the index buffers for the app and map their memory to the GPU
  void createIndexBuffers();
  // Creates the uniform buffers for the app
  void createUniformBuffers();
  // Creates a descriptor pool to allocate the descriptor sets for the uniforms
  void createDescriptorPool();
  // Creates the descriptor sets for the uniform buffers
  void createDescriptorSets();
  // Copy a buffer from the cpu to the device local memory through a staging buffer
  void copyBuffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);
  // Begin a list of single time commands and return it
  VkCommandBuffer beginSingleTimeCommands();
  // end the list of single time commands
  void endSingleTimeCommands(VkCommandBuffer cmd_buffer);
  // Handles layout transitions
  void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout);
  // Copies a buffer into an image
  void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
  // Creates the command buffers for each swap chain framebuffer
  void createCommandBuffers();
  // Creates the semaphores needed for rendering
  void createSyncObjects();

  // Updates the uniform buffers and map their memory
  void updateUniformBuffers(uint32_t current_image);
  // Draw using the recorded command buffers
  void drawFrame();

  // Recreate the swap chain to make it compatible with the current requirements
  void recreateSwapChain();
  // Cleans all the swapchain objects
  void cleanupSwapChain();
  // GLFW callback for window resize
  static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

  // Find the memory type for the given type filter
  uint32_t findMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties);
  // Find a supported image format with a list of candidates
  VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
  // Find a depth format
  VkFormat findDepthFormat();
  // Return true if the selected depth format has a stencil component
  bool hasStencilComponent(VkFormat format);

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
  VkDescriptorSetLayout descriptor_set_layout_;
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
  std::vector<uint32_t> indices_;
  VkBuffer vertex_buffer_;
  VkDeviceMemory vertex_buffer_memory_;
  VkBuffer index_buffer_;
  VkDeviceMemory index_buffer_memory_;
  std::vector<VkBuffer> uniform_buffers_;
  std::vector<VkDeviceMemory> uniform_buffers_memory_;
  VkDescriptorPool descriptor_pool_;
  std::vector<VkDescriptorSet> descriptor_sets_;
  VkImage texture_image_;
  VkDeviceMemory texture_image_memory_;
  VkImageView texture_image_view_;
  VkSampler texture_sampler_;
  VkImage depth_image_;
  VkDeviceMemory depth_image_memory_;
  VkImageView depth_image_view_;

};

#endif // __BILLBOARDS_APP_H__