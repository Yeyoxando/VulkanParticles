/*
 *	Author: Diego Ochando Torres
 *  Date: 07/01/2021
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

// ------------------------------------------------------------------------- //

#include "particle_editor.h"
#include "../src/engine_internal/internal_app_data.h"

#include <stdexcept>

#include <tiny_obj_loader.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Provisional
#include "components/component_transform.h"

// ----------------------- Functions definition ---------------------------- //

ParticleEditor::AppData::AppData() {

  window_ = nullptr;
  window_width_ = 0;
  window_height_ = 0;

  instance_ = VK_NULL_HANDLE;
  debug_messenger_ = VK_NULL_HANDLE;
  physical_device_ = VK_NULL_HANDLE;
  logical_device_ = VK_NULL_HANDLE;
  graphics_queue_ = VK_NULL_HANDLE;
  present_queue_ = VK_NULL_HANDLE;
  surface_ = VK_NULL_HANDLE;
  swap_chain_ = VK_NULL_HANDLE;
  swap_chain_extent_ = {};
  render_pass_ = VK_NULL_HANDLE;

  vertex_buffers_ = std::vector<Buffer*>(0);
  index_buffers_ = std::vector<Buffer*>(0);

  materials_ = std::vector<Material*>(0);
  texture_images_ = std::vector<Image*>(0);

  depth_image_ = nullptr;
  color_image_ = nullptr;

  msaa_samples_ = VK_SAMPLE_COUNT_1_BIT;

  current_frame_ = 0;
  resized_framebuffer_ = false;
  close_window_ = false;

  system_draw_objects_ = new SystemDrawObjects();
  system_draw_translucents_ = new SystemDrawTranslucents();
  system_draw_particles_ = new SystemDrawParticles();

}

// ------------------------------------------------------------------------- //

ParticleEditor::AppData::~AppData(){

  delete system_draw_objects_;
  delete system_draw_translucents_;
  delete system_draw_particles_;

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::initWindow(int width /*= 800*/, int height /*= 600*/) {

  glfwInit();

  // Tell GLFW to not use OpenGL API
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

  window_ = glfwCreateWindow(width, height, "Particle System Editor", nullptr, nullptr);
  window_width_ = width;
  window_height_ = height;

  glfwSetWindowUserPointer(window_, this);
  glfwSetFramebufferSizeCallback(window_, framebufferResizeCallback);
  glfwSetInputMode(window_, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::initVulkan() {

  createInstance();
  setupDebugMessenger();
  createSurface();
  pickPhysicalDevice();
  createLogicalDevice();
  createSwapChain();
  createRenderPass();
  setupMaterials();
  for (int i = 0; i < materials_.size(); ++i) {
    if (window_width_ == 0 || window_height_ == 0) break;
    materials_[i]->createDescriptorSetLayout();
    materials_[i]->createPipelineLayout();
    materials_[i]->createGraphicPipeline();
  }
  createCommandPool();
  createColorResources();
  createDepthResources();
  createFramebuffers();
  createTextureImages();
  setupVertexBuffers();
	setupIndexBuffers();
	loadModels();
  for (int i = 0; i < materials_.size(); ++i) {
    if (window_width_ == 0 || window_height_ == 0) break;
    materials_[i]->createDescriptorPools();
    materials_[i]->initDescriptorSets();
  }
  createCommandBuffers();
  createSyncObjects();

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::renderLoopEnd() {

  // Wait until the device is on idle so we can clean up resources without being in use
  vkDeviceWaitIdle(logical_device_);

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::closeVulkan() {

  // Vulkan cleanup
  cleanupSwapChain();

  for (int i = 0; i < texture_images_.size(); i++) {
    texture_images_[i]->clean(logical_device_);
    delete texture_images_[i];
  }

  for (int i = 0; i < materials_.size(); i++) {
    materials_[i]->deleteMaterialResources();
    delete materials_[i];
  }

  for (int i = 0; i < index_buffers_.size(); i++) {
    index_buffers_[i]->clean(logical_device_);
    delete index_buffers_[i];
  }

  for (int i = 0; i < vertex_buffers_.size(); i++) {
    vertex_buffers_[i]->clean(logical_device_);
    delete vertex_buffers_[i];
  }

  for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroySemaphore(logical_device_, available_image_semaphores_[i], nullptr);
    vkDestroySemaphore(logical_device_, finished_render_semaphores_[i], nullptr);
    vkDestroyFence(logical_device_, in_flight_fences_[i], nullptr);
  }

  vkDestroyCommandPool(logical_device_, command_pool_, nullptr);

  vkDestroyDevice(logical_device_, nullptr);

  vkDestroySurfaceKHR(instance_, surface_, nullptr);

  if (kEnableValidationLayers) {
    DestroyDebugUtilsMessengerEXT(instance_, debug_messenger_, nullptr);
  }

  vkDestroyInstance(instance_, nullptr);

  // Window close
  glfwDestroyWindow(window_);
  glfwTerminate();

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::createInstance() {

  // Check for validation layers
  if (kEnableValidationLayers && !checkValidationLayerSupport()) {
    throw std::runtime_error("\nRequested validation layers are not available.");
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
    create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_create_info;
  }
  else {
    create_info.enabledLayerCount = 0;
  }

  // Create the instance
  if (vkCreateInstance(&create_info, nullptr, &instance_) != VK_SUCCESS) {
    throw std::runtime_error("\nVulkan instance creation failed.");
  }

  // Report available Vulkan extensions
  uint32_t available_extensions = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &available_extensions, nullptr);

  std::vector<VkExtensionProperties> extension_properties(available_extensions);
  vkEnumerateInstanceExtensionProperties(nullptr, &available_extensions, extension_properties.data());

  printf("\nAvailable Vulkan extensions:");
  for (uint32_t i = 0; i < available_extensions; i++) {
    printf("\n\t. %s", extension_properties[i].extensionName);
  }

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::createSurface() {

  if (glfwCreateWindowSurface(instance_, window_, nullptr, &surface_) != VK_SUCCESS) {
    throw std::runtime_error("\nFailed to create window surface.");
  }

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::pickPhysicalDevice() {

  // Request available physical devices (graphics cards)
  uint32_t device_count = 0;
  vkEnumeratePhysicalDevices(instance_, &device_count, nullptr);

  if (device_count == 0) {
    throw std::runtime_error("\nThe device does not have any GPUs supporting Vulkan.");
  }

  std::vector<VkPhysicalDevice> devices(device_count);
  vkEnumeratePhysicalDevices(instance_, &device_count, devices.data());

  for (uint32_t i = 0; i < devices.size(); i++) {
    if (isDeviceSuitable(devices[i], surface_)) {
      physical_device_ = devices[i];
      msaa_samples_ = getMaxUsableSampleCount(physical_device_);
      break;
    }
  }

  if (physical_device_ == VK_NULL_HANDLE) {
    throw std::runtime_error("\nThe device's GPUs are not suitable for the requested functionality.");
  }

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::createLogicalDevice() {

  // Create needed queues
  QueueFamilyIndices indices = findQueueFamilies(physical_device_, surface_);
  float queue_priority = 1.0f;

  // Store unique queues
  std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
  std::set<uint32_t> unique_queue_families = { indices.graphics_family.value(),
    indices.present_family.value() };

  // Create unique queues create info
  for (uint32_t queue_family : unique_queue_families) {
    VkDeviceQueueCreateInfo queue_create_info{};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = queue_family;
    queue_create_info.queueCount = 1;
    queue_create_info.pQueuePriorities = &queue_priority;
    queue_create_infos.push_back(queue_create_info);
  }

  // Set needed Vulkan features
  VkPhysicalDeviceFeatures device_features{};
  device_features.samplerAnisotropy = VK_TRUE;

  // Create the device
  VkDeviceCreateInfo device_create_info{};
  device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  device_create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
  device_create_info.pQueueCreateInfos = queue_create_infos.data();
  device_create_info.pEnabledFeatures = &device_features;
  device_create_info.enabledExtensionCount = static_cast<uint32_t>(kDeviceExtensions.size());
  device_create_info.ppEnabledExtensionNames = kDeviceExtensions.data();

  // Only for older Vulkan apps, layers don't need to be set in the device on actual versions
  if (kEnableValidationLayers) {
    device_create_info.enabledLayerCount = static_cast<uint32_t>(kVkValidationLayers.size());
    device_create_info.ppEnabledLayerNames = kVkValidationLayers.data();
  }
  else {
    device_create_info.enabledLayerCount = 0;
  }

  if (vkCreateDevice(physical_device_, &device_create_info, nullptr, &logical_device_) != VK_SUCCESS) {
    throw std::runtime_error("\nFailed to create Vulkan logical device.");
  }

  // Store graphics queue handle
  vkGetDeviceQueue(logical_device_, indices.graphics_family.value(), 0, &graphics_queue_);

  // Store present queue handle
  vkGetDeviceQueue(logical_device_, indices.present_family.value(), 0, &present_queue_);

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::createSwapChain() {

  if (window_width_ == 0 || window_height_ == 0) return;

  // Choose preferred details
  SwapChainSupportDetails details = querySwapChainSupportDetails(physical_device_, surface_);

  VkSurfaceFormatKHR format = chooseSwapChainSurfaceFormat(details.formats);
  VkPresentModeKHR present_mode = chooseSwapChainPresentMode(details.present_modes);
  VkExtent2D extent = chooseSwapChainExtent(window_, details.capabilities);

  // Request one more image than the min to prevent waiting for them
  uint32_t image_count = details.capabilities.minImageCount + 1;
  // But not exceeding the max number (0 indicates no constraints)
  if (details.capabilities.maxImageCount > 0 && image_count > details.capabilities.maxImageCount) {
    image_count = details.capabilities.maxImageCount;
  }

  // Create info structure
  VkSwapchainCreateInfoKHR create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  create_info.surface = surface_;
  create_info.minImageCount = image_count;
  create_info.imageFormat = format.format;
  create_info.imageColorSpace = format.colorSpace;
  create_info.imageExtent = extent;
  create_info.imageArrayLayers = 1;
  create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;//VK_IMAGE_USAGE_TRANSFER_DST_BIT for rendering to another buffer i.e.

  QueueFamilyIndices indices = findQueueFamilies(physical_device_, surface_);
  uint32_t queue_family_indices[] = { indices.graphics_family.value(), indices.present_family.value() };
  if (indices.graphics_family != indices.present_family) {
    // Used in multiple queues
    create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    create_info.queueFamilyIndexCount = 2; // Indicate number of shared families
    create_info.pQueueFamilyIndices = queue_family_indices; // Shared families
  }
  else {
    // Used exclusively in one queue
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 0; // Optional
    create_info.pQueueFamilyIndices = nullptr; // Optional
  }

  // No image modification when presenting
  create_info.preTransform = details.capabilities.currentTransform;
  // No alpha with other windows
  create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

  create_info.presentMode = present_mode;
  create_info.clipped = VK_TRUE;
  create_info.oldSwapchain = VK_NULL_HANDLE; // Set later when resizing or something like that

  if (vkCreateSwapchainKHR(logical_device_, &create_info, nullptr, &swap_chain_) != VK_SUCCESS) {
    throw std::runtime_error("\nFailed to create swap chain.");
  }

  // Store image handles for render operation
  vkGetSwapchainImagesKHR(logical_device_, swap_chain_, &image_count, nullptr);
  swap_chain_images_.resize(image_count);
  std::vector<VkImage> tmp_images;
  tmp_images.resize(image_count);
  vkGetSwapchainImagesKHR(logical_device_, swap_chain_, &image_count, tmp_images.data());
  for (uint32_t i = 0; i < image_count; i++) {
    swap_chain_images_[i] = new Image(Image::kImageType_Framebuffer);
    swap_chain_images_[i]->image_ = tmp_images[i];
  }

  // Store format and extent for future operations
  swap_chain_image_format_ = format.format;
  swap_chain_extent_ = extent;

  // Create the image views
  for (int i = 0; i < swap_chain_images_.size(); i++) {
    swap_chain_images_[i]->createImageView(logical_device_, swap_chain_image_format_, VK_IMAGE_ASPECT_COLOR_BIT);
  }

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::createRenderPass() {

  if (window_width_ == 0 || window_height_ == 0) return;

  // How to load and store the received info for color
  VkAttachmentDescription color_attachment{};
  color_attachment.format = swap_chain_image_format_;
  color_attachment.samples = msaa_samples_;
  // Color and depth data
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  // Stencil data
  color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference color_attachment_ref{};
  color_attachment_ref.attachment = 0;
  color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  // How to load and store the received info for depth
  VkAttachmentDescription depth_attachment{};
  depth_attachment.format = findDepthFormat(physical_device_);
  depth_attachment.samples = msaa_samples_;
  depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depth_attachment_ref{};
  depth_attachment_ref.attachment = 1;
  depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  // Color attachment transformation to present after msaa
  VkAttachmentDescription color_attachment_resolve{};
  color_attachment_resolve.format = swap_chain_image_format_;
  color_attachment_resolve.samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment_resolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment_resolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment_resolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment_resolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment_resolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment_resolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference color_attachment_resolve_ref{};
  color_attachment_resolve_ref.attachment = 2;
  color_attachment_resolve_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  // Sub passes (only 1 for now)
  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &color_attachment_ref; // layout location = 0 in frag shader
  subpass.pDepthStencilAttachment = &depth_attachment_ref;
  subpass.pResolveAttachments = &color_attachment_resolve_ref;

  // Dependency for the render pass to start (Until color attachment is finished)
  VkSubpassDependency subpass_dependency{};
  subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  subpass_dependency.dstSubpass = 0;
  subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
    VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  subpass_dependency.srcAccessMask = 0;
  subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
    VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
    VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

  // Create the render pass
  std::array<VkAttachmentDescription, 3> attachments = { color_attachment, depth_attachment, color_attachment_resolve };
  VkRenderPassCreateInfo render_pass_info{};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_info.attachmentCount = static_cast<uint32_t>(attachments.size());
  render_pass_info.pAttachments = attachments.data();
  render_pass_info.subpassCount = 1;
  render_pass_info.pSubpasses = &subpass;
  render_pass_info.dependencyCount = 1;
  render_pass_info.pDependencies = &subpass_dependency;

  if (vkCreateRenderPass(logical_device_, &render_pass_info, nullptr, &render_pass_) != VK_SUCCESS) {
    throw std::runtime_error("\nFailed to create the render pass");
  }

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::createFramebuffers() {

  if (window_width_ == 0 || window_height_ == 0) return;

  swap_chain_framebuffers_.resize(swap_chain_images_.size());

  for (int i = 0; i < swap_chain_images_.size(); i++) {
    std::array<VkImageView, 3> attachments = {
      color_image_->image_view_,
      depth_image_->image_view_,
      swap_chain_images_[i]->image_view_,
    };

    VkFramebufferCreateInfo framebuffer_info{};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = render_pass_;
    framebuffer_info.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebuffer_info.pAttachments = attachments.data();
    framebuffer_info.width = swap_chain_extent_.width;
    framebuffer_info.height = swap_chain_extent_.height;
    framebuffer_info.layers = 1;

    if (vkCreateFramebuffer(logical_device_, &framebuffer_info, nullptr, &swap_chain_framebuffers_[i]) != VK_SUCCESS) {
      throw std::runtime_error("\nFailed to create framebuffer.");
    }
  }

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::createCommandPool() {

  QueueFamilyIndices indices = findQueueFamilies(physical_device_, surface_);

  VkCommandPoolCreateInfo command_pool_info{};
  command_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  command_pool_info.queueFamilyIndex = indices.graphics_family.value();
  command_pool_info.flags = 0; // VK_COMMAND_POOL_CREATE_TRANSIENT_BIT means that command buffers will be rerecorded with new commands very often

  if (vkCreateCommandPool(logical_device_, &command_pool_info, nullptr, &command_pool_) != VK_SUCCESS) {
    throw std::runtime_error("\nFailed to create command pool.");
  }

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::createColorResources() {

  VkFormat format = swap_chain_image_format_;

  color_image_ = new Image(Image::kImageType_Framebuffer);
  color_image_->create(physical_device_, logical_device_, swap_chain_extent_.width,
    swap_chain_extent_.height, msaa_samples_, format, VK_IMAGE_TILING_OPTIMAL,
    VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  color_image_->createImageView(logical_device_, format, VK_IMAGE_ASPECT_COLOR_BIT);
  
}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::createDepthResources() {

  VkFormat format = findDepthFormat(physical_device_);

  depth_image_ = new Image(Image::kImageType_Framebuffer);

  depth_image_->create(physical_device_, logical_device_, swap_chain_extent_.width,
    swap_chain_extent_.height, msaa_samples_, format, VK_IMAGE_TILING_OPTIMAL,
    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  depth_image_->createImageView(logical_device_, format, VK_IMAGE_ASPECT_DEPTH_BIT);
  
  transitionImageLayout(depth_image_->image_, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::createTextureImages() {

  int tex_width;
  int tex_height;
  int tex_channels;
  stbi_uc* pixels;
  VkDeviceSize image_size;

	auto it = loaded_textures_.cbegin();
  while (it != loaded_textures_.cend()) {
    Image* texture_image = new Image(Image::kImageType_Texture);

    // Load any texture with 4 channels with STBI_rgb_alpha
    pixels = stbi_load(it->second,
      &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);

    image_size = tex_width * tex_height * 4;

    if (!pixels) {
      throw std::runtime_error("\nFailed to load texture.");
    }

    // Create a staging buffer to transfer it to the device memory
    Buffer* staging_buffer = new Buffer(Buffer::kBufferType_Image);
    staging_buffer->create(physical_device_, logical_device_, image_size,
      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    // Copy values to the buffer
    void* data;
    vkMapMemory(logical_device_, staging_buffer->buffer_memory_, 0, image_size, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(image_size));
    vkUnmapMemory(logical_device_, staging_buffer->buffer_memory_);

    stbi_image_free(pixels);

    // Create the texture image
    texture_image->create(physical_device_, logical_device_, tex_width, tex_height, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB,
      VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  
    transitionImageLayout(texture_image->image_, VK_FORMAT_R8G8B8A8_SRGB,
      VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    texture_image->copyFromBuffer(logical_device_, command_pool_, graphics_queue_, *staging_buffer,
      static_cast<uint32_t>(tex_width), static_cast<uint32_t>(tex_height));
    transitionImageLayout(texture_image->image_, VK_FORMAT_R8G8B8A8_SRGB,
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    staging_buffer->clean(logical_device_);
    delete staging_buffer;

    texture_image->createImageView(logical_device_, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
    texture_image->createSampler(physical_device_, logical_device_);

    texture_images_.push_back(texture_image);

    ++it;
	}

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::loadModels() {

	// Load model from file
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

  auto it = loaded_models_.cbegin();
	while (it != loaded_models_.cend()) {

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

    const char* model_path = it->second;
		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, model_path)) {
			throw std::runtime_error(warn + err);
		}

		// Create the model with indices
		std::unordered_map<Vertex, uint32_t> unique_vertices{};

		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				Vertex vertex{};

				vertex.position = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};

				vertex.tex_coord = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
				};

				if (unique_vertices.count(vertex) == 0) {
					unique_vertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}

				indices.push_back(unique_vertices[vertex]);
			}
		}

		createVertexBuffer(vertices);
		createIndexBuffer(indices);

    ++it;
  }

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::createVertexBuffer(std::vector<Vertex> &vertices) {

  // Driver developers says that buffers should be created in only one VkBuffer
  // and use them with the offset properties that are in the functions
  // this will allow cache optimizations known as aliasing
	vertex_buffers_.push_back(new Buffer(Buffer::kBufferType_Vertex));

  // Create a staging buffer and allocate its memory
  VkDeviceSize buffer_size = sizeof(vertices[0]) * vertices.size();
  Buffer* staging_buffer = new Buffer(Buffer::kBufferType_Vertex);
  staging_buffer->create(physical_device_, logical_device_, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  // Map the memory from the CPU to GPU
  void* data;
  vkMapMemory(logical_device_, staging_buffer->buffer_memory_, 0, buffer_size, 0, &data);
  memcpy(data, vertices.data(), (size_t)buffer_size);
  vkUnmapMemory(logical_device_, staging_buffer->buffer_memory_);

  // The memory is not copied instantly, but it should be before the next call to vkQueueSubmit
  // This could cause performance decreases and it could be done with a host coherent memory heap
  // or using vkFlushMappedMemoryRanges after writing to mapped memory and vkInvalidateMappedMemoryRanges before reading


  // Create the actual vertex buffer
  vertex_buffers_[vertex_buffers_.size() - 1]->create(physical_device_, logical_device_, buffer_size,
    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  // Copy the content from the staging buffer to the vertex buffer (allocated in gpu memory)
  vertex_buffers_[vertex_buffers_.size() - 1]->copy(logical_device_, command_pool_, graphics_queue_, *staging_buffer, buffer_size);

  // Delete and free the staging buffer
  staging_buffer->clean(logical_device_);
  delete staging_buffer;

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::createIndexBuffer(std::vector<uint32_t>& indices) {

	index_buffers_.push_back(new Buffer(Buffer::kBufferType_Index));

  // Create a staging buffer and allocate its memory
  VkDeviceSize buffer_size = sizeof(indices[0]) * indices.size();
  
  Buffer* staging_buffer = new Buffer(Buffer::kBufferType_Index);
  staging_buffer->create(physical_device_, logical_device_,
    buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
    static_cast<uint32_t>(indices.size()));

  // Map the memory from the CPU to GPU
  void* data;
  vkMapMemory(logical_device_, staging_buffer->buffer_memory_, 0, buffer_size, 0, &data);
  memcpy(data, indices.data(), (size_t)buffer_size);
  vkUnmapMemory(logical_device_, staging_buffer->buffer_memory_);

  // Create the actual index buffer
  index_buffers_[index_buffers_.size() - 1]->create(physical_device_, logical_device_, buffer_size, 
    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, static_cast<uint32_t>(indices.size()));
  
  // Copy the content from the staging buffer to the vertex buffer (allocated in gpu memory)
  index_buffers_[index_buffers_.size() - 1]->copy(logical_device_, command_pool_, graphics_queue_, *staging_buffer, buffer_size);
  
  // Delete and free the staging buffer
  staging_buffer->clean(logical_device_);
  delete staging_buffer;
  
}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::createCommandBuffers() {

  if (window_width_ == 0 || window_height_ == 0) return;

  command_buffers_.resize(swap_chain_framebuffers_.size());

  // Allocate command buffers
  VkCommandBufferAllocateInfo allocate_info{};
  allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocate_info.commandPool = command_pool_;
  allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocate_info.commandBufferCount = (uint32_t)command_buffers_.size();

  if (vkAllocateCommandBuffers(logical_device_, &allocate_info, command_buffers_.data()) != VK_SUCCESS) {
    throw std::runtime_error("\nFailed to create command buffers.");
  }

  auto scene = ParticleEditor::instance().getScene();

  // Record the command buffers
  for (int i = 0; i < command_buffers_.size(); i++) {
    // Begin command buffers recording
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = 0;
    begin_info.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(command_buffers_[i], &begin_info) != VK_SUCCESS) {
      throw std::runtime_error("\nFailed to begin command buffer recording.");
    }

    // Set the render pass begin info
    VkRenderPassBeginInfo render_pass_begin{};
    render_pass_begin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_begin.renderPass = render_pass_;
    render_pass_begin.framebuffer = swap_chain_framebuffers_[i];
    render_pass_begin.renderArea.offset = { 0, 0 };
    render_pass_begin.renderArea.extent = swap_chain_extent_;
    std::array<VkClearValue, 2> clear_values{};
    clear_values[0].color = { 0.0f, 0.0f, 0.0f, 1.0f }; // Black
    clear_values[1].depthStencil = { 1.0f, 0 };
    render_pass_begin.clearValueCount = static_cast<uint32_t>(clear_values.size());
    render_pass_begin.pClearValues = clear_values.data();

    // Begin recording the commands on the command buffer
    vkCmdBeginRenderPass(command_buffers_[i], &render_pass_begin, VK_SUBPASS_CONTENTS_INLINE);

    // Call draw systems to prepare the commands for all the entities
    system_draw_objects_->addDrawCommands(i, command_buffers_[i], 
      scene->getEntities(0)); // opaque entities

		system_draw_translucents_->addDrawCommands(i, command_buffers_[i],
      scene->getEntities(1)); // translucent entities

		system_draw_particles_->addParticlesDrawCommand(i, command_buffers_[i],
			scene->getEntities(2)); // particle system entities

    // Finish recording commands
    vkCmdEndRenderPass(command_buffers_[i]);

    if (vkEndCommandBuffer(command_buffers_[i]) != VK_SUCCESS) {
      throw std::runtime_error("\nFailed to end command buffer recording.");
    }

  }

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::createSyncObjects() {

  available_image_semaphores_.resize(MAX_FRAMES_IN_FLIGHT);
  finished_render_semaphores_.resize(MAX_FRAMES_IN_FLIGHT);
  in_flight_fences_.resize(MAX_FRAMES_IN_FLIGHT);
  images_in_flight_.resize(swap_chain_images_.size(), VK_NULL_HANDLE);

  VkSemaphoreCreateInfo semaphore_info{};
  semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fence_info{};
  fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    if ((vkCreateSemaphore(logical_device_, &semaphore_info, nullptr, &available_image_semaphores_[i]) ||
      vkCreateSemaphore(logical_device_, &semaphore_info, nullptr, &finished_render_semaphores_[i]) ||
      vkCreateFence(logical_device_, &fence_info, nullptr, &in_flight_fences_[i]))
      != VK_SUCCESS) {
      throw std::runtime_error("\nFailed to create synchronization objects for a frame.");
    }
  }

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::updateUniformBuffers(uint32_t current_image) {

  auto scene = ParticleEditor::instance().getScene();

  // Update the dynamic buffer using the draw systems
  // opaque entities
  system_draw_objects_->updateUniformBuffers(current_image, scene->getEntities(0)); 

  // translucent entities
	system_draw_translucents_->updateUniformBuffers(current_image, scene->getEntities(1)); 

  // particles entities
  system_draw_particles_->updateUniformBuffers(current_image, scene->getEntities(2));

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::drawFrame() {

  if (window_width_ == 0 || window_height_ == 0) return;

  // Wait until a frame is available
  vkWaitForFences(logical_device_, 1, &in_flight_fences_[current_frame_], VK_TRUE, UINT64_MAX);

  // Acquire an image from the swap chain
  uint32_t image_index;
  VkResult result = vkAcquireNextImageKHR(logical_device_, swap_chain_, UINT64_MAX, available_image_semaphores_[current_frame_],
    VK_NULL_HANDLE, &image_index); // UINT64_MAX stands for no timeout

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    recreateSwapChain();
  }
  else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("\nFailed to acquire swap chain image.");
  }

  // Check if a previous frame is using the image
  if (images_in_flight_[image_index] != VK_NULL_HANDLE) {
    vkWaitForFences(logical_device_, 1, &images_in_flight_[image_index], VK_TRUE, UINT64_MAX);
  }

  // Mark the image as used in this frame
  images_in_flight_[image_index] = in_flight_fences_[current_frame_];

  // Update the uniform buffers
  updateUniformBuffers(image_index);

  // Execute the command buffer with that image as attachment in the framebuffer
  VkSubmitInfo submit_info{};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  VkSemaphore wait_semaphores[] = { available_image_semaphores_[current_frame_] };
  VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
  submit_info.waitSemaphoreCount = 1;
  submit_info.pWaitSemaphores = wait_semaphores;
  submit_info.pWaitDstStageMask = wait_stages;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &command_buffers_[image_index];
  VkSemaphore signal_semaphores[] = { finished_render_semaphores_[current_frame_] };
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = signal_semaphores;

  vkResetFences(logical_device_, 1, &in_flight_fences_[current_frame_]);

  if (vkQueueSubmit(graphics_queue_, 1, &submit_info, in_flight_fences_[current_frame_]) != VK_SUCCESS) {
    throw std::runtime_error("\nFailed to submit draw command buffer.");
  }

  // Return the image to the swap chain for presentation
  VkPresentInfoKHR present_info{};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = signal_semaphores;
  VkSwapchainKHR swap_chains[] = { swap_chain_ };
  present_info.swapchainCount = 1;
  present_info.pSwapchains = swap_chains;
  present_info.pImageIndices = &image_index;
  present_info.pResults = nullptr;

  result = vkQueuePresentKHR(graphics_queue_, &present_info);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || resized_framebuffer_) {
    resized_framebuffer_ = false;
    recreateSwapChain();
  }
  else if (result != VK_SUCCESS) {
    throw std::runtime_error("\nFailed to present swap chain image.");
  }

  current_frame_ = (current_frame_ + 1) % MAX_FRAMES_IN_FLIGHT;

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::recreateSwapChain() {

  int width = 0;
  int height = 0;
  glfwGetFramebufferSize(window_, &width, &height);
  if (width == 0 || height == 0) {
    glfwGetFramebufferSize(window_, &width, &height);
    glfwWaitEvents();
  }

  vkDeviceWaitIdle(logical_device_);

  cleanupSwapChain();

  createSwapChain();
	createRenderPass();
  for (int i = 0; i < materials_.size(); i++) {
    if (window_width_ == 0 || window_height_ == 0) break;
    materials_[i]->createPipelineLayout();
    materials_[i]->createGraphicPipeline();
  }
  createColorResources();
  createDepthResources();
  createFramebuffers();
  for (int i = 0; i < materials_.size(); i++) {
    if (window_width_ == 0 || window_height_ == 0) break;
    materials_[i]->createDescriptorPools();
    materials_[i]->initDescriptorSets();
  }
  createCommandBuffers();

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::cleanupSwapChain() {

  // Render images
  color_image_->clean(logical_device_);
  depth_image_->clean(logical_device_);
  delete color_image_;
  delete depth_image_;

  // Swap chain frame buffers
  for (int i = 0; i < swap_chain_framebuffers_.size(); i++) {
    vkDestroyFramebuffer(logical_device_, swap_chain_framebuffers_[i], nullptr);
  }

  // Command buffer
  vkFreeCommandBuffers(logical_device_, command_pool_,
    static_cast<uint32_t>(command_buffers_.size()), command_buffers_.data());

  
  // Pipelines and all related to them
  for (int i = 0; i < materials_.size(); i++) {
    materials_[i]->cleanMaterialResources();
  }

  // Render pass
  vkDestroyRenderPass(logical_device_, render_pass_, nullptr);

  // Swap chain image views
  for (int i = 0; i < swap_chain_images_.size(); i++) {
    vkDestroyImageView(logical_device_, swap_chain_images_[i]->image_view_, nullptr);
  }

  // Swap chain
  vkDestroySwapchainKHR(logical_device_, swap_chain_, nullptr);

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::framebufferResizeCallback(GLFWwindow* window, int width, int height) {

  auto app = reinterpret_cast<AppData*>(glfwGetWindowUserPointer(window));
  app->resized_framebuffer_ = true;
  app->window_width_ = width;
  app->window_height_ = height;

  if (width != 0) {
    ParticleEditor::instance().getCamera()->setupProjection(90.0f, (float)app->window_width_ / (float)app->window_height_, 0.1f, 10.0f);
  }

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::setupDebugMessenger() {

  if (!kEnableValidationLayers) return;

  VkDebugUtilsMessengerCreateInfoEXT messenger_info;
  populateDebugMessengerCreateInfo(messenger_info);

  if (CreateDebugUtilsMessengerEXT(instance_, &messenger_info, nullptr, &debug_messenger_) != VK_SUCCESS) {
    throw std::runtime_error("\nDebug Messenger creation has failed.");
  }

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout) {

  VkCommandBuffer cmd_buffer = beginSingleTimeCommands(logical_device_, command_pool_);

  VkImageMemoryBarrier barrier{};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = old_layout;
  barrier.newLayout = new_layout;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = image;
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;

  VkPipelineStageFlags source_stage;
  VkPipelineStageFlags destination_stage;

  if (new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    if (hasStencilComponent(format)) {
      barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }
  }
  else {
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  }


  if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  }
  else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  }
  else if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
      VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destination_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  }
  else {
    throw std::runtime_error("\nUnsupported layout transition.");
  }

  vkCmdPipelineBarrier(cmd_buffer, source_stage, destination_stage,
    0, 0, nullptr,
    0, nullptr, 1, &barrier);

  endSingleTimeCommands(logical_device_, command_pool_, cmd_buffer, graphics_queue_);

}

// ------------------------------------------------------------------------- //

// ------------------------------------------------------------------------- //

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::setupVertexBuffers(){

	std::vector<Vertex> quad_vertices;
  // Quad
  quad_vertices = {
		{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}},
		{{ 0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}},
		{{ 0.5f,  0.5f, 0.0f}, {0.0f, 1.0f}},
		{{-0.5f,  0.5f, 0.0f}, {1.0f, 1.0f}}
	};

  createVertexBuffer(quad_vertices);


  // Cube
	//vertices = {
    //{{-0.5f, ... };
	//createVertexBuffer(vertices);


}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::setupIndexBuffers(){

	std::vector<uint32_t> quad_indices;
  // Quad
	quad_indices = { 0, 1, 2, 2, 3, 0 };
  
  createIndexBuffer(quad_indices);

	// Cube
	//indices = {
		//{{-0.5f, ... };
	//createIndexBuffer(indices);

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::setupMaterials(){

  OpaqueMaterial* opaque_material = new OpaqueMaterial();
  opaque_material->setInternalReferences(&logical_device_, &physical_device_,
    static_cast<uint32_t>(swap_chain_images_.size()));
  materials_.push_back(opaque_material);
  
  TranslucentMaterial* translucent_material = new TranslucentMaterial();
  translucent_material->setInternalReferences(&logical_device_, &physical_device_,
    static_cast<uint32_t>(swap_chain_images_.size()));
	materials_.push_back(translucent_material);

  ParticlesMaterial* particles_material = new ParticlesMaterial();
  particles_material->setInternalReferences(&logical_device_, &physical_device_,
		static_cast<uint32_t>(swap_chain_images_.size()));
	materials_.push_back(particles_material);
  
}

// ------------------------------------------------------------------------- //
