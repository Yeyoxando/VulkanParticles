/*
 *	Author: Diego Ochando Torres
 *  Date: 07/01/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

#ifndef __INTERNAL_APP_DATA_H__
#define __INTERNAL_APP_DATA_H__

// ------------------------------------------------------------------------- //

#include "common_def.h"
#include "vulkan_utils.h"
#include "systems/system_draw_objects.h"
#include "../src/internal/internal_gpu_resources.h"

#include <GLFW/glfw3.h>

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// ------------------------------------------------------------------------- //

struct SceneUBO {
	glm::mat4 view;
	glm::mat4 projection;
};

struct ModelsUBO {
  glm::mat4* models = nullptr;
};

struct OpaqueUBO {
  glm::mat4* packed_uniforms = nullptr;
};
/*
struct TranslucentUBO{
	glm::vec4 color;
	// Textures should be bind on these sets
}

// Special UBO for the lights, as they are the same for all objects
struct LightsUBO{
	// Will contain a limited number of lights data
  // For the moment start with one directional in the scene
}
*/

// ------------------------------------------------------------------------- //

// All data used in the application, including Vulkan instance and resources
// Some Vulkan helper functionalities are extracted in "vulkan_utils.h" and "internal_gpu_resources.h"
struct ParticleEditor::AppData {

// --------------- VARIABLES ---------------

  // ----- WINDOW -----
  GLFWwindow* window_;
  int window_width_;
	int window_height_;
	bool resized_framebuffer_;
	bool close_window_;


  // ----- VULKAN -----
  VkInstance instance_;
  VkDebugUtilsMessengerEXT debug_messenger_;
  VkPhysicalDevice physical_device_; // GPU
  VkDevice logical_device_;

  VkQueue graphics_queue_;
  VkQueue present_queue_;
  VkSurfaceKHR surface_; //Abstract window
  VkSwapchainKHR swap_chain_;
  VkFormat swap_chain_image_format_;
  VkExtent2D swap_chain_extent_;
	std::vector<Image*> swap_chain_images_;
	Image* depth_image_;
	Image* color_image_;
  std::vector<VkFramebuffer> swap_chain_framebuffers_;
  VkRenderPass render_pass_;
  VkCommandPool command_pool_;
  std::vector<VkCommandBuffer> command_buffers_;
  std::vector<VkSemaphore> available_image_semaphores_;
  std::vector<VkSemaphore> finished_render_semaphores_;
  std::vector<VkFence> in_flight_fences_;
  std::vector<VkFence> images_in_flight_;
	VkSampleCountFlagBits msaa_samples_;
	int current_frame_;


	// ----- SYSTEMS (ECS) -----
	SystemDrawObjects* system_draw_objects_;



  // ----- RESOURCES -----
  const int default_geometries = 1;
  std::vector<Buffer*> vertex_buffers_;
  std::vector<Buffer*> index_buffers_;
	std::map<int, const char*> loaded_models_;

  std::vector<Material*> materials_;
  std::vector<Image*> texture_images_;
	std::map<int, const char*> loaded_textures_;

  // ----- SPECIFIC UBOS SETTINGS-----
  // - Scene UBO -
	VkDescriptorSetLayout scene_descriptor_set_layout_; 
	VkDescriptorPool scene_descriptor_pool_; 
	std::vector<VkDescriptorSet> scene_descriptor_sets_; // one per swap chain image.
	std::vector<Buffer*> scene_uniform_buffers_; // one per swap chain image.
	SceneUBO scene_ubo_;

  // - Models UBO -
	VkDescriptorSetLayout models_descriptor_set_layout_;
	VkDescriptorPool models_descriptor_pool_;
	std::vector<VkDescriptorSet> models_descriptor_sets_; // one per swap chain image.
	std::vector<Buffer*> models_uniform_buffers_; // one per swap chain image.
  ModelsUBO models_ubo_;

	// - Opaque UBO -
	VkDescriptorSetLayout opaque_descriptor_set_layout_;
	VkDescriptorPool opaque_descriptor_pool_;
	std::vector<VkDescriptorSet> opaque_descriptor_sets_; // one per swap chain image.
	std::vector<Buffer*> opaque_uniform_buffers_; // one per swap chain image.
	OpaqueUBO opaque_ubo_;

  // TranslucentUBO/Particles


// --------------- METHODS ---------------

  // ----- Constructor -----
  AppData();
  ~AppData();


  // ----- Init and close -----
  // Initializes GLFW and creates a window
  void initWindow(int width = 800, int height = 600);
  // Allocates all the necessary Vulkan resources
  void initVulkan();
  // Waits for the device to finish, call it after render loop finishes
  void renderLoopEnd();
  // Frees all the allocated resources and close the app
  void closeVulkan();


  // ----- Vulkan setup -----
  // Creates a Vulkan instance and checks for extensions support
  void createInstance();
	// Creates a custom debug messenger
	void setupDebugMessenger();
	// Creates a surface handle for the current window
  void createSurface();
  // Select a physical device that supports the requested features
  void pickPhysicalDevice();
  // Creates a logical device for the selected physical device
  void createLogicalDevice();
  // Creates the swap chain
  void createSwapChain();
  // Creates the render pass for the graphics pipeline
  void createRenderPass();
  // Creates the frame buffers used for rendering
  void createFramebuffers();
  // Creates a command pool for manage the memory of the command buffers 
  void createCommandPool();
  // Creates the color framebuffer resources for multi-sampling
  void createColorResources();
  // Creates the depth resources for depth testing
  void createDepthResources();
  // Creates the vertex buffers for the app and map their memory to the GPU
  void createVertexBuffer(std::vector<Vertex>& vertices);
  // Creates the index buffers for the app and map their memory to the GPU
  void createIndexBuffer(std::vector<uint32_t>& indices);
  // Creates the command buffers for each swap chain framebuffer
  void createCommandBuffers();
  // Creates the semaphores needed for rendering
  void createSyncObjects();


  // ----- Frame -----
  // Updates frame logic
  void updateFrame();
  // Updates the uniform buffers and map their memory
  void updateUniformBuffers(uint32_t current_image);
  // Draw using the recorded command buffers
  void drawFrame();


  // ----- Swap chain recreation -----
  // Recreate the swap chain to make it compatible with the current requirements
  void recreateSwapChain();
  // Cleans all the swapchain objects
  void cleanupSwapChain();
  // GLFW callback for window resize
  static void framebufferResizeCallback(GLFWwindow* window, int width, int height);


  // ----- Helper functions -----
	// Create a shader module with the given bytecode
	VkShaderModule createShaderModule(const std::vector<char>& bytecode);
	// Handles layout transitions
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout);
 
  
	// ----- Internal structure (Resource manager) -----
	// - MESHES -
	// Loads all the requested OBJ models
	void loadModels();
	// Creates the vertex buffers using the internal geometries and loaded models
	void setupVertexBuffers();
	// Creates the index buffers using the internal geometries and loaded models
	void setupIndexBuffers();

	// - MATERIALS / TEXTURES -
	// Creates the internal material parents and set their values
	void setupMaterials();
	// Creates all the texture images marked for load
	void createTextureImages();

  // - DESCRIPTOR SETS AND BUFFERS -
	// Creates the descriptor layout to upload uniforms to the shader
	void createDescriptorSetLayouts();
	// Creates the pipeline layout for the different graphics pipelines
	void createPipelineLayouts();
	// Creates a default graphic pipeline for opaque objects with vertex and fragment shaders
	void createGraphicsPipelines();
	// Creates a descriptor pool to allocate the descriptor sets for the uniforms
	void createDescriptorPools();
  // Initialize uniform buffers and descriptor sets 
  void initializeDescriptorSets();
  
  // Creates the uniform buffers for a material
	void createSceneUniformBuffers(std::vector<Buffer*>& buffers);
	// Creates the uniform dynamic buffers for a material
	void createModelDynamicUniformBuffers(std::vector<Buffer*>& buffers);
	// Creates the uniform dynamic buffers for a opaque material
	void createOpaqueDynamicUniformBuffers(std::vector<Buffer*>& buffers);
	// Updates the scene uniform buffer
	void updateUniformBuffer(SceneUBO ubo, Buffer* buffer);
	// Updates the object models buffer
	void updateUniformBuffer(ModelsUBO ubo, int objects, Buffer* buffer);
	// Updates a uniform opaque buffer
	void updateUniformBuffer(OpaqueUBO ubo, int objects, Buffer* buffer);
	// Updates a uniform translucent buffer
	//void updateUniformBuffer(TranslucentUBO ubo, Buffer* buffer);
  // Clean up the uniform buffers independent from their origin
  void cleanUniformBuffers(std::vector<Buffer*>& buffers_);

  // Populates the descriptor set for the scene
  void populateSceneDescriptorSets();
  // Populates the descriptor set for the objects models
	void populateModelsDescriptorSets();
	// Populates the descriptor set for the objects opaque pipeline uniforms and textures
	void populateOpaqueDescriptorSets();

};

#endif // __INTERNAL_APP_DATA_H__