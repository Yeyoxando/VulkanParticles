/*
 *	Author: Diego Ochando Torres
 *  Date: 05/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

// ------------------------------------------------------------------------- // 

#include "billboards_app.h"
#include "vulkan_utils.h"

#include <stdexcept>
#include <cstring>
#include <vector>
#include <set>
#include <chrono>
#include <unordered_map>

#include <glm/gtc/matrix_transform.hpp>

#include <stb_image.h>
#include <tiny_obj_loader.h>

// ------------------------------------------------------------------------- // 

BillboardsApp::BillboardsApp() {

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
  descriptor_set_layout_ = VK_NULL_HANDLE;
  pipeline_layout_ = VK_NULL_HANDLE;
  graphics_pipeline_ = VK_NULL_HANDLE;
  command_pool_ = VK_NULL_HANDLE;
  
  vertex_buffer_ = VK_NULL_HANDLE;
  vertex_buffer_memory_ = VK_NULL_HANDLE;
  index_buffer_ = VK_NULL_HANDLE;
  index_buffer_memory_ = VK_NULL_HANDLE;
  
  descriptor_pool_ = VK_NULL_HANDLE;
  
  texture_image_ = VK_NULL_HANDLE;
  texture_image_memory_ = VK_NULL_HANDLE;;
  texture_image_view_ = VK_NULL_HANDLE;
  texture_sampler_ = VK_NULL_HANDLE;

  depth_image_ = VK_NULL_HANDLE;
  depth_image_memory_ = VK_NULL_HANDLE;
  depth_image_view_ = VK_NULL_HANDLE;

  msaa_samples_ = VK_SAMPLE_COUNT_1_BIT;
  color_image_ = VK_NULL_HANDLE;
  color_image_memory_ = VK_NULL_HANDLE;
  color_image_view_ = VK_NULL_HANDLE;

  current_frame_ = 0;
  resized_framebuffer_ = false;
  close_window_ = false;

  camera_ = new Camera();
  input_ = new InputManager();

}

// ------------------------------------------------------------------------- // 

BillboardsApp::~BillboardsApp() {

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::run() {

  initWindow();

  initVulkan();
  renderLoop();
  close();

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::initWindow(int width /*= 800*/, int height /*= 600*/) {

  glfwInit();

  // Tell GLFW to not use OpenGL API
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

  window_ = glfwCreateWindow(width, height, "Hello Billboards", nullptr, nullptr);
  window_width_ = width;
  window_height_ = height;

  glfwSetWindowUserPointer(window_, this);
  glfwSetFramebufferSizeCallback(window_, framebufferResizeCallback);
  glfwSetInputMode(window_, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::initVulkan() {

  vertices_ = { 
    {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{ 0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{ 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
    {{-0.5f,  0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{ 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
    {{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
  };

  indices_ = {0, 1, 2, 2, 3, 0,
              4, 5, 6, 6, 7, 4 };

  createInstance();
  setupDebugMessenger();
  createSurface();
  pickPhysicalDevice();
  createLogicalDevice();
  createSwapChain();
  createImageViews();
  createRenderPass();
  createDescriptorSetLayout();
  createGraphicsPipeline();
  createCommandPool();
  createColorResources();
  createDepthResources();
  createFramebuffers();
  //createTextureImage("../../../resources/textures/numerical_grid.jpg");
  createTextureImage("../../../resources/textures/viking_room.png");
  createTextureImageView();
  createTextureSampler();
  loadModel("../../../resources/models/viking_room.obj");
  createVertexBuffers();
  createIndexBuffers();
  createUniformBuffers();
  createDescriptorPool();
  createDescriptorSets();
  createCommandBuffers();
  createSyncObjects();

  input_->init(window_);

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::renderLoop() {

  while (!glfwWindowShouldClose(window_) && !close_window_) {
    
    glfwPollEvents();

    updateFrame();

    drawFrame();

  }

  // Wait until the device is on idle so we can clean up resources without being in use
  vkDeviceWaitIdle(logical_device_);

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::close() {

  // Program cleanup
  delete input_;
  delete camera_;

  // Vulkan cleanup
  cleanupSwapChain();

  vkDestroySampler(logical_device_, texture_sampler_, nullptr);
  vkDestroyImageView(logical_device_, texture_image_view_, nullptr);
  vkDestroyImage(logical_device_, texture_image_, nullptr);
  vkFreeMemory(logical_device_, texture_image_memory_, nullptr);

  vkDestroyDescriptorSetLayout(logical_device_, descriptor_set_layout_, nullptr);

  vkDestroyBuffer(logical_device_, index_buffer_, nullptr);
  vkFreeMemory(logical_device_, index_buffer_memory_, nullptr);

  vkDestroyBuffer(logical_device_, vertex_buffer_, nullptr);
  vkFreeMemory(logical_device_, vertex_buffer_memory_, nullptr);

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

void BillboardsApp::createInstance() {

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

std::vector<const char*> BillboardsApp::getRequiredExtensions() {

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

bool BillboardsApp::checkValidationLayerSupport() {

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

void BillboardsApp::setupDebugMessenger() {

  if (!kEnableValidationLayers) return;

  VkDebugUtilsMessengerCreateInfoEXT messenger_info;
  populateDebugMessengerCreateInfo(messenger_info);

  if (CreateDebugUtilsMessengerEXT(instance_, &messenger_info, nullptr, &debug_messenger_) != VK_SUCCESS) {
    throw std::runtime_error("Debug Messenger creation has failed.");
  }

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& create_info) {
  
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

VKAPI_ATTR VkBool32 VKAPI_CALL BillboardsApp::debugCallback(
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

void BillboardsApp::createSurface() {

  if (glfwCreateWindowSurface(instance_, window_, nullptr, &surface_) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create window surface.");
  }

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::pickPhysicalDevice() {

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
      msaa_samples_ = getMaxUsableSampleCount();
      break;
    }
  }

  if (physical_device_ == VK_NULL_HANDLE) {
    throw std::runtime_error("The device's GPUs are not suitable for the requested functionality.");
  }

}

// ------------------------------------------------------------------------- // 

bool BillboardsApp::isDeviceSuitable(VkPhysicalDevice device) {

  // Basic device properties
  VkPhysicalDeviceProperties device_properties;
  vkGetPhysicalDeviceProperties(device, &device_properties);

  // Tex compression, 64-bit floats and multi-viewport rendering
  VkPhysicalDeviceFeatures device_features;
  vkGetPhysicalDeviceFeatures(device, &device_features);

  // Return false if a requested feature isnot supported
  if (!device_features.samplerAnisotropy) {
    return false;
  }

  // Check if device supports graphic and present queues
  QueueFamilyIndices indices = findQueueFamilies(device);

  // Check if the required extensions are supported
  bool extensions_supported = checkDeviceExtensionSupport(device);
   
  // Check if the swap chain details are adequate on the device
  bool adequate_swap_chain = false;
  if (extensions_supported) {
    SwapChainSupportDetails details = querySwapChainSupportDetails(device);
    adequate_swap_chain = !details.formats.empty() && !details.present_modes.empty();
  }

  // Expand this function with necessary features

  return indices.isComplete() && extensions_supported && adequate_swap_chain;

}

// ------------------------------------------------------------------------- // 

bool BillboardsApp::checkDeviceExtensionSupport(VkPhysicalDevice device) {

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

QueueFamilyIndices BillboardsApp::findQueueFamilies(VkPhysicalDevice device) {
  
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
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface_, &present_support);
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

SwapChainSupportDetails BillboardsApp::querySwapChainSupportDetails(VkPhysicalDevice device) {

  SwapChainSupportDetails details;

  // Request device supported capabilities
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_, &details.capabilities);

  // Request device supported formats
  uint32_t formats_count;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formats_count, nullptr);
  if (formats_count != 0) {
    details.formats.resize(formats_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formats_count, details.formats.data());
  }

  // Request device supported present modes
  uint32_t present_modes_count;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &present_modes_count, nullptr);
  if (present_modes_count != 0) {
    details.present_modes.resize(present_modes_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &present_modes_count, details.present_modes.data());
  }

  return details;

}

// ------------------------------------------------------------------------- // 

VkSurfaceFormatKHR BillboardsApp::chooseSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats) {

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

VkPresentModeKHR BillboardsApp::chooseSwapChainPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes) {

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

VkExtent2D BillboardsApp::chooseSwapChainExtent(const VkSurfaceCapabilitiesKHR& capabilities) {

  if (capabilities.currentExtent.width != UINT32_MAX) {
    // Some window managers set this by default, if is not equal to this it can be modified
    return capabilities.currentExtent;
  }
  else {
    // Choose best window resolution fit between the current and min/max image extent
    int width;
    int height;
    glfwGetFramebufferSize(window_, &width, &height);

    VkExtent2D window_extent = {
      static_cast<uint32_t>(width),
      static_cast<uint32_t>(height)
    };

    window_extent.width = std::max(capabilities.minImageExtent.width,
      std::min(capabilities.maxImageExtent.width, window_extent.width));
    window_extent.height = std::max(capabilities.minImageExtent.height,
      std::min(capabilities.maxImageExtent.height, window_extent.height));

    return window_extent;
  }

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::createLogicalDevice() {

  // Create needed queues
  QueueFamilyIndices indices = findQueueFamilies(physical_device_);
  float queue_priority = 1.0f;

  // Store unique queues
  std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
  std::set<uint32_t> unique_queue_families = {indices.graphics_family.value(), 
    indices.present_family.value()};

  // Create unique queues create info
  for (uint32_t queue_family : unique_queue_families) {
    VkDeviceQueueCreateInfo queue_create_info{};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = queue_family;
    queue_create_info.queueCount = 1;
    queue_create_info.pQueuePriorities = &queue_priority;
    queue_create_infos.push_back(queue_create_info);
  }

  // Set needed vulkan features
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
    throw std::runtime_error("Failed to create Vulkan logical device.");
  }

  // Store graphics queue handle
  vkGetDeviceQueue(logical_device_, indices.graphics_family.value(), 0, &graphics_queue_);

  // Store present queue handle
  vkGetDeviceQueue(logical_device_, indices.present_family.value(), 0, &present_queue_);

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::createSwapChain() {

  if (window_width_ == 0 || window_height_ == 0) return;

  // Choose preferred details
  SwapChainSupportDetails details = querySwapChainSupportDetails(physical_device_);

  VkSurfaceFormatKHR format = chooseSwapChainSurfaceFormat(details.formats);
  VkPresentModeKHR present_mode = chooseSwapChainPresentMode(details.present_modes);
  VkExtent2D extent = chooseSwapChainExtent(details.capabilities);

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

  QueueFamilyIndices indices = findQueueFamilies(physical_device_);
  uint32_t queue_family_indices[] = { indices.graphics_family.value(), indices.present_family.value() };
  if (indices.graphics_family != indices.present_family) {
    // Used in multiple queues
    create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    create_info.queueFamilyIndexCount = 2; // Indicate number of shared families
    create_info.pQueueFamilyIndices = queue_family_indices; // Shared families
  }
  else{
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
    throw std::runtime_error("Failed to create swap chain.");
  }

  // Store image handles for render operation
  vkGetSwapchainImagesKHR(logical_device_, swap_chain_, &image_count, nullptr);
  swap_chain_images_.resize(image_count);
  vkGetSwapchainImagesKHR(logical_device_, swap_chain_, &image_count, swap_chain_images_.data());

  // Store format and extent for future operations
  swap_chain_image_format_ = format.format;
  swap_chain_extent_ = extent;

}

// ------------------------------------------------------------------------- // 

VkImageView BillboardsApp::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspect_flags) {

  VkImageViewCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  create_info.image = image;
  create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  create_info.format = format;

  // Remapping components (Set to default)
  create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

  // Image usage (color bit for those)
  create_info.subresourceRange.aspectMask = aspect_flags;
  create_info.subresourceRange.baseMipLevel = 0;
  create_info.subresourceRange.levelCount = 1;
  create_info.subresourceRange.baseArrayLayer = 0;
  create_info.subresourceRange.layerCount = 1;

  VkImageView image_view;
  if (vkCreateImageView(logical_device_, &create_info, nullptr, &image_view) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create texture image view.");
  }

  return image_view;

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::createImageViews() {

  if (window_width_ == 0 || window_height_ == 0) return;

  swap_chain_image_views_.resize(swap_chain_images_.size());

  for (int i = 0; i < swap_chain_images_.size(); i++) {
    swap_chain_image_views_[i] = createImageView(swap_chain_images_[i], swap_chain_image_format_, VK_IMAGE_ASPECT_COLOR_BIT);
  }

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::createRenderPass() {

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
  depth_attachment.format = findDepthFormat();
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

  // Subpasses (only 1 for now)
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
    throw std::runtime_error("Failed to create the render pass");
  }

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::createDescriptorSetLayout() {

  // Create the binding for the vertex shader MVP matrices
  VkDescriptorSetLayoutBinding ubo_layout_binding{};
  ubo_layout_binding.binding = 0;
  ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  ubo_layout_binding.descriptorCount = 1;
  ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  ubo_layout_binding.pImmutableSamplers = nullptr;

  VkDescriptorSetLayoutBinding sampler_layout_binding{};
  sampler_layout_binding.binding = 1;
  sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  sampler_layout_binding.descriptorCount = 1;
  sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  sampler_layout_binding.pImmutableSamplers = nullptr;

  // Create the descriptor set
  std::array<VkDescriptorSetLayoutBinding, 2> bindings = { ubo_layout_binding, sampler_layout_binding };
  VkDescriptorSetLayoutCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  create_info.bindingCount = static_cast<uint32_t>(bindings.size());
  create_info.pBindings = bindings.data();

  if (vkCreateDescriptorSetLayout(logical_device_, &create_info, nullptr, &descriptor_set_layout_) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create descriptor set layout.");
  }

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::createGraphicsPipeline() {

  if (window_width_ == 0 || window_height_ == 0) return;

  // Different pipelines has to be created for different render options
  // Not like in OpenGL where you can change glBlendFunc between objects
  // Whole pipeline has to be created and changed to do this

  // Load shaders
  auto vert_shader_code = readFile("../../../resources/shaders/shaders_spirv/v_hello_triangle.spv");
  auto frag_shader_code = readFile("../../../resources/shaders/shaders_spirv/f_hello_triangle.spv");

  VkShaderModule vert_shader_module = createShaderModule(vert_shader_code);
  VkShaderModule frag_shader_module = createShaderModule(frag_shader_code);

  VkPipelineShaderStageCreateInfo vert_shader_stage_info{};
  vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vert_shader_stage_info.module = vert_shader_module;
  vert_shader_stage_info.pName = "main";
  vert_shader_stage_info.pSpecializationInfo = nullptr; // Constants can be defined here

  VkPipelineShaderStageCreateInfo frag_shader_stage_info{};
  frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  frag_shader_stage_info.module = frag_shader_module;
  frag_shader_stage_info.pName = "main";
  frag_shader_stage_info.pSpecializationInfo = nullptr; // Constants can be defined here

  VkPipelineShaderStageCreateInfo shader_stages[] = { vert_shader_stage_info, frag_shader_stage_info };

  // Set vertex input format
  auto binding_desc = Vertex::getBindingDescription();
  auto attribute_desc = Vertex::getAttributeDescription();

  VkPipelineVertexInputStateCreateInfo vertex_input_info{};
  vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_input_info.vertexBindingDescriptionCount = 1;
  vertex_input_info.pVertexBindingDescriptions = &binding_desc;
  vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_desc.size());
  vertex_input_info.pVertexAttributeDescriptions = attribute_desc.data();

  // Set input assembly settings
  VkPipelineInputAssemblyStateCreateInfo input_assembly_info{};
  input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_assembly_info.primitiveRestartEnable = VK_FALSE; // True to use index buffers with triangle strip

  // Create viewport area to draw
  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)swap_chain_extent_.width;
  viewport.height = (float)swap_chain_extent_.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  // Create scissor rectangle with extent size
  VkRect2D scissor{};
  scissor.offset = { 0, 0 };
  scissor.extent = swap_chain_extent_;

  VkPipelineViewportStateCreateInfo viewport_state_info{};
  viewport_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state_info.viewportCount = 1;
  viewport_state_info.pViewports = &viewport;
  viewport_state_info.scissorCount = 1;
  viewport_state_info.pScissors = &scissor;

  // Create the rasterizer settings
  VkPipelineRasterizationStateCreateInfo rasterizer_state_info{};
  rasterizer_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer_state_info.depthClampEnable = VK_FALSE;
  rasterizer_state_info.rasterizerDiscardEnable = VK_FALSE;
  rasterizer_state_info.polygonMode = VK_POLYGON_MODE_FILL; // Line to draw wireframe but require a GPU feature
  rasterizer_state_info.lineWidth = 1.0f;
  rasterizer_state_info.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer_state_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // Turned counter-clockwise due to glm y clip flip on the projection matrix
  rasterizer_state_info.depthBiasEnable = VK_FALSE;
  rasterizer_state_info.depthBiasClamp = 0.0f;
  rasterizer_state_info.depthBiasConstantFactor = 0.0f;
  rasterizer_state_info.depthBiasSlopeFactor = 0.0f;

  // Create multi sampling settings
  VkPipelineMultisampleStateCreateInfo multisample_state_info{};
  multisample_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisample_state_info.sampleShadingEnable = VK_FALSE;
  multisample_state_info.rasterizationSamples = msaa_samples_;
  multisample_state_info.minSampleShading = 1.0f;
  multisample_state_info.pSampleMask = nullptr;
  multisample_state_info.alphaToCoverageEnable = VK_FALSE;
  multisample_state_info.alphaToOneEnable = VK_FALSE;

  // Create depth and stencil settings for the framebuffer
  VkPipelineDepthStencilStateCreateInfo depth_stencil_info{};
  depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depth_stencil_info.depthTestEnable = VK_TRUE;
  depth_stencil_info.depthWriteEnable = VK_TRUE;
  depth_stencil_info.depthCompareOp = VK_COMPARE_OP_LESS;
  depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
  depth_stencil_info.minDepthBounds = 0.0f;
  depth_stencil_info.maxDepthBounds = 1.0f;
  depth_stencil_info.stencilTestEnable = VK_FALSE;
  depth_stencil_info.front = {};
  depth_stencil_info.back = {};

  // Create color blend settings
  VkPipelineColorBlendAttachmentState color_blend_attachment{};
  color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
    VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  color_blend_attachment.blendEnable = VK_FALSE;
  color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
  color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
  // Alpha blending
  //color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  //color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
  color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

  VkPipelineColorBlendStateCreateInfo blend_state_info{};
  blend_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  blend_state_info.logicOpEnable = VK_FALSE;
  blend_state_info.logicOp = VK_LOGIC_OP_COPY;
  blend_state_info.attachmentCount = 1;
  blend_state_info.pAttachments = &color_blend_attachment;
  blend_state_info.blendConstants[0] = 0.0f;
  blend_state_info.blendConstants[1] = 0.0f;
  blend_state_info.blendConstants[2] = 0.0f;
  blend_state_info.blendConstants[3] = 0.0f;

  // Create dynamic settings of the pipeline
  VkDynamicState dynamic_states[] = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_LINE_WIDTH,
  };

  VkPipelineDynamicStateCreateInfo dynamic_state_info{};
  dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamic_state_info.dynamicStateCount = 2;
  dynamic_state_info.pDynamicStates = dynamic_states;

  // Create pipeline state layout (Uniforms)
  VkPipelineLayoutCreateInfo layout_info{};
  layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  layout_info.setLayoutCount = 1;
  layout_info.pSetLayouts = &descriptor_set_layout_;
  layout_info.pushConstantRangeCount = 0;
  layout_info.pPushConstantRanges = nullptr;

  if (vkCreatePipelineLayout(logical_device_, &layout_info, nullptr, &pipeline_layout_) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create pipeline layout.");
  }

  // Create the actual graphics pipeline
  VkGraphicsPipelineCreateInfo graphics_pipeline_info{};
  graphics_pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  graphics_pipeline_info.stageCount = 2;
  graphics_pipeline_info.pStages = shader_stages;
  graphics_pipeline_info.pVertexInputState = &vertex_input_info;
  graphics_pipeline_info.pInputAssemblyState = &input_assembly_info;
  graphics_pipeline_info.pViewportState = &viewport_state_info;
  graphics_pipeline_info.pRasterizationState = &rasterizer_state_info;
  graphics_pipeline_info.pMultisampleState = &multisample_state_info;
  graphics_pipeline_info.pDepthStencilState = &depth_stencil_info;
  graphics_pipeline_info.pColorBlendState = &blend_state_info;
  graphics_pipeline_info.pDynamicState = nullptr;
  graphics_pipeline_info.layout = pipeline_layout_;
  graphics_pipeline_info.renderPass = render_pass_;
  graphics_pipeline_info.subpass = 0;
  graphics_pipeline_info.basePipelineHandle = VK_NULL_HANDLE; // derive from other graphics pipeline with flags VK_PIPELINE_CREATE_DERIVATIVE_BIT
  graphics_pipeline_info.basePipelineIndex = -1;

  if (vkCreateGraphicsPipelines(logical_device_, VK_NULL_HANDLE, 1, &graphics_pipeline_info,
    nullptr, &graphics_pipeline_) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create the graphics pipeline.");
  }

  vkDestroyShaderModule(logical_device_, vert_shader_module, nullptr);
  vkDestroyShaderModule(logical_device_, frag_shader_module, nullptr);

}

// ------------------------------------------------------------------------- // 

VkShaderModule BillboardsApp::createShaderModule(const std::vector<char>& bytecode) {
  
  VkShaderModuleCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.codeSize = bytecode.size();
  create_info.pCode = reinterpret_cast<const uint32_t*>(bytecode.data());

  VkShaderModule shader_module;

  if (vkCreateShaderModule(logical_device_, &create_info, nullptr, &shader_module) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create the shader module.");
  }

  return shader_module;

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::createFramebuffers() {

  if (window_width_ == 0 || window_height_ == 0) return;

  swap_chain_framebuffers_.resize(swap_chain_image_views_.size());

  for (int i = 0; i < swap_chain_image_views_.size(); i++) {
    std::array<VkImageView, 3> attachments = {
      color_image_view_,
      depth_image_view_,
      swap_chain_image_views_[i],
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
      throw std::runtime_error("Failed to create framebuffer.");
    }
  }

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::createCommandPool() {

  QueueFamilyIndices indices = findQueueFamilies(physical_device_);

  VkCommandPoolCreateInfo command_pool_info{};
  command_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  command_pool_info.queueFamilyIndex = indices.graphics_family.value();
  command_pool_info.flags = 0; // VK_COMMAND_POOL_CREATE_TRANSIENT_BIT means that command buffers will be rerecorded with new commands very often

  if (vkCreateCommandPool(logical_device_, &command_pool_info, nullptr, &command_pool_) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create command pool.");
  }

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::createColorResources() {

  VkFormat format = swap_chain_image_format_;

  createImage(swap_chain_extent_.width, swap_chain_extent_.height, msaa_samples_, format,
    VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, color_image_, color_image_memory_);
  color_image_view_ = createImageView(color_image_, format, VK_IMAGE_ASPECT_COLOR_BIT);

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::createDepthResources() {

  VkFormat format = findDepthFormat();

  createImage(swap_chain_extent_.width, swap_chain_extent_.height, msaa_samples_, format,
    VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depth_image_, depth_image_memory_);

  depth_image_view_ = createImageView(depth_image_, format, VK_IMAGE_ASPECT_DEPTH_BIT);

  transitionImageLayout(depth_image_, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::createTextureImage(const char* texture_path) {

  int tex_width;
  int tex_height;
  int tex_channels;

  // Load any texture with 4 channels with STBI_rgb_alpha
  stbi_uc* pixels = stbi_load(texture_path,
    &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);

  VkDeviceSize image_size = tex_width * tex_height * 4;

  if (!pixels) {
    throw std::runtime_error("Failed to load texture.");
  }

  // Create a staging buffer to transfer it to the device memory
  VkBuffer staging_buffer;
  VkDeviceMemory staging_buffer_memory;

  createBuffer(image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    staging_buffer, staging_buffer_memory);

  // Copy values to the buffer
  void* data;
  vkMapMemory(logical_device_, staging_buffer_memory, 0, image_size, 0, &data);
  memcpy(data, pixels, static_cast<size_t>(image_size));
  vkUnmapMemory(logical_device_, staging_buffer_memory);

  stbi_image_free(pixels);

  // Create the texture image

  createImage(tex_width, tex_height, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB,
    VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, texture_image_, texture_image_memory_);

  transitionImageLayout(texture_image_, VK_FORMAT_R8G8B8A8_SRGB,
    VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  copyBufferToImage(staging_buffer, texture_image_,
    static_cast<uint32_t>(tex_width), static_cast<uint32_t>(tex_height));
  transitionImageLayout(texture_image_, VK_FORMAT_R8G8B8A8_SRGB,
    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  vkDestroyBuffer(logical_device_, staging_buffer, nullptr);
  vkFreeMemory(logical_device_, staging_buffer_memory, nullptr);

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::createTextureImageView() {

  texture_image_view_ = createImageView(texture_image_, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::createTextureSampler() {

  VkSamplerCreateInfo sampler_info{};
  sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  sampler_info.minFilter = VK_FILTER_LINEAR;
  sampler_info.magFilter = VK_FILTER_LINEAR;

  sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

  sampler_info.anisotropyEnable = VK_TRUE;
  VkPhysicalDeviceProperties properties{};
  vkGetPhysicalDeviceProperties(physical_device_, &properties);
  sampler_info.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

  sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  sampler_info.unnormalizedCoordinates = VK_FALSE;
  sampler_info.compareEnable = VK_FALSE;
  sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
  sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  sampler_info.mipLodBias = 0.0f;
  sampler_info.minLod = 0.0f;
  sampler_info.maxLod = 0.0f;

  if (vkCreateSampler(logical_device_, &sampler_info, nullptr, &texture_sampler_) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create texture sampler");
  }

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::createImage(uint32_t width, uint32_t height, VkSampleCountFlagBits samples_count, VkFormat format, VkImageTiling tiling,
  VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& image_memory) {

  VkImageCreateInfo image_info{};
  image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  image_info.imageType = VK_IMAGE_TYPE_2D;
  image_info.extent.width = width;
  image_info.extent.height = height;
  image_info.extent.depth = 1;
  image_info.mipLevels = 1;
  image_info.arrayLayers = 1;
  image_info.format = format;
  image_info.tiling = tiling;
  image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  image_info.usage = usage;
  image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  image_info.samples = samples_count;
  image_info.flags = 0;

  if (vkCreateImage(logical_device_, &image_info, nullptr, &image) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create texture image.");
  }

  // Request memory requirements
  VkMemoryRequirements requirements;
  vkGetImageMemoryRequirements(logical_device_, image, &requirements);

  VkMemoryAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.allocationSize = requirements.size;
  alloc_info.memoryTypeIndex = findMemoryType(requirements.memoryTypeBits, properties);

  if (vkAllocateMemory(logical_device_, &alloc_info, nullptr, &image_memory) != VK_SUCCESS) {
    throw std::runtime_error("Failed to allocate image memory.");
  }

  vkBindImageMemory(logical_device_, image, image_memory, 0);


}

// ------------------------------------------------------------------------- // 

void BillboardsApp::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& memory) {

  // Create the  buffer
  VkBufferCreateInfo buffer_info{};
  buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_info.flags = 0;
  buffer_info.size = size;
  buffer_info.usage = usage;
  buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(logical_device_, &buffer_info, nullptr, &buffer) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create buffer.");
  }

  // Find memory requirements and allocate memory
  VkMemoryRequirements mem_requirements;
  vkGetBufferMemoryRequirements(logical_device_, buffer, &mem_requirements);

  VkMemoryAllocateInfo allocate_info{};
  allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocate_info.allocationSize = mem_requirements.size;
  allocate_info.memoryTypeIndex = findMemoryType(mem_requirements.memoryTypeBits, properties);

  if (vkAllocateMemory(logical_device_, &allocate_info, nullptr, &memory) != VK_SUCCESS) {
    throw std::runtime_error("Failed to allocate device memory for buffer.");
  }

  // Bind buffer and memory together
  vkBindBufferMemory(logical_device_, buffer, memory, 0);

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::loadModel(const char* model_path) {

  // Load model from file
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;

  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, model_path)) {
    throw std::runtime_error(warn + err);
  }

  // TODO: Provisional!!!!
  vertices_.clear();
  indices_.clear();

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

      vertex.color = { 1.0f, 1.0f, 1.0f };

      if (unique_vertices.count(vertex) == 0) {
        unique_vertices[vertex] = static_cast<uint32_t>(vertices_.size());
        vertices_.push_back(vertex);
      }

      indices_.push_back(unique_vertices[vertex]);
    }
  }

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::createVertexBuffers() {

  // Driver developers says that buffers should be created in only one VkBuffer
  // and use them with the offset properties that are in the functions
  // this will allow cache optimizations known as aliasing

  // Create a staging buffer and allocate its memory
  VkDeviceSize buffer_size = sizeof(vertices_[0]) * vertices_.size();
  VkBuffer staging_buffer;
  VkDeviceMemory staging_buffer_memory;
  
  createBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    staging_buffer, staging_buffer_memory);

  // Map the memory from the CPU to GPU
  void* data;
  vkMapMemory(logical_device_, staging_buffer_memory, 0, buffer_size, 0, &data);
  memcpy(data, vertices_.data(), (size_t)buffer_size);
  vkUnmapMemory(logical_device_, staging_buffer_memory);

  // The memory is not copied instantly, but it should be before the next call to vkQueueSubmit
  // This could cause performance decreases and it could be done with a host coherent memory heap
  // or using vkFlushMappedMemoryRanges after writing to mapped memory and vkInvalidateMappedMemoryRanges before reading


  // Create the actual vertex buffer
  createBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertex_buffer_, vertex_buffer_memory_);

  // Copy the content from the staging buffer to the vertex buffer (allocated in gpu memory)
  copyBuffer(staging_buffer, vertex_buffer_, buffer_size);

  // Delete and free the staging buffer
  vkDestroyBuffer(logical_device_, staging_buffer, nullptr);
  vkFreeMemory(logical_device_, staging_buffer_memory, nullptr);

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::createIndexBuffers() {

  // Create a staging buffer and allocate its memory
  VkDeviceSize buffer_size = sizeof(indices_[0]) * indices_.size();
  VkBuffer staging_buffer;
  VkDeviceMemory staging_buffer_memory;

  createBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    staging_buffer, staging_buffer_memory);

  // Map the memory from the CPU to GPU
  void* data;
  vkMapMemory(logical_device_, staging_buffer_memory, 0, buffer_size, 0, &data);
  memcpy(data, indices_.data(), (size_t)buffer_size);
  vkUnmapMemory(logical_device_, staging_buffer_memory);

  // Create the actual vertex buffer
  createBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, index_buffer_, index_buffer_memory_);

  // Copy the content from the staging buffer to the vertex buffer (allocated in gpu memory)
  copyBuffer(staging_buffer, index_buffer_, buffer_size);

  // Delete and free the staging buffer
  vkDestroyBuffer(logical_device_, staging_buffer, nullptr);
  vkFreeMemory(logical_device_, staging_buffer_memory, nullptr);

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::createUniformBuffers() {

  if (window_width_ == 0 || window_height_ == 0) return;

  uniform_buffers_.resize(swap_chain_images_.size());
  uniform_buffers_memory_.resize(swap_chain_images_.size());

  VkDeviceSize buffer_size = sizeof(UniformBufferObject);

  for (int i = 0; i < swap_chain_images_.size(); i++) {
    createBuffer(buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
      uniform_buffers_[i], uniform_buffers_memory_[i]);
  }

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::createDescriptorPool() {

  // Create descriptor pool
  std::array<VkDescriptorPoolSize, 2> pool_sizes{};
  pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  pool_sizes[0].descriptorCount = static_cast<uint32_t>(swap_chain_images_.size());
  pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  pool_sizes[1].descriptorCount = static_cast<uint32_t>(swap_chain_images_.size());

  VkDescriptorPoolCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  create_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
  create_info.pPoolSizes = pool_sizes.data();
  create_info.maxSets = static_cast<uint32_t>(swap_chain_images_.size());

  if (vkCreateDescriptorPool(logical_device_, &create_info, nullptr, &descriptor_pool_) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create descriptor pool.");
  }

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::createDescriptorSets() {

  // Allocate the descriptor sets
  std::vector<VkDescriptorSetLayout> descriptor_set_layouts(swap_chain_images_.size(), descriptor_set_layout_);
  VkDescriptorSetAllocateInfo allocate_info{};
  allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocate_info.descriptorPool = descriptor_pool_;
  allocate_info.descriptorSetCount = static_cast<uint32_t>(swap_chain_images_.size());
  allocate_info.pSetLayouts = descriptor_set_layouts.data();

  descriptor_sets_.resize(swap_chain_images_.size());
  if (vkAllocateDescriptorSets(logical_device_, &allocate_info, descriptor_sets_.data()) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create descriptor sets.");
  }

  // Populate the descriptors
  for (int i = 0; i < swap_chain_images_.size(); i++) {
    VkDescriptorBufferInfo buffer_info{};
    buffer_info.buffer = uniform_buffers_[i];
    buffer_info.offset = 0;
    buffer_info.range = sizeof(UniformBufferObject);

    VkDescriptorImageInfo image_info{};
    image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    image_info.imageView = texture_image_view_;
    image_info.sampler = texture_sampler_;

    std::array<VkWriteDescriptorSet, 2> write_descriptors{};
    write_descriptors[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_descriptors[0].dstSet = descriptor_sets_[i];
    write_descriptors[0].dstBinding = 0;
    write_descriptors[0].dstArrayElement = 0;
    write_descriptors[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write_descriptors[0].descriptorCount = 1;
    write_descriptors[0].pBufferInfo = &buffer_info;
    write_descriptors[0].pImageInfo = nullptr; // Image data
    write_descriptors[0].pTexelBufferView = nullptr; // Buffer views

    write_descriptors[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_descriptors[1].dstSet = descriptor_sets_[i];
    write_descriptors[1].dstBinding = 1;
    write_descriptors[1].dstArrayElement = 0;
    write_descriptors[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write_descriptors[1].descriptorCount = 1;
    write_descriptors[1].pBufferInfo = nullptr; // Buffer data
    write_descriptors[1].pImageInfo = &image_info; 
    write_descriptors[1].pTexelBufferView = nullptr; // Buffer views

    vkUpdateDescriptorSets(logical_device_, static_cast<uint32_t>(write_descriptors.size()), 
      write_descriptors.data(), 0, nullptr);
  }

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::copyBuffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size) {

  VkCommandBuffer command_buffer = beginSingleTimeCommands();

  VkBufferCopy copy_region{};
  copy_region.srcOffset = 0;
  copy_region.dstOffset = 0;
  copy_region.size = size;
  vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);

  endSingleTimeCommands(command_buffer);

}

// ------------------------------------------------------------------------- // 

VkCommandBuffer BillboardsApp::beginSingleTimeCommands() {

  // Temporary command buffer to perform the operations
  VkCommandBufferAllocateInfo temp_cmd_buffer_info{};
  temp_cmd_buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  temp_cmd_buffer_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  temp_cmd_buffer_info.commandPool = command_pool_;
  temp_cmd_buffer_info.commandBufferCount = 1;

  VkCommandBuffer temp_command_buffer;
  vkAllocateCommandBuffers(logical_device_, &temp_cmd_buffer_info, &temp_command_buffer);

  // Start recording the cmd buffer
  VkCommandBufferBeginInfo begin_info{};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  vkBeginCommandBuffer(temp_command_buffer, &begin_info);

  return temp_command_buffer;

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::endSingleTimeCommands(VkCommandBuffer cmd_buffer) {

  // Finish recording command buffer
  vkEndCommandBuffer(cmd_buffer);

  // Submit the buffer to execution
  VkSubmitInfo submit_info{};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &cmd_buffer;

  // Graphics queue support transfer commands, not necessary to have a separated queue
  vkQueueSubmit(graphics_queue_, 1, &submit_info, VK_NULL_HANDLE);
  vkQueueWaitIdle(graphics_queue_);

  // Free the temp command buffer
  vkFreeCommandBuffers(logical_device_, command_pool_, 1, &cmd_buffer);

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout) {

  VkCommandBuffer cmd_buffer = beginSingleTimeCommands();

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
    throw std::runtime_error("Unsupported layout transition.");
  }

  vkCmdPipelineBarrier(cmd_buffer, source_stage, destination_stage,
    0, 0, nullptr,
    0, nullptr, 1, &barrier);

  endSingleTimeCommands(cmd_buffer);

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {

  VkCommandBuffer cmd_buffer = beginSingleTimeCommands();

  VkBufferImageCopy region{};
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;

  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount = 1;

  region.imageOffset = { 0, 0, 0 };
  region.imageExtent = { width, height, 1 };

  vkCmdCopyBufferToImage(cmd_buffer, buffer, image,
    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

  endSingleTimeCommands(cmd_buffer);

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::createCommandBuffers() {

  if (window_width_ == 0 || window_height_ == 0) return;

  command_buffers_.resize(swap_chain_framebuffers_.size());

  // Allocate command buffers
  VkCommandBufferAllocateInfo allocate_info{};
  allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocate_info.commandPool = command_pool_;
  allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocate_info.commandBufferCount = (uint32_t)command_buffers_.size();

  if (vkAllocateCommandBuffers(logical_device_, &allocate_info, command_buffers_.data()) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create command buffers.");
  }

  // Record the command buffers
  for (int i = 0; i < command_buffers_.size(); i++) {
    // Begin command buffers recording
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = 0;
    begin_info.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(command_buffers_[i], &begin_info) != VK_SUCCESS) {
      throw std::runtime_error("Failed to begin command buffer recording.");
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

    // Record the commands on the command buffer
    vkCmdBeginRenderPass(command_buffers_[i], &render_pass_begin, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(command_buffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_);

    VkBuffer vertex_buffers[] = { vertex_buffer_ };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(command_buffers_[i], 0, 1, vertex_buffers, offsets);
    
    vkCmdBindIndexBuffer(command_buffers_[i], index_buffer_, 0, VK_INDEX_TYPE_UINT32);
    
    vkCmdBindDescriptorSets(command_buffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS, 
      pipeline_layout_, 0, 1, &descriptor_sets_[i], 0, nullptr);

    vkCmdDrawIndexed(command_buffers_[i], static_cast<uint32_t>(indices_.size()), 1, 0, 0, 0);

    vkCmdEndRenderPass(command_buffers_[i]);

    if (vkEndCommandBuffer(command_buffers_[i]) != VK_SUCCESS) {
      throw std::runtime_error("Failed to end command buffer recording.");
    }
  }

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::createSyncObjects() {

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
      throw std::runtime_error("Failed to create synchronization objects for a frame.");
    }
  }

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::updateFrame() {

  // Calculate time since rendering started
  static auto start_time = std::chrono::high_resolution_clock::now();

  auto current_time = std::chrono::high_resolution_clock::now();

  float time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();

  // INPUTS
  if (input_->getState()->right_click_action_ == GLFW_PRESS) {
    camera_->updateViewMatrix(input_->getMousePos());
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_RIGHT);
  }
  if (input_->getState()->right_click_action_ == GLFW_RELEASE) {
    camera_->setRotating(false);
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
  if (input_->getState()->wheel_offset > 0.05f ||
    input_->getState()->wheel_offset < -0.05f) {
      camera_->moveFront(input_->getState()->wheel_offset * 0.05f);
      input_->getState()->wheel_offset = 0.0f;
  }
  if (input_->getState()->escape_key_action == GLFW_PRESS) {
    close_window_ = true;
  }

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::updateUniformBuffers(uint32_t current_image) {

  // Calculate time since rendering started
  static auto start_time = std::chrono::high_resolution_clock::now();

  auto current_time = std::chrono::high_resolution_clock::now();

  float time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();

  // Update the uniform buffer to make the object spin
  UniformBufferObject ubo{};
  ubo.model = glm::rotate(glm::mat4(1.0f), 0.0f/*time * glm::radians(9.0f)*/, 
    glm::vec3(0.0f, 0.0f, 1.0f));

  ubo.view = camera_->getViewMatrix();

  ubo.projection = glm::perspective(glm::radians(90.0f),
    swap_chain_extent_.width / (float)swap_chain_extent_.height, 0.1f, 10.0f);
  // Invert clip Y due to GLM works with OpenGL and its inverted
  ubo.projection[1][1] *= -1;

  // Map the memory from the CPU to GPU
  void* data;
  vkMapMemory(logical_device_, uniform_buffers_memory_[current_image], 0,
    sizeof(ubo), 0, &data);
  memcpy(data, &ubo, sizeof(ubo));
  vkUnmapMemory(logical_device_, uniform_buffers_memory_[current_image]);

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::drawFrame() {

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
    throw std::runtime_error("Failed to acquire swap chain image.");
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
  VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
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
    throw std::runtime_error("Failed to submit draw command buffer.");
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
    throw std::runtime_error("Failed to present swap chain image.");
  }

  current_frame_ = (current_frame_ + 1) % MAX_FRAMES_IN_FLIGHT;

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::recreateSwapChain() {

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
  createImageViews();
  createRenderPass();
  createGraphicsPipeline();
  createColorResources();
  createDepthResources();
  createFramebuffers();
  createUniformBuffers();
  createDescriptorPool();
  createDescriptorSets();
  createCommandBuffers();

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::cleanupSwapChain() {

  vkDestroyImage(logical_device_, color_image_, nullptr);
  vkDestroyImageView(logical_device_, color_image_view_, nullptr);
  vkFreeMemory(logical_device_, color_image_memory_, nullptr);

  vkDestroyImage(logical_device_, depth_image_, nullptr);
  vkDestroyImageView(logical_device_, depth_image_view_, nullptr);
  vkFreeMemory(logical_device_, depth_image_memory_, nullptr);

  for (int i = 0; i < swap_chain_framebuffers_.size(); i++) {
    vkDestroyFramebuffer(logical_device_, swap_chain_framebuffers_[i], nullptr);
  }

  vkFreeCommandBuffers(logical_device_, command_pool_, 
    static_cast<uint32_t>(command_buffers_.size()), command_buffers_.data());

  vkDestroyPipeline(logical_device_, graphics_pipeline_, nullptr);

  vkDestroyPipelineLayout(logical_device_, pipeline_layout_, nullptr);

  vkDestroyRenderPass(logical_device_, render_pass_, nullptr);

  for (int i = 0; i < swap_chain_images_.size(); i++) {
    vkDestroyBuffer(logical_device_, uniform_buffers_[i], nullptr);
    vkFreeMemory(logical_device_, uniform_buffers_memory_[i], nullptr);
  }

  vkDestroyDescriptorPool(logical_device_, descriptor_pool_, nullptr);

  for (int i = 0; i < swap_chain_image_views_.size(); i++) {
    vkDestroyImageView(logical_device_, swap_chain_image_views_[i], nullptr);
  }

  vkDestroySwapchainKHR(logical_device_, swap_chain_, nullptr);

}

// ------------------------------------------------------------------------- // 

void BillboardsApp::framebufferResizeCallback(GLFWwindow* window, int width, int height) {

  auto app = reinterpret_cast<BillboardsApp*>(glfwGetWindowUserPointer(window));
  app->resized_framebuffer_ = true;
  app->window_width_ = width;
  app->window_height_ = height;

}

// ------------------------------------------------------------------------- // 

uint32_t BillboardsApp::findMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties) {

  // Request supported memory properties from the graphics card
  VkPhysicalDeviceMemoryProperties mem_properties;
  vkGetPhysicalDeviceMemoryProperties(physical_device_, &mem_properties);

  // Find a suitable memory type (Mask memory type bit and property flags)
  for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
    if (type_filter & (1 << i) && 
      (mem_properties.memoryTypes[i].propertyFlags & properties) == properties) {
      return i;
    }
  }

  throw std::runtime_error("Failed to find a suitable memory type.");

}

// ------------------------------------------------------------------------- // 

VkFormat BillboardsApp::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {

  for (uint32_t i = 0; i < candidates.size(); i++) {
    VkFormatProperties properties;
    vkGetPhysicalDeviceFormatProperties(physical_device_, candidates[i], &properties);

    if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features) {
      return candidates[i];
    }
    else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features) {
      return candidates[i];
    }
  }

  throw std::runtime_error("Failed to find a supported a format.");

}
// ------------------------------------------------------------------------- // 

VkFormat BillboardsApp::findDepthFormat() {

  return findSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
    VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

}

// ------------------------------------------------------------------------- // 

bool BillboardsApp::hasStencilComponent(VkFormat format) {

  return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;

}

// ------------------------------------------------------------------------- // 

VkSampleCountFlagBits BillboardsApp::getMaxUsableSampleCount() {

  VkPhysicalDeviceProperties phys_device_properties;
  vkGetPhysicalDeviceProperties(physical_device_, &phys_device_properties);

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
