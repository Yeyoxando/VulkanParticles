/*
 *	Author: Diego Ochando Torres
 *  Date: 09/01/2021
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

 // ------------------------------------------------------------------------- //

#include "systems/system_draw_translucents.h"
#include "../src/engine_internal/internal_app_data.h"
#include "components/component_mesh.h"
#include "components/component_material.h"
#include "components/component_transform.h"

// ------------------------------------------------------------------------- //

SystemDrawTranslucents::SystemDrawTranslucents() {

	setRequiredArchetype(Entity::kArchetype_3DObject);

}

// ------------------------------------------------------------------------- //

SystemDrawTranslucents::~SystemDrawTranslucents() {



}

// ------------------------------------------------------------------------- //

void SystemDrawTranslucents::drawObjectsCommand(int cmd_buffer_image, VkCommandBuffer& cmd_buffer,
	std::vector<Entity*>& entities) {

	ParticleEditor::AppData* app_data = ParticleEditor::instance().app_data_;
	int index = 0;

	auto material_parent = app_data->materials_[1]; // Opaque material
	// Bind pipeline
	vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		material_parent->graphics_pipeline_);

	// Record all the draw commands needed for a 3D object
	for (int i = 0; i < entities.size(); i++) {
		if (hasRequiredComponents(entities[i])) {

			auto mesh = static_cast<ComponentMesh*>(entities[i]->
				getComponent(Component::kComponentKind_Mesh));

			// Vertex and index buffers
			VkBuffer vertex_buffers[] = {
				app_data->vertex_buffers_[mesh->getID()]->buffer_ };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(cmd_buffer, 0, 1, vertex_buffers, offsets);
			vkCmdBindIndexBuffer(cmd_buffer,
				app_data->index_buffers_[mesh->getID()]->buffer_, 0, VK_INDEX_TYPE_UINT32);

			uint32_t dynamic_offset = index * static_cast<uint32_t>
				(material_parent->models_dynamic_alignment_);
			uint32_t translucent_dynamic_offset = index * static_cast<uint32_t>
				(material_parent->specific_dynamic_alignment_);

			// Bind descriptor set (update is not here)
			vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
				material_parent->pipeline_layout_,
				0, 1, &material_parent->scene_descriptor_sets_[cmd_buffer_image], 0, nullptr);
			vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
				material_parent->pipeline_layout_,
				1, 1, &material_parent->models_descriptor_sets_[cmd_buffer_image], 1, &dynamic_offset);
			vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
				material_parent->pipeline_layout_,
				2, 1, &material_parent->specific_descriptor_sets_[cmd_buffer_image], 1, &translucent_dynamic_offset);

			// Draw
			vkCmdDrawIndexed(cmd_buffer,
				app_data->index_buffers_[mesh->getID()]->data_count_, 1, 0, 0, 0);

			++index;

		}
	}

}

// ------------------------------------------------------------------------- //

void SystemDrawTranslucents::updateUniformBuffers(int current_image, std::vector<Entity*>& entities) {

	auto material_parent = ParticleEditor::instance().app_data_->materials_[1];

	// Scene ubo only got updated when camera moves
	//Map memory to GPU
	material_parent->updateSceneUBO(current_image);

	// Update model matrices
	material_parent->models_ubo_.models = getObjectModels(entities);
	// Map the memory from the CPU to GPU
	material_parent->updateModelsUBO(current_image);

	// Update per object uniforms and textures
	material_parent->specific_ubo_.packed_uniforms = getObjectTranslucentData(entities);
	// Map the memory from the CPU to GPU
	material_parent->updateSpecificUBO(current_image);

}

// ------------------------------------------------------------------------- //

glm::mat4* SystemDrawTranslucents::getObjectModels(std::vector<Entity*>& entities) {

	ParticleEditor::AppData* app_data = ParticleEditor::instance().app_data_;

	glm::mat4* model_mat = nullptr;
	auto material_parent = app_data->materials_[1];
	int index = 0;

	// store all the objects models matrices
	for (auto entity : entities) {
		if (hasRequiredComponents(entity)) {

			auto transform = static_cast<ComponentTransform*>
				(entity->getComponent(Component::ComponentKind::kComponentKind_Transform));

			// Do the dynamic offset things
			model_mat = (glm::mat4*)(((uint64_t)material_parent->models_ubo_.models +
				(index * material_parent->models_dynamic_alignment_)));

			// Update matrices
			*model_mat = transform->getModelMatrix();

			++index;

		}
	}

	model_mat = (glm::mat4*)((uint64_t)material_parent->models_ubo_.models);

	return model_mat;

}

// ------------------------------------------------------------------------- //

glm::mat4* SystemDrawTranslucents::getObjectTranslucentData(std::vector<Entity*>& entities) {

	ParticleEditor::AppData* app_data = ParticleEditor::instance().app_data_;

	glm::mat4* packed_uniforms = nullptr;
	auto material_parent = app_data->materials_[1];
	glm::mat4 aux = glm::mat4(0.0f);
	int index = 0;

	// store all the objects colors and texture_ids
	for (auto entity : entities) {
		if (hasRequiredComponents(entity)) {

			auto material = static_cast<ComponentMaterial*>
				(entity->getComponent(Component::ComponentKind::kComponentKind_Material));
			auto translucent_data = static_cast<ComponentMaterial::TranslucentData*>
				(material->getInstanceData());

			// Update color
			aux[0] = translucent_data->color_;

			//Update texture ids
			aux[1] = translucent_data->getTextureIDs();

			// Do the dynamic offset things
			packed_uniforms = (glm::mat4*)(((uint64_t)material_parent->specific_ubo_.packed_uniforms
				+ (index * material_parent->specific_dynamic_alignment_)));

			// Update uniforms
			*packed_uniforms = aux;

			++index;

		}
	}

	packed_uniforms = (glm::mat4*)((uint64_t)material_parent->specific_ubo_.packed_uniforms);

	return packed_uniforms;

}

// ------------------------------------------------------------------------- //
