/*
 *	Author: Diego Ochando Torres
 *  Date: 11/01/2021
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

// ------------------------------------------------------------------------- //

#include "systems/system_draw_particles.h"
#include "../src/engine_internal/internal_app_data.h"

#include "components/component_particle_system.h"

// ------------------------------------------------------------------------- //

SystemDrawParticles::SystemDrawParticles(){

	setRequiredArchetype(Entity::kArchetype_ParticleSystem);

}

// ------------------------------------------------------------------------- //

SystemDrawParticles::~SystemDrawParticles() {



}

// ------------------------------------------------------------------------- //

void SystemDrawParticles::drawObjectsCommand(int cmd_buffer_image, VkCommandBuffer& cmd_buffer, 
	std::vector<Entity*>& entities){

	ParticleEditor::AppData* app_data = ParticleEditor::instance().app_data_;
	int index = 0;

	auto material_parent = app_data->materials_[2]; // Opaque material
	// Bind pipeline
	vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		material_parent->graphics_pipeline_);

	// Vertex and index buffers
	int quad_id = 0;
	VkBuffer vertex_buffers[] = {
		app_data->vertex_buffers_[quad_id]->buffer_ };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(cmd_buffer, 0, 1, vertex_buffers, offsets);
	vkCmdBindIndexBuffer(cmd_buffer,
		app_data->index_buffers_[quad_id]->buffer_, 0, VK_INDEX_TYPE_UINT32);

	// Record all the draw commands needed for a 3D object
	for (int i = 0; i < entities.size(); i++) {
		if (hasRequiredComponents(entities[i])) {			

			auto ps = static_cast<ComponentParticleSystem*>
				(entities[i]->getComponent(Component::ComponentKind::kComponentKind_ParticleSystem));

			for (int j = 0; j < ps->getMaxParticles(); ++j) {
				// Dynamic offset things
				uint32_t dynamic_offset = index * static_cast<uint32_t>
					(material_parent->models_dynamic_alignment_);
				uint32_t particles_dynamic_offset = index * static_cast<uint32_t>
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
					2, 1, &material_parent->specific_descriptor_sets_[cmd_buffer_image], 1, &particles_dynamic_offset);

				// Draw
				vkCmdDrawIndexed(cmd_buffer, 6, 1, 0, 0, 0);

				++index;
			}

		}
	}

}

// ------------------------------------------------------------------------- //

void SystemDrawParticles::updateUniformBuffers(int current_image, std::vector<Entity*>& entities) {

	auto material_parent = ParticleEditor::instance().app_data_->materials_[2];

	// Scene ubo only got updated when camera moves
	//Map memory to GPU
	material_parent->updateSceneUBO(current_image);

	// Update model matrices
	material_parent->models_ubo_.models = getParticlesModel(entities);
	// Map the memory from the CPU to GPU
	material_parent->updateModelsUBO(current_image);

	// Update per object uniforms and textures
	material_parent->specific_ubo_.packed_uniforms = getParticlesData(entities);
	// Map the memory from the CPU to GPU
	material_parent->updateSpecificUBO(current_image);

}

// ------------------------------------------------------------------------- //

glm::mat4* SystemDrawParticles::getParticlesModel(std::vector<Entity*>& entities) {

	ParticleEditor::AppData* app_data = ParticleEditor::instance().app_data_;

	glm::mat4* model_mat = nullptr;
	auto material_parent = app_data->materials_[2];
	int index = 0;

	// store all the objects models matrices
	for (auto entity : entities) {
		if (hasRequiredComponents(entity)) {

			auto ps = static_cast<ComponentParticleSystem*>
				(entity->getComponent(Component::ComponentKind::kComponentKind_ParticleSystem));
			auto particles = ps->getAllParticles();

			for (int j = 0; j < ps->getMaxParticles(); ++j) {

				// Do the dynamic offset things
				model_mat = (glm::mat4*)(((uint64_t)material_parent->models_ubo_.models +
					(index * material_parent->models_dynamic_alignment_)));

				// Update matrices
				glm::mat4 aux_model = glm::translate(glm::mat4(1.0f), particles[j]->position_);
				aux_model = glm::scale(aux_model, glm::vec3(0.2f, 0.2f, 0.2f));
				
				*model_mat = aux_model;

				++index;
			}

		}
	}

	model_mat = (glm::mat4*)((uint64_t)material_parent->models_ubo_.models);

	return model_mat;

}

// ------------------------------------------------------------------------- //

glm::mat4* SystemDrawParticles::getParticlesData(std::vector<Entity*>& entities) {

	ParticleEditor::AppData* app_data = ParticleEditor::instance().app_data_;

	glm::mat4* packed_uniforms = nullptr;
	auto material_parent = app_data->materials_[2];
	glm::mat4 aux = glm::mat4(0.0f);
	int index = 0;

	// store all the objects colors and texture_ids
	for (auto entity : entities) {
		if (hasRequiredComponents(entity)) {

			auto ps = static_cast<ComponentParticleSystem*>
				(entity->getComponent(Component::ComponentKind::kComponentKind_ParticleSystem));
			auto particles = ps->getAllParticles();

			for (int j = 0; j < ps->getMaxParticles(); ++j) {

				// Update color
				aux[0] = particles[j]->color_;

				//Update texture ids
				aux[1] = glm::vec4(ps->getTextureID(), -1, -1, -1);

				// Do the dynamic offset things
				packed_uniforms = (glm::mat4*)(((uint64_t)material_parent->specific_ubo_.packed_uniforms
					+ (index * material_parent->specific_dynamic_alignment_)));

				// Update uniforms
				*packed_uniforms = aux;

				++index;

			}

		}
	}

	packed_uniforms = (glm::mat4*)((uint64_t)material_parent->specific_ubo_.packed_uniforms);

	return packed_uniforms;

}

// ------------------------------------------------------------------------- //
