/*
 *	Author: Diego Ochando Torres
 *  Date: 19/03/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

#ifndef __INTERNAL_MATERIALS_H__
#define __INTERNAL_MATERIALS_H__

// ------------------------------------------------------------------------- //

#include <Vulkan/vulkan.h>
#include <vector>
#include <glm/glm.hpp>

#include "particle_editor.h"

// ------------------------------------------------------------------------- //

class Buffer;

// ------------------------------------------------------------------------- //
// ------------------------------- UBOS DATA ------------------------------- //
// ------------------------------------------------------------------------- //

struct SceneUBO {
	glm::mat4 view;
	glm::mat4 projection;
};

struct ModelsUBO {
	glm::mat4* models = nullptr;
};

struct SpecificUBO {
	glm::mat4* packed_uniforms = nullptr;
};

/*struct LightsUBO{
	// Will contain a limited number of lights data
  // For the moment start with one directional in the scene
}*/

// ------------------------------------------------------------------------- //
// ----------------------------- MATERIAL BASE ----------------------------- //
// ------------------------------------------------------------------------- //

class Material {
public:
	virtual ~Material();

	// Set references to save time accessing them from the singleton
	void setInternalReferences(VkDevice* logical_device, VkPhysicalDevice* phys_device, 
		uint32_t swap_chain_image_count);

	// Creates the descriptor layouts a material, implemented on children
	virtual void createDescriptorSetLayout() {}
	// Creates a pipeline layout for the different graphics pipelines, implemented on children
	virtual void createPipelineLayout() {}
	// Creates a graphic pipeline, implemented on children
	virtual void createGraphicPipeline() {}
	// Creates a descriptor pool to allocate the descriptor sets for the uniforms, implemented on children
	virtual void createDescriptorPools() {}

	// Initialize uniform buffers and descriptor sets 
	void initDescriptorSets();

	// Updates the scene uniform buffer
	void updateSceneUBO(int buffer_id);
	// Updates the object models dynamic buffer
	void updateModelsUBO(int buffer_id);
	// Updates the object specific pipeline dynamic buffer
	virtual void updateSpecificUBO(int buffer_id) {}

	// Clean up all the uniform buffers, specific ones must be added in the children virtual function
	virtual void cleanUniformBuffers();
	// Cleans the specific resources for swap chain recreation, children specific resources must be cleared in virtual function
	virtual void cleanMaterialResources();
	// Clean the rest of resources not attached to swap chain
	virtual void deleteMaterialResources();



	VkPipeline graphics_pipeline_;
	VkPipelineLayout pipeline_layout_;

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
	size_t models_dynamic_alignment_;

	// - Specific UBO - (All its functionality is implemented in children classes)
	VkDescriptorSetLayout specific_descriptor_set_layout_;
	VkDescriptorPool specific_descriptor_pool_;
	std::vector<VkDescriptorSet> specific_descriptor_sets_; // one per swap chain image.
	std::vector<Buffer*> specific_uniform_buffers_; // one per swap chain image.
	SpecificUBO specific_ubo_;
	size_t specific_dynamic_alignment_;

protected:
	// Private constructor to only create children classes
	Material();

	// Creates the scene uniform buffer for a material
	void createSceneUniformBuffers();
	// Creates the models uniform dynamic buffers for a material
	void createModelsUniformBuffers();
	// Creates the uniform dynamic buffers for a material, implemented specifically on children
	virtual void createSpecificUniformBuffers() {}

	// Populates the descriptor sets for the scene
	void populateSceneDescriptorSets();
	// Populates the descriptor set for the objects models
	void populateModelsDescriptorSets();
	// Populates the descriptor set for the  pipeline uniforms and textures, implemented specifically on children
	virtual void populateSpecificDescriptorSets() {}

	

	int material_id_;

	// Store a reference to not access continuously to the singleton
	VkDevice* logical_device_reference_;
	VkPhysicalDevice* physical_device_reference_;
	uint32_t swap_chain_image_count_;

};

// ------------------------------------------------------------------------- //
// ---------------------------- MATERIAL OPAQUE ---------------------------- //
// ------------------------------------------------------------------------- //

class OpaqueMaterial : public Material {
public:
	OpaqueMaterial();
	virtual ~OpaqueMaterial();

	// Creates the descriptor layouts for opaque material to upload uniforms to the shader
	virtual void createDescriptorSetLayout() override;
	// Creates a pipeline layout for the opaque graphics pipeline 
	virtual void createPipelineLayout() override;
	// Creates a graphic pipeline for opaque objects
	virtual void createGraphicPipeline() override;
	// Creates a descriptor pool to allocate the descriptor sets for the opaque material uniforms
	virtual void createDescriptorPools() override;

	// Updates the object specific opaque dynamic buffer
	virtual void updateSpecificUBO(int buffer_id) override;

	// Clean up the opaque uniform buffers
	virtual void cleanUniformBuffers() override;
	// Clean up the resources related with the swap chain
	virtual void cleanMaterialResources() override;
	// Clean the rest of resources not attached to swap chain
	virtual void deleteMaterialResources() override;

protected:
	// Creates the opaque uniform dynamic buffers
	virtual void createSpecificUniformBuffers() override;

	// Populates the descriptor set for the opaque pipeline uniforms and textures
	virtual void populateSpecificDescriptorSets() override;

};

// ------------------------------------------------------------------------- //
// ------------------------- MATERIAL TRANSLUCENT -------------------------- //
// ------------------------------------------------------------------------- //

class TranslucentMaterial : public Material {
public:
	TranslucentMaterial();
	virtual ~TranslucentMaterial();

	// Creates the descriptor layouts for translucent material to upload uniforms to the shader
	virtual void createDescriptorSetLayout() override;
	// Creates a pipeline layout for the translucent graphics pipeline 
	virtual void createPipelineLayout() override;
	// Creates a graphic pipeline for translucent objects
	virtual void createGraphicPipeline() override;
	// Creates a descriptor pool to allocate the descriptor sets for the translucent material uniforms
	virtual void createDescriptorPools() override;

	// Updates the object specific translucent dynamic buffer
	virtual void updateSpecificUBO(int buffer_id) override;

	// Clean up the translucent uniform buffers
	virtual void cleanUniformBuffers() override;
	// Clean up the resources related with the swap chain
	virtual void cleanMaterialResources() override;
	// Clean the rest of resources not attached to swap chain
	virtual void deleteMaterialResources() override;

protected:
	// Creates the translucent uniform dynamic buffers
	virtual void createSpecificUniformBuffers() override;
	// Populates the descriptor set for the opaque pipeline uniforms and textures
	virtual void populateSpecificDescriptorSets() override;

};

// ------------------------------------------------------------------------- //
// ------------------------------------------------------------------------- //
// ------------------------------------------------------------------------- //

#endif // __INTERNAL_MATERIALS_H__