/*
 *	Author: Diego Ochando Torres
 *  Date: 19/03/2021
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

 // ------------------------------------------------------------------------- //

#include "internal_materials.h"
#include "engine/vulkan_utils.h"

#include "../src/engine_internal/internal_gpu_resources.h"
#include "../src/engine_internal/internal_app_data.h"

// ------------------------------------------------------------------------- //
// ----------------------------- MATERIAL BASE ----------------------------- //
// ------------------------------------------------------------------------- //

Material::Material(){

	material_id_ = -1;

	graphics_pipeline_ = VK_NULL_HANDLE;
	pipeline_layout_ = VK_NULL_HANDLE;

	scene_descriptor_set_layout_ = VK_NULL_HANDLE;
	scene_descriptor_pool_ = VK_NULL_HANDLE;
	scene_descriptor_sets_ = std::vector<VkDescriptorSet>(0);
	scene_uniform_buffers_ = std::vector<Buffer*>(0);

	models_descriptor_set_layout_ = VK_NULL_HANDLE;
	models_descriptor_pool_ = VK_NULL_HANDLE;
	models_descriptor_sets_ = std::vector<VkDescriptorSet>(0);
	models_uniform_buffers_ = std::vector<Buffer*>(0);

	logical_device_reference_ = VK_NULL_HANDLE;
	physical_device_reference_ = VK_NULL_HANDLE;
	swap_chain_image_count_ = 0;

}

// ------------------------------------------------------------------------- //

Material::~Material(){

	// cleanMaterialResources must be called before deleting it to correctly delete gpu resources

	scene_descriptor_sets_.clear();
	scene_uniform_buffers_.clear();

	models_descriptor_sets_.clear();
	models_uniform_buffers_.clear();

}

// ------------------------------------------------------------------------- //

void Material::setInternalReferences(VkDevice* logical_device, VkPhysicalDevice* phys_device,
	uint32_t swap_chain_image_count){

	logical_device_reference_ = logical_device;
	physical_device_reference_ = phys_device;
	swap_chain_image_count_ = swap_chain_image_count;

}

// ------------------------------------------------------------------------- //

void Material::initDescriptorSets() {

	createSceneUniformBuffers();
	createModelsUniformBuffers();
	createSpecificUniformBuffers();

	populateSceneDescriptorSets();
	populateModelsDescriptorSets();
	populateSpecificDescriptorSets();

}

// ------------------------------------------------------------------------- //

void Material::createSceneUniformBuffers() {

	scene_uniform_buffers_.resize(swap_chain_image_count_);

	VkDeviceSize buffer_size = sizeof(SceneUBO);

	for (int i = 0; i < swap_chain_image_count_; i++) {
		scene_uniform_buffers_[i] = new Buffer(Buffer::kBufferType_Uniform);
		scene_uniform_buffers_[i]->create(*physical_device_reference_, *logical_device_reference_, buffer_size,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
		scene_uniform_buffers_[i]->map(*logical_device_reference_, buffer_size);
	}

}

// ------------------------------------------------------------------------- //

void Material::createModelsUniformBuffers() {

	models_uniform_buffers_.resize(swap_chain_image_count_);

	// Return false if a requested feature is not supported
	VkPhysicalDeviceProperties device_properties;
	vkGetPhysicalDeviceProperties(*physical_device_reference_, &device_properties);

	// Calculate required alignment  for the ubo based on minimum device offset alignment
	size_t min_ubo_alignment = device_properties.limits.minUniformBufferOffsetAlignment;
	models_dynamic_alignment_ = sizeof(glm::mat4);
	if (min_ubo_alignment > 0) {
		models_dynamic_alignment_ = (models_dynamic_alignment_ +
			min_ubo_alignment - 1) & ~(min_ubo_alignment - 1);
	}

	// Number of 3D objects * dynamic alignment
	size_t buffer_size = ParticleEditor::instance().getScene()->getNumberOfObjects(material_id_) *
		models_dynamic_alignment_;
	if (buffer_size == 0) buffer_size = 1; // prevent errors if there aren't objects of this kind


	// Allocate the memory for the ubo
	models_ubo_.models = (glm::mat4*)alignedAlloc(buffer_size, models_dynamic_alignment_);
	if (models_ubo_.models == nullptr) {
		throw std::runtime_error("\nFailed to allocate dynamic uniform buffer.");
	}

	// Uniform buffer object with per-object matrices
	for (int i = 0; i < swap_chain_image_count_; i++) {
		models_uniform_buffers_[i] = new Buffer(Buffer::kBufferType_Uniform);
		models_uniform_buffers_[i]->create(*physical_device_reference_, *logical_device_reference_, buffer_size,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
		models_uniform_buffers_[i]->map(*logical_device_reference_, buffer_size);
	}

}

// ------------------------------------------------------------------------- //

void Material::populateSceneDescriptorSets() {

	std::vector<VkDescriptorSetLayout> descriptor_set_layouts(swap_chain_image_count_, scene_descriptor_set_layout_);
	VkDescriptorSetAllocateInfo allocate_info{};
	allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocate_info.descriptorPool = scene_descriptor_pool_;
	allocate_info.descriptorSetCount = swap_chain_image_count_;
	allocate_info.pSetLayouts = descriptor_set_layouts.data();

	scene_descriptor_sets_.resize(swap_chain_image_count_);
	if (vkAllocateDescriptorSets(*logical_device_reference_, &allocate_info, scene_descriptor_sets_.data()) != VK_SUCCESS) {
		throw std::runtime_error("\nFailed to create scene descriptor sets.");
	}


	for (int i = 0; i < scene_descriptor_sets_.size(); i++) {
		VkDescriptorBufferInfo buffer_info{};
		buffer_info.buffer = scene_uniform_buffers_[i]->buffer_;
		buffer_info.offset = 0;
		buffer_info.range = sizeof(SceneUBO);

		VkWriteDescriptorSet write_descriptor{};
		write_descriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptor.dstSet = scene_descriptor_sets_[i];
		write_descriptor.dstBinding = 0;
		write_descriptor.dstArrayElement = 0;
		write_descriptor.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		write_descriptor.descriptorCount = 1;
		write_descriptor.pBufferInfo = &buffer_info;
		write_descriptor.pImageInfo = nullptr; // Image data
		write_descriptor.pTexelBufferView = nullptr; // Buffer views

		vkUpdateDescriptorSets(*logical_device_reference_, 1, &write_descriptor, 0, nullptr);
	}

}

// ------------------------------------------------------------------------- //

void Material::populateModelsDescriptorSets() {

	std::vector<VkDescriptorSetLayout> descriptor_set_layouts(swap_chain_image_count_, models_descriptor_set_layout_);
	VkDescriptorSetAllocateInfo allocate_info{};
	allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocate_info.descriptorPool = models_descriptor_pool_;
	allocate_info.descriptorSetCount = swap_chain_image_count_;
	allocate_info.pSetLayouts = descriptor_set_layouts.data();

	models_descriptor_sets_.resize(swap_chain_image_count_);
	if (vkAllocateDescriptorSets(*logical_device_reference_, &allocate_info, models_descriptor_sets_.data()) != VK_SUCCESS) {
		throw std::runtime_error("\nFailed to create models descriptor sets.");
	}


	for (int i = 0; i < models_descriptor_sets_.size(); i++) {
		VkDescriptorBufferInfo buffer_info{};
		buffer_info.buffer = models_uniform_buffers_[i]->buffer_;
		buffer_info.offset = 0;
		buffer_info.range = models_dynamic_alignment_;
		if (ParticleEditor::instance().getScene()->getNumberOfObjects(material_id_) == 0) {
			buffer_info.range = 1;
		}

		VkWriteDescriptorSet write_descriptor{};
		write_descriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptor.dstSet = models_descriptor_sets_[i];
		write_descriptor.dstBinding = 0;
		write_descriptor.dstArrayElement = 0;
		write_descriptor.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		write_descriptor.descriptorCount = 1;
		write_descriptor.pBufferInfo = &buffer_info;
		write_descriptor.pImageInfo = nullptr; // Image data
		write_descriptor.pTexelBufferView = nullptr; // Buffer views

		vkUpdateDescriptorSets(*logical_device_reference_, 1, &write_descriptor, 0, nullptr);
	}

}

// ------------------------------------------------------------------------- //

void Material::updateSceneUBO(int buffer_id) {

	memcpy(scene_uniform_buffers_[buffer_id]->mapped_memory_, &scene_ubo_, sizeof(scene_ubo_));

}

// ------------------------------------------------------------------------- //

void Material::updateModelsUBO(int buffer_id) {

	uint32_t objects = ParticleEditor::instance().getScene()->getNumberOfObjects(material_id_);
	uint32_t size = objects * models_dynamic_alignment_;

	for (int i = 0; i < objects; ++i) {
		// Do the dynamic offset things
		uint32_t offset = (i * models_dynamic_alignment_);
		glm::mat4* mem_offset = (glm::mat4*)(((uint64_t)models_uniform_buffers_[buffer_id]->
			mapped_memory_ + offset));
		glm::mat4* model = (glm::mat4*)(((uint64_t)models_ubo_.models + offset));
		memcpy((void*)mem_offset, *(&model), models_dynamic_alignment_);
	}

	// Flush to make changes visible to the host if visible host flag is not set
	/*VkMappedMemoryRange memoryRange = vks::initializers::mappedMemoryRange();
	memoryRange.memory = uniformBuffers.dynamic.memory;
	memoryRange.size = uniformBuffers.dynamic.size;
	vkFlushMappedMemoryRanges(device, 1, &memoryRange);*/

}

// ------------------------------------------------------------------------- //

void Material::cleanUniformBuffers() {

	for (int i = 0; i < swap_chain_image_count_; i++) {
		scene_uniform_buffers_[i]->unmap(*logical_device_reference_);
		scene_uniform_buffers_[i]->clean(*logical_device_reference_);
		delete scene_uniform_buffers_[i];

		models_uniform_buffers_[i]->unmap(*logical_device_reference_);
		models_uniform_buffers_[i]->clean(*logical_device_reference_);
		delete models_uniform_buffers_[i];

		specific_uniform_buffers_[i]->unmap(*logical_device_reference_);
		specific_uniform_buffers_[i]->clean(*logical_device_reference_);
		delete specific_uniform_buffers_[i];
	}

	scene_uniform_buffers_.clear();
	models_uniform_buffers_.clear();
	specific_uniform_buffers_.clear();

}

// ------------------------------------------------------------------------- //

void Material::cleanMaterialResources(){

	vkDestroyPipeline(*logical_device_reference_, graphics_pipeline_, nullptr);
	vkDestroyPipelineLayout(*logical_device_reference_, pipeline_layout_, nullptr);

	vkDestroyDescriptorPool(*logical_device_reference_, scene_descriptor_pool_, nullptr);
	scene_descriptor_sets_.clear();

	vkDestroyDescriptorPool(*logical_device_reference_, models_descriptor_pool_, nullptr);
	models_descriptor_sets_.clear();
	
	vkDestroyDescriptorPool(*logical_device_reference_, specific_descriptor_pool_, nullptr);
	specific_descriptor_sets_.clear();
	
	cleanUniformBuffers();

}

// ------------------------------------------------------------------------- //

void Material::deleteMaterialResources(){

	vkDestroyDescriptorSetLayout(*logical_device_reference_, scene_descriptor_set_layout_, nullptr);
	vkDestroyDescriptorSetLayout(*logical_device_reference_, models_descriptor_set_layout_, nullptr);
	vkDestroyDescriptorSetLayout(*logical_device_reference_, specific_descriptor_set_layout_, nullptr);

}

// ------------------------------------------------------------------------- //
// ---------------------------- MATERIAL OPAQUE ---------------------------- //
// ------------------------------------------------------------------------- //

OpaqueMaterial::OpaqueMaterial() : Material(){

	material_id_ = 0;

	specific_descriptor_set_layout_ = VK_NULL_HANDLE;
	specific_descriptor_pool_ = VK_NULL_HANDLE;
	specific_descriptor_sets_ = std::vector<VkDescriptorSet>(0);
	specific_uniform_buffers_ = std::vector<Buffer*>(0);

}

// ------------------------------------------------------------------------- //

OpaqueMaterial::~OpaqueMaterial() {

	specific_descriptor_sets_.clear();
	specific_uniform_buffers_.clear();

}

// ------------------------------------------------------------------------- //

void OpaqueMaterial::createDescriptorSetLayout() {

	// SCENE UBO
	VkDescriptorSetLayoutBinding vp_ubo_layout_binding{};
	vp_ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	vp_ubo_layout_binding.binding = 0;
	vp_ubo_layout_binding.descriptorCount = 1;
	vp_ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	vp_ubo_layout_binding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutCreateInfo dsl_scene_create_info{};
	dsl_scene_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	dsl_scene_create_info.bindingCount = 1;
	dsl_scene_create_info.pBindings = &vp_ubo_layout_binding;

	if (vkCreateDescriptorSetLayout(*logical_device_reference_, &dsl_scene_create_info, nullptr, &scene_descriptor_set_layout_) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create scene descriptor set layout.");
	}


	// MODELS UBO
	VkDescriptorSetLayoutBinding models_ubo_layout_binding{};
	models_ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	models_ubo_layout_binding.binding = 0;
	models_ubo_layout_binding.descriptorCount = 1;
	models_ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	models_ubo_layout_binding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutCreateInfo dsl_models_create_info{};
	dsl_models_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	dsl_models_create_info.bindingCount = 1;
	dsl_models_create_info.pBindings = &models_ubo_layout_binding;

	if (vkCreateDescriptorSetLayout(*logical_device_reference_, &dsl_models_create_info, nullptr, &models_descriptor_set_layout_) != VK_SUCCESS) {
		throw std::runtime_error("\nFailed to create models descriptor set layout.");
	}


	// OPAQUE UBO
	// Create the binding for the vertex shader MVP matrices
	VkDescriptorSetLayoutBinding opaque_ubo_layout_binding{};
	opaque_ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	opaque_ubo_layout_binding.binding = 0;
	opaque_ubo_layout_binding.descriptorCount = 1;
	opaque_ubo_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	opaque_ubo_layout_binding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding sampler_layout_binding{};
	sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	sampler_layout_binding.binding = 1;
	sampler_layout_binding.descriptorCount = ParticleEditor::instance().app_data_->loaded_textures_.size();
	sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	sampler_layout_binding.pImmutableSamplers = nullptr;

	// Create the descriptor set layout
	std::array<VkDescriptorSetLayoutBinding, 2> opaque_bindings = { opaque_ubo_layout_binding, sampler_layout_binding };

	VkDescriptorSetLayoutCreateInfo opaque_create_info{};
	opaque_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	opaque_create_info.bindingCount = static_cast<uint32_t>(opaque_bindings.size());
	opaque_create_info.pBindings = opaque_bindings.data();

	if (vkCreateDescriptorSetLayout(*logical_device_reference_, &opaque_create_info, nullptr,
		&specific_descriptor_set_layout_) != VK_SUCCESS) {
		throw std::runtime_error("\nFailed to create opaque descriptor set layout.");
	}

}

// ------------------------------------------------------------------------- //

void OpaqueMaterial::createPipelineLayout() {

	std::array<VkDescriptorSetLayout, 3> opaque_descriptor_set_layouts{
		scene_descriptor_set_layout_, models_descriptor_set_layout_,
		specific_descriptor_set_layout_
	};

	VkPipelineLayoutCreateInfo opaque_layout_info{};
	opaque_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	opaque_layout_info.setLayoutCount = static_cast<uint32_t>(opaque_descriptor_set_layouts.size());
	opaque_layout_info.pSetLayouts = opaque_descriptor_set_layouts.data();
	opaque_layout_info.pushConstantRangeCount = 0;
	opaque_layout_info.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(*logical_device_reference_, &opaque_layout_info, nullptr, &pipeline_layout_) != VK_SUCCESS) {
		throw std::runtime_error("\nFailed to create opaque pipeline layout.");
	}

}

// ------------------------------------------------------------------------- //

void OpaqueMaterial::createGraphicPipeline() {

	auto app_data = ParticleEditor::instance().app_data_;

	// Load shaders
	auto vert_shader_code = readFile("../../../resources/shaders/shaders_spirv/v_default.spv");
	auto frag_shader_code = readFile("../../../resources/shaders/shaders_spirv/f_default.spv");

	VkShaderModule vert_shader_module = createShaderModule(logical_device_reference_, vert_shader_code);
	VkShaderModule frag_shader_module = createShaderModule(logical_device_reference_, frag_shader_code);


	VkPipelineShaderStageCreateInfo vert_shader_stage_info{};
	vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vert_shader_stage_info.module = vert_shader_module;
	vert_shader_stage_info.pName = "main";
	vert_shader_stage_info.pSpecializationInfo = nullptr; // Constants can be defined here

	// Fragment shader num textures in array constant
	VkSpecializationMapEntry entry{};
	entry.constantID = 0;
	entry.offset = 0;
	entry.size = sizeof(int32_t);
	VkSpecializationInfo spec_info{};
	spec_info.mapEntryCount = 1;
	spec_info.pMapEntries = &entry;
	spec_info.dataSize = sizeof(uint32_t);
	uint32_t data = app_data->loaded_textures_.size();
	spec_info.pData = &data;

	VkPipelineShaderStageCreateInfo frag_shader_stage_info{};
	frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	frag_shader_stage_info.module = frag_shader_module;
	frag_shader_stage_info.pName = "main";
	frag_shader_stage_info.pSpecializationInfo = &spec_info; // Constants can be defined here

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
	viewport.width = (float)app_data->swap_chain_extent_.width;
	viewport.height = (float)app_data->swap_chain_extent_.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	// Create scissor rectangle with extent size
	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = app_data->swap_chain_extent_;

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
	rasterizer_state_info.polygonMode = VK_POLYGON_MODE_FILL; // Line to draw wire frame but require a GPU feature
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
	multisample_state_info.rasterizationSamples = app_data->msaa_samples_;
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
	color_blend_attachment.blendEnable = VK_FALSE;
	color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
	// Alpha blending
	color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
	color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
		VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendStateCreateInfo blend_state_info{};
	blend_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	blend_state_info.logicOpEnable = VK_FALSE;
	blend_state_info.logicOp = VK_LOGIC_OP_COPY;
	blend_state_info.attachmentCount = 1;
	blend_state_info.pAttachments = &color_blend_attachment;
	/*blend_state_info.blendConstants[0] = 0.0f;
	blend_state_info.blendConstants[1] = 0.0f;
	blend_state_info.blendConstants[2] = 0.0f;
	blend_state_info.blendConstants[3] = 0.0f;*/

	// Create dynamic settings of the pipeline
	VkDynamicState dynamic_states[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_LINE_WIDTH,
	};

	VkPipelineDynamicStateCreateInfo dynamic_state_info{};
	dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state_info.dynamicStateCount = 2;
	dynamic_state_info.pDynamicStates = dynamic_states;

	// Create the actual graphics pipeline
	VkGraphicsPipelineCreateInfo opaque_pipeline_info{};
	opaque_pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	opaque_pipeline_info.stageCount = 2;
	opaque_pipeline_info.pStages = shader_stages;
	opaque_pipeline_info.pVertexInputState = &vertex_input_info;
	opaque_pipeline_info.pInputAssemblyState = &input_assembly_info;
	opaque_pipeline_info.pViewportState = &viewport_state_info;
	opaque_pipeline_info.pRasterizationState = &rasterizer_state_info;
	opaque_pipeline_info.pMultisampleState = &multisample_state_info;
	opaque_pipeline_info.pDepthStencilState = &depth_stencil_info;
	opaque_pipeline_info.pColorBlendState = &blend_state_info;
	opaque_pipeline_info.pDynamicState = nullptr;
	opaque_pipeline_info.layout = pipeline_layout_;
	opaque_pipeline_info.renderPass = app_data->render_pass_;
	opaque_pipeline_info.subpass = 0;
	opaque_pipeline_info.basePipelineHandle = VK_NULL_HANDLE; // derive from other graphics pipeline with flags VK_PIPELINE_CREATE_DERIVATIVE_BIT
	opaque_pipeline_info.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(*logical_device_reference_, VK_NULL_HANDLE, 1, &opaque_pipeline_info,
		nullptr, &graphics_pipeline_) != VK_SUCCESS) {
		throw std::runtime_error("\nFailed to create the opaque graphics pipeline.");
	}

	// Destroy shader modules as they're not used anymore
	vkDestroyShaderModule(*logical_device_reference_, vert_shader_module, nullptr);
	vkDestroyShaderModule(*logical_device_reference_, frag_shader_module, nullptr);

}

// ------------------------------------------------------------------------- //

void OpaqueMaterial::createDescriptorPools() {

	// Scene
	VkDescriptorPoolSize scene_pool_size{};
	scene_pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	scene_pool_size.descriptorCount = swap_chain_image_count_;

	VkDescriptorPoolCreateInfo scene_dp_create_info{};
	scene_dp_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	scene_dp_create_info.poolSizeCount = 1;
	scene_dp_create_info.pPoolSizes = &scene_pool_size;
	scene_dp_create_info.maxSets = swap_chain_image_count_;

	if (vkCreateDescriptorPool(*logical_device_reference_, &scene_dp_create_info, nullptr, &scene_descriptor_pool_) != VK_SUCCESS) {
		throw std::runtime_error("\nFailed to create scene descriptor pool.");
	}

	// Models
	VkDescriptorPoolSize models_pool_size{};
	models_pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	models_pool_size.descriptorCount = swap_chain_image_count_;

	VkDescriptorPoolCreateInfo models_dp_create_info{};
	models_dp_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	models_dp_create_info.poolSizeCount = 1;
	models_dp_create_info.pPoolSizes = &models_pool_size;
	models_dp_create_info.maxSets = swap_chain_image_count_;

	if (vkCreateDescriptorPool(*logical_device_reference_, &models_dp_create_info, nullptr, &models_descriptor_pool_) != VK_SUCCESS) {
		throw std::runtime_error("\nFailed to create models descriptor pool.");
	}



	// OPAQUE MATERIAL DESCRIPTOR POOL
	int num_textures = ParticleEditor::instance().app_data_->loaded_textures_.size();
	std::vector<VkDescriptorPoolSize> pool_sizes = std::vector<VkDescriptorPoolSize>(1 + num_textures);
	pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	pool_sizes[0].descriptorCount = swap_chain_image_count_;
	for (int i = 1; i < num_textures + 1; ++i) {
		pool_sizes[i].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		pool_sizes[i].descriptorCount = swap_chain_image_count_;
	}

	VkDescriptorPoolCreateInfo create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	create_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
	create_info.pPoolSizes = pool_sizes.data();
	create_info.maxSets = swap_chain_image_count_;

	if (vkCreateDescriptorPool(*logical_device_reference_, &create_info, nullptr, &specific_descriptor_pool_) != VK_SUCCESS) {
		throw std::runtime_error("\nFailed to create opaque descriptor pool.");
	}

}

// ------------------------------------------------------------------------- //

void OpaqueMaterial::updateSpecificUBO(int buffer_id) {

	uint32_t objects = ParticleEditor::instance().getScene()->getNumberOfObjects(material_id_);
	uint32_t size = objects * specific_dynamic_alignment_;

	for (int i = 0; i < objects; ++i) {
		// Do the dynamic offset things
		uint64_t offset = (i * specific_dynamic_alignment_);
		glm::mat4* mem_offset = (glm::mat4*)(((uint64_t)specific_uniform_buffers_[buffer_id]->
			mapped_memory_ + offset));
		glm::mat4* packed_uniform = (glm::mat4*)(((uint64_t)specific_ubo_.packed_uniforms + offset));
		memcpy((void*)mem_offset, *(&packed_uniform), specific_dynamic_alignment_);
	}

	// Flush to make changes visible to the host if visible host flag is not set
	/*VkMappedMemoryRange memoryRange = vks::initializers::mappedMemoryRange();
	memoryRange.memory = uniformBuffers.dynamic.memory;
	memoryRange.size = uniformBuffers.dynamic.size;
	vkFlushMappedMemoryRanges(device, 1, &memoryRange);*/

}

// ------------------------------------------------------------------------- //

void OpaqueMaterial::createSpecificUniformBuffers() {

	specific_uniform_buffers_.resize(swap_chain_image_count_);

	// Return false if a requested feature is not supported
	VkPhysicalDeviceProperties device_properties;
	vkGetPhysicalDeviceProperties(*physical_device_reference_, &device_properties);

	// Calculate required alignment  for the ubo based on minimum device offset alignment
	size_t min_ubo_alignment = device_properties.limits.minUniformBufferOffsetAlignment;
	specific_dynamic_alignment_ = sizeof(glm::mat4);
	if (min_ubo_alignment > 0) {
		specific_dynamic_alignment_ = (specific_dynamic_alignment_ +
			min_ubo_alignment - 1) & ~(min_ubo_alignment - 1);
	}

	// Number of 3D objects * dynamic alignment
	size_t buffer_size = ParticleEditor::instance().getScene()->getNumberOfObjects(material_id_) *
		specific_dynamic_alignment_;
	if (buffer_size == 0) buffer_size = 1; // prevent errors if there aren't objects of this kind

	// Allocate the memory for the ubo (First half of the ubo)
	specific_ubo_.packed_uniforms = (glm::mat4*)alignedAlloc(buffer_size, specific_dynamic_alignment_);
	if (specific_ubo_.packed_uniforms == nullptr) {
		throw std::runtime_error("\nFailed to allocate opaque dynamic uniform buffer.");
	}

	// Uniform buffer object with per-object matrices
	for (int i = 0; i < swap_chain_image_count_; i++) {
		specific_uniform_buffers_[i] = new Buffer(Buffer::kBufferType_Uniform);
		specific_uniform_buffers_[i]->create(*physical_device_reference_, *logical_device_reference_, buffer_size,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
		specific_uniform_buffers_[i]->map(*logical_device_reference_, buffer_size);
	}

}

// ------------------------------------------------------------------------- //

void OpaqueMaterial::populateSpecificDescriptorSets() {

	auto app_data = ParticleEditor::instance().app_data_;

	std::vector<VkDescriptorSetLayout> descriptor_set_layouts(swap_chain_image_count_, specific_descriptor_set_layout_);
	VkDescriptorSetAllocateInfo allocate_info{};
	allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocate_info.descriptorPool = specific_descriptor_pool_;
	allocate_info.descriptorSetCount = swap_chain_image_count_;
	allocate_info.pSetLayouts = descriptor_set_layouts.data();

	specific_descriptor_sets_.resize(swap_chain_image_count_);
	if (vkAllocateDescriptorSets(*logical_device_reference_, &allocate_info, specific_descriptor_sets_.data()) != VK_SUCCESS) {
		throw std::runtime_error("\nFailed to create opaque descriptor sets.");
	}


	for (int i = 0; i < specific_descriptor_sets_.size(); i++) {
		VkDescriptorBufferInfo buffer_info{};
		buffer_info.buffer = specific_uniform_buffers_[i]->buffer_;
		buffer_info.offset = 0;
		buffer_info.range = specific_dynamic_alignment_;
		if (ParticleEditor::instance().getScene()->getNumberOfObjects(0) == 0) {
			buffer_info.range = 1;
		}

		const int num_textures = app_data->loaded_textures_.size();
		std::vector<VkDescriptorImageInfo> image_info = std::vector<VkDescriptorImageInfo>(num_textures);
		for (int j = 0; j < num_textures; ++j) {
			image_info[j] = {};
			image_info[j].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			image_info[j].imageView = app_data->texture_images_[j]->image_view_;
			image_info[j].sampler = app_data->texture_images_[j]->texture_sampler_;
		}

		std::array<VkWriteDescriptorSet, 2> write_descriptors{};
		write_descriptors[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptors[0].dstSet = specific_descriptor_sets_[i];
		write_descriptors[0].dstBinding = 0;
		write_descriptors[0].dstArrayElement = 0;
		write_descriptors[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		write_descriptors[0].descriptorCount = 1;
		write_descriptors[0].pBufferInfo = &buffer_info;
		write_descriptors[0].pImageInfo = nullptr; // Image data
		write_descriptors[0].pTexelBufferView = nullptr; // Buffer views

		write_descriptors[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptors[1].dstSet = specific_descriptor_sets_[i];
		write_descriptors[1].dstBinding = 1;
		write_descriptors[1].dstArrayElement = 0;
		write_descriptors[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		write_descriptors[1].descriptorCount = num_textures;
		write_descriptors[1].pBufferInfo = nullptr; // Buffer data
		write_descriptors[1].pImageInfo = image_info.data();
		write_descriptors[1].pTexelBufferView = nullptr; // Buffer views

		vkUpdateDescriptorSets(*logical_device_reference_, static_cast<uint32_t>(write_descriptors.size()),
			write_descriptors.data(), 0, nullptr);
	}

}

// ------------------------------------------------------------------------- //
// ------------------------- MATERIAL TRANSLUCENT -------------------------- //
// ------------------------------------------------------------------------- //

TranslucentMaterial::TranslucentMaterial() : Material() {

	material_id_ = 1;

	specific_descriptor_set_layout_ = VK_NULL_HANDLE;
	specific_descriptor_pool_ = VK_NULL_HANDLE;
	specific_descriptor_sets_ = std::vector<VkDescriptorSet>(0);
	specific_uniform_buffers_ = std::vector<Buffer*>(0);

}

// ------------------------------------------------------------------------- //

TranslucentMaterial::~TranslucentMaterial() {

	specific_descriptor_sets_.clear();
	specific_uniform_buffers_.clear();

}

// ------------------------------------------------------------------------- //

void TranslucentMaterial::createDescriptorSetLayout() {

	// SCENE UBO
	VkDescriptorSetLayoutBinding vp_ubo_layout_binding{};
	vp_ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	vp_ubo_layout_binding.binding = 0;
	vp_ubo_layout_binding.descriptorCount = 1;
	vp_ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	vp_ubo_layout_binding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutCreateInfo dsl_scene_create_info{};
	dsl_scene_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	dsl_scene_create_info.bindingCount = 1;
	dsl_scene_create_info.pBindings = &vp_ubo_layout_binding;

	if (vkCreateDescriptorSetLayout(*logical_device_reference_, &dsl_scene_create_info, nullptr, &scene_descriptor_set_layout_) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create scene descriptor set layout.");
	}


	// MODELS UBO
	VkDescriptorSetLayoutBinding models_ubo_layout_binding{};
	models_ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	models_ubo_layout_binding.binding = 0;
	models_ubo_layout_binding.descriptorCount = 1;
	models_ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	models_ubo_layout_binding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutCreateInfo dsl_models_create_info{};
	dsl_models_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	dsl_models_create_info.bindingCount = 1;
	dsl_models_create_info.pBindings = &models_ubo_layout_binding;

	if (vkCreateDescriptorSetLayout(*logical_device_reference_, &dsl_models_create_info, nullptr, &models_descriptor_set_layout_) != VK_SUCCESS) {
		throw std::runtime_error("\nFailed to create models descriptor set layout.");
	}


	// TRANSLUCENT UBO
	VkDescriptorSetLayoutBinding translucent_ubo_layout_binding{};
	translucent_ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	translucent_ubo_layout_binding.binding = 0;
	translucent_ubo_layout_binding.descriptorCount = 1;
	translucent_ubo_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	translucent_ubo_layout_binding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding sampler_layout_binding{};
	sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	sampler_layout_binding.binding = 1;
	sampler_layout_binding.descriptorCount = ParticleEditor::instance().app_data_->loaded_textures_.size();
	sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	sampler_layout_binding.pImmutableSamplers = nullptr;

	// Create the descriptor set layout
	std::array<VkDescriptorSetLayoutBinding, 2> translucent_bindings = { translucent_ubo_layout_binding, sampler_layout_binding };

	VkDescriptorSetLayoutCreateInfo translucent_create_info{};
	translucent_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	translucent_create_info.bindingCount = static_cast<uint32_t>(translucent_bindings.size());
	translucent_create_info.pBindings = translucent_bindings.data();

	if (vkCreateDescriptorSetLayout(*logical_device_reference_, &translucent_create_info, nullptr,
		&specific_descriptor_set_layout_) != VK_SUCCESS) {
		throw std::runtime_error("\nFailed to create translucent descriptor set layout.");
	}

}

// ------------------------------------------------------------------------- //

void TranslucentMaterial::createPipelineLayout() {

	std::array<VkDescriptorSetLayout, 3> translucent_descriptor_set_layouts{
		scene_descriptor_set_layout_, models_descriptor_set_layout_,
		specific_descriptor_set_layout_
	};

	VkPipelineLayoutCreateInfo translucent_layout_info{};
	translucent_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	translucent_layout_info.setLayoutCount = static_cast<uint32_t>(translucent_descriptor_set_layouts.size());
	translucent_layout_info.pSetLayouts = translucent_descriptor_set_layouts.data();
	translucent_layout_info.pushConstantRangeCount = 0;
	translucent_layout_info.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(*logical_device_reference_, &translucent_layout_info, nullptr, &pipeline_layout_) != VK_SUCCESS) {
		throw std::runtime_error("\nFailed to create translucent pipeline layout.");
	}

}

// ------------------------------------------------------------------------- //

void TranslucentMaterial::createGraphicPipeline() {

	auto app_data = ParticleEditor::instance().app_data_;

	// Load shaders
	auto vert_shader_code = readFile("../../../resources/shaders/shaders_spirv/v_translucent.spv");
	auto frag_shader_code = readFile("../../../resources/shaders/shaders_spirv/f_translucent.spv");

	VkShaderModule vert_shader_module = createShaderModule(logical_device_reference_, vert_shader_code);
	VkShaderModule frag_shader_module = createShaderModule(logical_device_reference_, frag_shader_code);


	VkPipelineShaderStageCreateInfo vert_shader_stage_info{};
	vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vert_shader_stage_info.module = vert_shader_module;
	vert_shader_stage_info.pName = "main";
	vert_shader_stage_info.pSpecializationInfo = nullptr; // Constants can be defined here

	// Fragment shader num textures constant
	VkSpecializationMapEntry entry{};
	entry.constantID = 0;
	entry.offset = 0;
	entry.size = sizeof(int32_t);
	VkSpecializationInfo spec_info{};
	spec_info.mapEntryCount = 1;
	spec_info.pMapEntries = &entry;
	spec_info.dataSize = sizeof(uint32_t);
	uint32_t data = app_data->loaded_textures_.size();
	spec_info.pData = &data;

	VkPipelineShaderStageCreateInfo frag_shader_stage_info{};
	frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	frag_shader_stage_info.module = frag_shader_module;
	frag_shader_stage_info.pName = "main";
	frag_shader_stage_info.pSpecializationInfo = &spec_info; // Constants can be defined here

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
	viewport.width = (float)app_data->swap_chain_extent_.width;
	viewport.height = (float)app_data->swap_chain_extent_.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	// Create scissor rectangle with extent size
	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = app_data->swap_chain_extent_;

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
	rasterizer_state_info.polygonMode = VK_POLYGON_MODE_FILL; // Line to draw wire frame but require a GPU feature
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
	multisample_state_info.rasterizationSamples = app_data->msaa_samples_;
	multisample_state_info.minSampleShading = 1.0f;
	multisample_state_info.pSampleMask = nullptr;
	multisample_state_info.alphaToCoverageEnable = VK_FALSE;
	multisample_state_info.alphaToOneEnable = VK_FALSE;

	// Create depth and stencil settings for the framebuffer
	VkPipelineDepthStencilStateCreateInfo translucent_depth_stencil_info{};
	translucent_depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	translucent_depth_stencil_info.depthTestEnable = VK_TRUE;
	translucent_depth_stencil_info.depthWriteEnable = VK_TRUE;
	translucent_depth_stencil_info.depthCompareOp = VK_COMPARE_OP_LESS;
	translucent_depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
	translucent_depth_stencil_info.minDepthBounds = 0.0f;
	translucent_depth_stencil_info.maxDepthBounds = 1.0f;
	translucent_depth_stencil_info.stencilTestEnable = VK_FALSE;
	translucent_depth_stencil_info.front = {};
	translucent_depth_stencil_info.back = {};

	// Create color blend settings
	VkPipelineColorBlendAttachmentState translucent_color_blend_attachment{};
	translucent_color_blend_attachment.blendEnable = VK_TRUE;
	translucent_color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	translucent_color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	translucent_color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
	translucent_color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	translucent_color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	translucent_color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
	translucent_color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | 
		VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendStateCreateInfo translucent_blend_state_info{};
	translucent_blend_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	translucent_blend_state_info.logicOpEnable = VK_FALSE;
	translucent_blend_state_info.logicOp = VK_LOGIC_OP_COPY;
	translucent_blend_state_info.attachmentCount = 1;
	translucent_blend_state_info.pAttachments = &translucent_color_blend_attachment;
	translucent_blend_state_info.blendConstants[0] = 0.0f;
	translucent_blend_state_info.blendConstants[1] = 0.0f;
	translucent_blend_state_info.blendConstants[2] = 0.0f;
	translucent_blend_state_info.blendConstants[3] = 0.0f;

	// Create dynamic settings of the pipeline
	VkDynamicState dynamic_states[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_LINE_WIDTH,
	};

	VkPipelineDynamicStateCreateInfo dynamic_state_info{};
	dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state_info.dynamicStateCount = 2;
	dynamic_state_info.pDynamicStates = dynamic_states;

	// Create the actual graphics pipeline
	VkGraphicsPipelineCreateInfo opaque_pipeline_info{};
	opaque_pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	opaque_pipeline_info.stageCount = 2;
	opaque_pipeline_info.pStages = shader_stages;
	opaque_pipeline_info.pVertexInputState = &vertex_input_info;
	opaque_pipeline_info.pInputAssemblyState = &input_assembly_info;
	opaque_pipeline_info.pViewportState = &viewport_state_info;
	opaque_pipeline_info.pRasterizationState = &rasterizer_state_info;
	opaque_pipeline_info.pMultisampleState = &multisample_state_info;
	opaque_pipeline_info.pDepthStencilState = &translucent_depth_stencil_info;
	opaque_pipeline_info.pColorBlendState = &translucent_blend_state_info;
	opaque_pipeline_info.pDynamicState = nullptr;
	opaque_pipeline_info.layout = pipeline_layout_;
	opaque_pipeline_info.renderPass = app_data->render_pass_;
	opaque_pipeline_info.subpass = 0;
	opaque_pipeline_info.basePipelineHandle = VK_NULL_HANDLE; // derive from other graphics pipeline with flags VK_PIPELINE_CREATE_DERIVATIVE_BIT
	opaque_pipeline_info.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(*logical_device_reference_, VK_NULL_HANDLE, 1, &opaque_pipeline_info,
		nullptr, &graphics_pipeline_) != VK_SUCCESS) {
		throw std::runtime_error("\nFailed to create the translucent graphics pipeline.");
	}

	// Destroy shader modules as they're not used anymore
	vkDestroyShaderModule(*logical_device_reference_, vert_shader_module, nullptr);
	vkDestroyShaderModule(*logical_device_reference_, frag_shader_module, nullptr);

}

// ------------------------------------------------------------------------- //

void TranslucentMaterial::createDescriptorPools() {

	// Scene
	VkDescriptorPoolSize scene_pool_size{};
	scene_pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	scene_pool_size.descriptorCount = swap_chain_image_count_;

	VkDescriptorPoolCreateInfo scene_dp_create_info{};
	scene_dp_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	scene_dp_create_info.poolSizeCount = 1;
	scene_dp_create_info.pPoolSizes = &scene_pool_size;
	scene_dp_create_info.maxSets = swap_chain_image_count_;

	if (vkCreateDescriptorPool(*logical_device_reference_, &scene_dp_create_info, nullptr, &scene_descriptor_pool_) != VK_SUCCESS) {
		throw std::runtime_error("\nFailed to create scene descriptor pool.");
	}

	// Models
	VkDescriptorPoolSize models_pool_size{};
	models_pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	models_pool_size.descriptorCount = swap_chain_image_count_;

	VkDescriptorPoolCreateInfo models_dp_create_info{};
	models_dp_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	models_dp_create_info.poolSizeCount = 1;
	models_dp_create_info.pPoolSizes = &models_pool_size;
	models_dp_create_info.maxSets = swap_chain_image_count_;

	if (vkCreateDescriptorPool(*logical_device_reference_, &models_dp_create_info, nullptr, &models_descriptor_pool_) != VK_SUCCESS) {
		throw std::runtime_error("\nFailed to create models descriptor pool.");
	}



	// TRANSLUCENT MATERIAL DESCRIPTOR POOL
	int num_textures = ParticleEditor::instance().app_data_->loaded_textures_.size();
	std::vector<VkDescriptorPoolSize> pool_sizes = std::vector<VkDescriptorPoolSize>(1 + num_textures);
	pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	pool_sizes[0].descriptorCount = swap_chain_image_count_;
	for (int i = 1; i < num_textures + 1; ++i) {
		pool_sizes[i].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		pool_sizes[i].descriptorCount = swap_chain_image_count_;
	}

	VkDescriptorPoolCreateInfo create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	create_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
	create_info.pPoolSizes = pool_sizes.data();
	create_info.maxSets = swap_chain_image_count_;

	if (vkCreateDescriptorPool(*logical_device_reference_, &create_info, nullptr, &specific_descriptor_pool_) != VK_SUCCESS) {
		throw std::runtime_error("\nFailed to create translucent descriptor pool.");
	}

}

// ------------------------------------------------------------------------- //

void TranslucentMaterial::updateSpecificUBO(int buffer_id) {

	uint32_t objects = ParticleEditor::instance().getScene()->getNumberOfObjects(material_id_);
	uint32_t size = objects * specific_dynamic_alignment_;

	for (int i = 0; i < objects; ++i) {
		// Do the dynamic offset things
		uint64_t offset = (i * specific_dynamic_alignment_);
		glm::mat4* mem_offset = (glm::mat4*)(((uint64_t)specific_uniform_buffers_[buffer_id]->
			mapped_memory_ + offset));
		glm::mat4* packed_uniform = (glm::mat4*)(((uint64_t)specific_ubo_.packed_uniforms + offset));
		memcpy((void*)mem_offset, *(&packed_uniform), specific_dynamic_alignment_);
	}

	// Flush to make changes visible to the host if visible host flag is not set
	/*VkMappedMemoryRange memoryRange = vks::initializers::mappedMemoryRange();
	memoryRange.memory = uniformBuffers.dynamic.memory;
	memoryRange.size = uniformBuffers.dynamic.size;
	vkFlushMappedMemoryRanges(device, 1, &memoryRange);*/

}

// ------------------------------------------------------------------------- //

void TranslucentMaterial::createSpecificUniformBuffers() {

	specific_uniform_buffers_.resize(swap_chain_image_count_);

	// Return false if a requested feature is not supported
	VkPhysicalDeviceProperties device_properties;
	vkGetPhysicalDeviceProperties(*physical_device_reference_, &device_properties);

	// Calculate required alignment  for the ubo based on minimum device offset alignment
	size_t min_ubo_alignment = device_properties.limits.minUniformBufferOffsetAlignment;
	specific_dynamic_alignment_ = sizeof(glm::mat4);
	if (min_ubo_alignment > 0) {
		specific_dynamic_alignment_ = (specific_dynamic_alignment_ +
			min_ubo_alignment - 1) & ~(min_ubo_alignment - 1);
	}

	// Number of 3D objects * dynamic alignment
	size_t buffer_size = ParticleEditor::instance().getScene()->getNumberOfObjects(material_id_) *
		specific_dynamic_alignment_;
	if (buffer_size == 0) buffer_size = 1; // prevent errors if there aren't objects of this kind

	// Allocate the memory for the ubo (First half of the ubo)
	specific_ubo_.packed_uniforms = (glm::mat4*)alignedAlloc(buffer_size, specific_dynamic_alignment_);
	if (specific_ubo_.packed_uniforms == nullptr) {
		throw std::runtime_error("\nFailed to allocate translucent dynamic uniform buffer.");
	}

	// Uniform buffer object with per-object matrices
	for (int i = 0; i < swap_chain_image_count_; i++) {
		specific_uniform_buffers_[i] = new Buffer(Buffer::kBufferType_Uniform);
		specific_uniform_buffers_[i]->create(*physical_device_reference_, *logical_device_reference_, buffer_size,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
		specific_uniform_buffers_[i]->map(*logical_device_reference_, buffer_size);
	}

}

// ------------------------------------------------------------------------- //

void TranslucentMaterial::populateSpecificDescriptorSets() {

	auto app_data = ParticleEditor::instance().app_data_;

	std::vector<VkDescriptorSetLayout> descriptor_set_layouts(swap_chain_image_count_, specific_descriptor_set_layout_);
	VkDescriptorSetAllocateInfo allocate_info{};
	allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocate_info.descriptorPool = specific_descriptor_pool_;
	allocate_info.descriptorSetCount = swap_chain_image_count_;
	allocate_info.pSetLayouts = descriptor_set_layouts.data();

	specific_descriptor_sets_.resize(swap_chain_image_count_);
	if (vkAllocateDescriptorSets(*logical_device_reference_, &allocate_info, specific_descriptor_sets_.data()) != VK_SUCCESS) {
		throw std::runtime_error("\nFailed to create translucent descriptor sets.");
	}


	for (int i = 0; i < specific_descriptor_sets_.size(); i++) {
		VkDescriptorBufferInfo buffer_info{};
		buffer_info.buffer = specific_uniform_buffers_[i]->buffer_;
		buffer_info.offset = 0;
		buffer_info.range = specific_dynamic_alignment_;
		if (ParticleEditor::instance().getScene()->getNumberOfObjects(1) == 0) {
			buffer_info.range = 1;
		}

		const int num_textures = app_data->loaded_textures_.size();
		std::vector<VkDescriptorImageInfo> image_info = std::vector<VkDescriptorImageInfo>(num_textures);
		for (int j = 0; j < num_textures; ++j) {
			image_info[j] = {};
			image_info[j].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			image_info[j].imageView = app_data->texture_images_[j]->image_view_;
			image_info[j].sampler = app_data->texture_images_[j]->texture_sampler_;
		}

		std::array<VkWriteDescriptorSet, 2> write_descriptors{};
		write_descriptors[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptors[0].dstSet = specific_descriptor_sets_[i];
		write_descriptors[0].dstBinding = 0;
		write_descriptors[0].dstArrayElement = 0;
		write_descriptors[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		write_descriptors[0].descriptorCount = 1;
		write_descriptors[0].pBufferInfo = &buffer_info;
		write_descriptors[0].pImageInfo = nullptr; // Image data
		write_descriptors[0].pTexelBufferView = nullptr; // Buffer views

		write_descriptors[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptors[1].dstSet = specific_descriptor_sets_[i];
		write_descriptors[1].dstBinding = 1;
		write_descriptors[1].dstArrayElement = 0;
		write_descriptors[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		write_descriptors[1].descriptorCount = num_textures;
		write_descriptors[1].pBufferInfo = nullptr; // Buffer data
		write_descriptors[1].pImageInfo = image_info.data();
		write_descriptors[1].pTexelBufferView = nullptr; // Buffer views

		vkUpdateDescriptorSets(*logical_device_reference_, static_cast<uint32_t>(write_descriptors.size()),
			write_descriptors.data(), 0, nullptr);
	}

}

// ------------------------------------------------------------------------- //
// -------------------------- MATERIAL PARTICLES --------------------------- //
// ------------------------------------------------------------------------- //

ParticlesMaterial::ParticlesMaterial(){

	material_id_ = 2;

	specific_descriptor_set_layout_ = VK_NULL_HANDLE;
	specific_descriptor_pool_ = VK_NULL_HANDLE;
	specific_descriptor_sets_ = std::vector<VkDescriptorSet>(0);
	specific_uniform_buffers_ = std::vector<Buffer*>(0);

}

// ------------------------------------------------------------------------- //

ParticlesMaterial::~ParticlesMaterial(){

	specific_descriptor_sets_.clear();
	specific_uniform_buffers_.clear();

}

// ------------------------------------------------------------------------- //

void ParticlesMaterial::createDescriptorSetLayout(){

	// SCENE UBO
	VkDescriptorSetLayoutBinding vp_ubo_layout_binding{};
	vp_ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	vp_ubo_layout_binding.binding = 0;
	vp_ubo_layout_binding.descriptorCount = 1;
	vp_ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	vp_ubo_layout_binding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutCreateInfo dsl_scene_create_info{};
	dsl_scene_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	dsl_scene_create_info.bindingCount = 1;
	dsl_scene_create_info.pBindings = &vp_ubo_layout_binding;

	if (vkCreateDescriptorSetLayout(*logical_device_reference_, &dsl_scene_create_info, nullptr, &scene_descriptor_set_layout_) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create scene descriptor set layout.");
	}


	// MODELS UBO
	VkDescriptorSetLayoutBinding models_ubo_layout_binding{};
	models_ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	models_ubo_layout_binding.binding = 0;
	models_ubo_layout_binding.descriptorCount = 1;
	models_ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	models_ubo_layout_binding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutCreateInfo dsl_models_create_info{};
	dsl_models_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	dsl_models_create_info.bindingCount = 1;
	dsl_models_create_info.pBindings = &models_ubo_layout_binding;

	if (vkCreateDescriptorSetLayout(*logical_device_reference_, &dsl_models_create_info, nullptr, &models_descriptor_set_layout_) != VK_SUCCESS) {
		throw std::runtime_error("\nFailed to create models descriptor set layout.");
	}


	// PARTICLES UBO
	VkDescriptorSetLayoutBinding particles_ubo_layout_binding{};
	particles_ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	particles_ubo_layout_binding.binding = 0;
	particles_ubo_layout_binding.descriptorCount = 1;
	particles_ubo_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	particles_ubo_layout_binding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding sampler_layout_binding{};
	sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	sampler_layout_binding.binding = 1;
	sampler_layout_binding.descriptorCount = ParticleEditor::instance().app_data_->loaded_textures_.size();
	sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	sampler_layout_binding.pImmutableSamplers = nullptr;

	// Create the descriptor set layout
	std::array<VkDescriptorSetLayoutBinding, 2> particles_bindings = { particles_ubo_layout_binding, sampler_layout_binding };

	VkDescriptorSetLayoutCreateInfo particles_create_info{};
	particles_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	particles_create_info.bindingCount = static_cast<uint32_t>(particles_bindings.size());
	particles_create_info.pBindings = particles_bindings.data();

	if (vkCreateDescriptorSetLayout(*logical_device_reference_, &particles_create_info, nullptr,
		&specific_descriptor_set_layout_) != VK_SUCCESS) {
		throw std::runtime_error("\nFailed to create particles descriptor set layout.");
	}

}

// ------------------------------------------------------------------------- //

void ParticlesMaterial::createPipelineLayout(){

	std::array<VkDescriptorSetLayout, 3> particles_descriptor_set_layouts{
		scene_descriptor_set_layout_, models_descriptor_set_layout_,
		specific_descriptor_set_layout_
	};

	VkPipelineLayoutCreateInfo particles_layout_info{};
	particles_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	particles_layout_info.setLayoutCount = static_cast<uint32_t>(particles_descriptor_set_layouts.size());
	particles_layout_info.pSetLayouts = particles_descriptor_set_layouts.data();
	particles_layout_info.pushConstantRangeCount = 0;
	particles_layout_info.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(*logical_device_reference_, &particles_layout_info, nullptr, &pipeline_layout_) != VK_SUCCESS) {
		throw std::runtime_error("\nFailed to create particles pipeline layout.");
	}

}

// ------------------------------------------------------------------------- //

void ParticlesMaterial::createGraphicPipeline(){

	auto app_data = ParticleEditor::instance().app_data_;

	// Load shaders
	auto vert_shader_code = readFile("../../../resources/shaders/shaders_spirv/v_billboard.spv");
	auto frag_shader_code = readFile("../../../resources/shaders/shaders_spirv/f_billboard.spv");

	VkShaderModule vert_shader_module = createShaderModule(logical_device_reference_, vert_shader_code);
	VkShaderModule frag_shader_module = createShaderModule(logical_device_reference_, frag_shader_code);


	VkPipelineShaderStageCreateInfo vert_shader_stage_info{};
	vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vert_shader_stage_info.module = vert_shader_module;
	vert_shader_stage_info.pName = "main";
	vert_shader_stage_info.pSpecializationInfo = nullptr; // Constants can be defined here

	// Fragment shader num textures constant
	VkSpecializationMapEntry entry{};
	entry.constantID = 0;
	entry.offset = 0;
	entry.size = sizeof(int32_t);
	VkSpecializationInfo spec_info{};
	spec_info.mapEntryCount = 1;
	spec_info.pMapEntries = &entry;
	spec_info.dataSize = sizeof(uint32_t);
	uint32_t data = app_data->loaded_textures_.size();
	spec_info.pData = &data;

	VkPipelineShaderStageCreateInfo frag_shader_stage_info{};
	frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	frag_shader_stage_info.module = frag_shader_module;
	frag_shader_stage_info.pName = "main";
	frag_shader_stage_info.pSpecializationInfo = &spec_info; // Constants can be defined here

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
	viewport.width = (float)app_data->swap_chain_extent_.width;
	viewport.height = (float)app_data->swap_chain_extent_.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	// Create scissor rectangle with extent size
	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = app_data->swap_chain_extent_;

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
	rasterizer_state_info.polygonMode = VK_POLYGON_MODE_FILL; // Line to draw wire frame but require a GPU feature
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
	multisample_state_info.rasterizationSamples = app_data->msaa_samples_;
	multisample_state_info.minSampleShading = 1.0f;
	multisample_state_info.pSampleMask = nullptr;
	multisample_state_info.alphaToCoverageEnable = VK_FALSE;
	multisample_state_info.alphaToOneEnable = VK_FALSE;

	// Create depth and stencil settings for the framebuffer
	VkPipelineDepthStencilStateCreateInfo translucent_depth_stencil_info{};
	translucent_depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	translucent_depth_stencil_info.depthTestEnable = VK_TRUE;
	translucent_depth_stencil_info.depthWriteEnable = VK_TRUE;
	translucent_depth_stencil_info.depthCompareOp = VK_COMPARE_OP_LESS;
	translucent_depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
	translucent_depth_stencil_info.minDepthBounds = 0.0f;
	translucent_depth_stencil_info.maxDepthBounds = 1.0f;
	translucent_depth_stencil_info.stencilTestEnable = VK_FALSE;
	translucent_depth_stencil_info.front = {};
	translucent_depth_stencil_info.back = {};

	// Create color blend settings
	VkPipelineColorBlendAttachmentState translucent_color_blend_attachment{};
	translucent_color_blend_attachment.blendEnable = VK_TRUE;
	translucent_color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	translucent_color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	translucent_color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
	translucent_color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	translucent_color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	translucent_color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
	translucent_color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
		VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendStateCreateInfo translucent_blend_state_info{};
	translucent_blend_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	translucent_blend_state_info.logicOpEnable = VK_FALSE;
	translucent_blend_state_info.logicOp = VK_LOGIC_OP_COPY;
	translucent_blend_state_info.attachmentCount = 1;
	translucent_blend_state_info.pAttachments = &translucent_color_blend_attachment;
	translucent_blend_state_info.blendConstants[0] = 0.0f;
	translucent_blend_state_info.blendConstants[1] = 0.0f;
	translucent_blend_state_info.blendConstants[2] = 0.0f;
	translucent_blend_state_info.blendConstants[3] = 0.0f;

	// Create dynamic settings of the pipeline
	VkDynamicState dynamic_states[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_LINE_WIDTH,
	};

	VkPipelineDynamicStateCreateInfo dynamic_state_info{};
	dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state_info.dynamicStateCount = 2;
	dynamic_state_info.pDynamicStates = dynamic_states;

	// Create the actual graphics pipeline
	VkGraphicsPipelineCreateInfo opaque_pipeline_info{};
	opaque_pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	opaque_pipeline_info.stageCount = 2;
	opaque_pipeline_info.pStages = shader_stages;
	opaque_pipeline_info.pVertexInputState = &vertex_input_info;
	opaque_pipeline_info.pInputAssemblyState = &input_assembly_info;
	opaque_pipeline_info.pViewportState = &viewport_state_info;
	opaque_pipeline_info.pRasterizationState = &rasterizer_state_info;
	opaque_pipeline_info.pMultisampleState = &multisample_state_info;
	opaque_pipeline_info.pDepthStencilState = &translucent_depth_stencil_info;
	opaque_pipeline_info.pColorBlendState = &translucent_blend_state_info;
	opaque_pipeline_info.pDynamicState = nullptr;
	opaque_pipeline_info.layout = pipeline_layout_;
	opaque_pipeline_info.renderPass = app_data->render_pass_;
	opaque_pipeline_info.subpass = 0;
	opaque_pipeline_info.basePipelineHandle = VK_NULL_HANDLE; // derive from other graphics pipeline with flags VK_PIPELINE_CREATE_DERIVATIVE_BIT
	opaque_pipeline_info.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(*logical_device_reference_, VK_NULL_HANDLE, 1, &opaque_pipeline_info,
		nullptr, &graphics_pipeline_) != VK_SUCCESS) {
		throw std::runtime_error("\nFailed to create the particles graphics pipeline.");
	}

	// Destroy shader modules as they're not used anymore
	vkDestroyShaderModule(*logical_device_reference_, vert_shader_module, nullptr);
	vkDestroyShaderModule(*logical_device_reference_, frag_shader_module, nullptr);

}

// ------------------------------------------------------------------------- //

void ParticlesMaterial::createDescriptorPools(){

	// Scene
	VkDescriptorPoolSize scene_pool_size{};
	scene_pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	scene_pool_size.descriptorCount = swap_chain_image_count_;

	VkDescriptorPoolCreateInfo scene_dp_create_info{};
	scene_dp_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	scene_dp_create_info.poolSizeCount = 1;
	scene_dp_create_info.pPoolSizes = &scene_pool_size;
	scene_dp_create_info.maxSets = swap_chain_image_count_;

	if (vkCreateDescriptorPool(*logical_device_reference_, &scene_dp_create_info, nullptr, &scene_descriptor_pool_) != VK_SUCCESS) {
		throw std::runtime_error("\nFailed to create scene descriptor pool.");
	}

	// Models
	VkDescriptorPoolSize models_pool_size{};
	models_pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	models_pool_size.descriptorCount = swap_chain_image_count_;

	VkDescriptorPoolCreateInfo models_dp_create_info{};
	models_dp_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	models_dp_create_info.poolSizeCount = 1;
	models_dp_create_info.pPoolSizes = &models_pool_size;
	models_dp_create_info.maxSets = swap_chain_image_count_;

	if (vkCreateDescriptorPool(*logical_device_reference_, &models_dp_create_info, nullptr, &models_descriptor_pool_) != VK_SUCCESS) {
		throw std::runtime_error("\nFailed to create models descriptor pool.");
	}



	// PARTICLES MATERIAL DESCRIPTOR POOL
	int num_textures = ParticleEditor::instance().app_data_->loaded_textures_.size();
	std::vector<VkDescriptorPoolSize> pool_sizes = std::vector<VkDescriptorPoolSize>(1 + num_textures);
	pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	pool_sizes[0].descriptorCount = swap_chain_image_count_;
	for (int i = 1; i < num_textures + 1; ++i) {
		pool_sizes[i].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		pool_sizes[i].descriptorCount = swap_chain_image_count_;
	}

	VkDescriptorPoolCreateInfo create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	create_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
	create_info.pPoolSizes = pool_sizes.data();
	create_info.maxSets = swap_chain_image_count_;

	if (vkCreateDescriptorPool(*logical_device_reference_, &create_info, nullptr, &specific_descriptor_pool_) != VK_SUCCESS) {
		throw std::runtime_error("\nFailed to create particles descriptor pool.");
	}

}

// ------------------------------------------------------------------------- //

void ParticlesMaterial::updateSpecificUBO(int buffer_id){

	uint32_t objects = ParticleEditor::instance().getScene()->getNumberOfObjects(material_id_);
	uint32_t size = objects * specific_dynamic_alignment_;

	for (int i = 0; i < objects; ++i) {
		// Do the dynamic offset things
		uint64_t offset = (i * specific_dynamic_alignment_);
		glm::mat4* mem_offset = (glm::mat4*)(((uint64_t)specific_uniform_buffers_[buffer_id]->
			mapped_memory_ + offset));
		glm::mat4* packed_uniform = (glm::mat4*)(((uint64_t)specific_ubo_.packed_uniforms + offset));
		memcpy((void*)mem_offset, *(&packed_uniform), specific_dynamic_alignment_);
	}

	// Flush to make changes visible to the host if visible host flag is not set
	/*VkMappedMemoryRange memoryRange = vks::initializers::mappedMemoryRange();
	memoryRange.memory = uniformBuffers.dynamic.memory;
	memoryRange.size = uniformBuffers.dynamic.size;
	vkFlushMappedMemoryRanges(device, 1, &memoryRange);*/

}

// ------------------------------------------------------------------------- //

void ParticlesMaterial::createSpecificUniformBuffers(){

	specific_uniform_buffers_.resize(swap_chain_image_count_);

	// Return false if a requested feature is not supported
	VkPhysicalDeviceProperties device_properties;
	vkGetPhysicalDeviceProperties(*physical_device_reference_, &device_properties);

	// Calculate required alignment  for the ubo based on minimum device offset alignment
	size_t min_ubo_alignment = device_properties.limits.minUniformBufferOffsetAlignment;
	specific_dynamic_alignment_ = sizeof(glm::mat4);
	if (min_ubo_alignment > 0) {
		specific_dynamic_alignment_ = (specific_dynamic_alignment_ +
			min_ubo_alignment - 1) & ~(min_ubo_alignment - 1);
	}

	// Number of 3D objects * dynamic alignment
	size_t buffer_size = ParticleEditor::instance().getScene()->getNumberOfObjects(material_id_) *
		specific_dynamic_alignment_;
	if (buffer_size == 0) buffer_size = 1; // prevent errors if there aren't objects of this kind

	// Allocate the memory for the ubo (First half of the ubo)
	specific_ubo_.packed_uniforms = (glm::mat4*)alignedAlloc(buffer_size, specific_dynamic_alignment_);
	if (specific_ubo_.packed_uniforms == nullptr) {
		throw std::runtime_error("\nFailed to allocate particles dynamic uniform buffer.");
	}

	// Uniform buffer object with per-object matrices
	for (int i = 0; i < swap_chain_image_count_; i++) {
		specific_uniform_buffers_[i] = new Buffer(Buffer::kBufferType_Uniform);
		specific_uniform_buffers_[i]->create(*physical_device_reference_, *logical_device_reference_, buffer_size,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
		specific_uniform_buffers_[i]->map(*logical_device_reference_, buffer_size);
	}

}

// ------------------------------------------------------------------------- //

void ParticlesMaterial::populateSpecificDescriptorSets(){

	auto app_data = ParticleEditor::instance().app_data_;

	std::vector<VkDescriptorSetLayout> descriptor_set_layouts(swap_chain_image_count_, specific_descriptor_set_layout_);
	VkDescriptorSetAllocateInfo allocate_info{};
	allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocate_info.descriptorPool = specific_descriptor_pool_;
	allocate_info.descriptorSetCount = swap_chain_image_count_;
	allocate_info.pSetLayouts = descriptor_set_layouts.data();

	specific_descriptor_sets_.resize(swap_chain_image_count_);
	if (vkAllocateDescriptorSets(*logical_device_reference_, &allocate_info, specific_descriptor_sets_.data()) != VK_SUCCESS) {
		throw std::runtime_error("\nFailed to create particles descriptor sets.");
	}


	for (int i = 0; i < specific_descriptor_sets_.size(); i++) {
		VkDescriptorBufferInfo buffer_info{};
		buffer_info.buffer = specific_uniform_buffers_[i]->buffer_;
		buffer_info.offset = 0;
		buffer_info.range = specific_dynamic_alignment_;
		if (ParticleEditor::instance().getScene()->getNumberOfObjects(2) == 0) {
			buffer_info.range = 1;
		}

		const int num_textures = app_data->loaded_textures_.size();
		std::vector<VkDescriptorImageInfo> image_info = std::vector<VkDescriptorImageInfo>(num_textures);
		for (int j = 0; j < num_textures; ++j) {
			image_info[j] = {};
			image_info[j].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			image_info[j].imageView = app_data->texture_images_[j]->image_view_;
			image_info[j].sampler = app_data->texture_images_[j]->texture_sampler_;
		}

		std::array<VkWriteDescriptorSet, 2> write_descriptors{};
		write_descriptors[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptors[0].dstSet = specific_descriptor_sets_[i];
		write_descriptors[0].dstBinding = 0;
		write_descriptors[0].dstArrayElement = 0;
		write_descriptors[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		write_descriptors[0].descriptorCount = 1;
		write_descriptors[0].pBufferInfo = &buffer_info;
		write_descriptors[0].pImageInfo = nullptr; // Image data
		write_descriptors[0].pTexelBufferView = nullptr; // Buffer views

		write_descriptors[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptors[1].dstSet = specific_descriptor_sets_[i];
		write_descriptors[1].dstBinding = 1;
		write_descriptors[1].dstArrayElement = 0;
		write_descriptors[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		write_descriptors[1].descriptorCount = num_textures;
		write_descriptors[1].pBufferInfo = nullptr; // Buffer data
		write_descriptors[1].pImageInfo = image_info.data();
		write_descriptors[1].pTexelBufferView = nullptr; // Buffer views

		vkUpdateDescriptorSets(*logical_device_reference_, static_cast<uint32_t>(write_descriptors.size()),
			write_descriptors.data(), 0, nullptr);
	}

}

// ------------------------------------------------------------------------- //
