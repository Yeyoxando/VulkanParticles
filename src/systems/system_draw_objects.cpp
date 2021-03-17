/*
 *	Author: Diego Ochando Torres
 *  Date: 09/01/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

// ------------------------------------------------------------------------- //

#include "systems/system_draw_objects.h"
#include "../src/internal/internal_app_data.h"
#include "components/component_mesh.h"
#include "components/component_material.h"
#include "components/component_transform.h"

// ------------------------------------------------------------------------- //

SystemDrawObjects::SystemDrawObjects(){

	setRequiredArchetype(Entity::kArchetype_3DObject); 

}

// ------------------------------------------------------------------------- //

void SystemDrawObjects::drawObjectsCommand(int cmd_buffer_image, VkCommandBuffer& cmd_buffer, std::vector<Entity*>& entities) {

	ParticleEditor::AppData* app_data = ParticleEditor::instance().app_data_;
	int index = 0;

	// Record all the draw commands needed for a 3D object
	for (int i = 0; i < entities.size(); i++) {
		if (hasRequiredComponents(entities[i])) {

			auto mesh = static_cast<ComponentMesh*>(entities[i]->getComponent(Component::kComponentKind_Mesh));
			auto material = static_cast<ComponentMaterial*>(entities[i]->getComponent(Component::kComponentKind_Material));

			// Vertex and index buffers
			VkBuffer vertex_buffers[] = {
				app_data->vertex_buffers_[mesh->getID()]->buffer_ };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(cmd_buffer, 0, 1, vertex_buffers, offsets);
			vkCmdBindIndexBuffer(cmd_buffer,
				app_data->index_buffers_[mesh->getID()]->buffer_, 0, VK_INDEX_TYPE_UINT32);

			// Bind pipeline
			vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
				app_data->materials_[material->getID()]->graphics_pipeline_);

			uint32_t dynamic_offset = index * static_cast<uint32_t>(dynamic_alignment_);
			uint32_t opaque_dynamic_offset = index * static_cast<uint32_t>(opaque_dynamic_alignment_);
			// Bind descriptor set (update is not here)
			vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
				app_data->materials_[material->getID()]->pipeline_layout_, 
				0, 1, &app_data->scene_descriptor_sets_[cmd_buffer_image], 0, nullptr);
			vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
				app_data->materials_[material->getID()]->pipeline_layout_, 
				1, 1, &app_data->models_descriptor_sets_[cmd_buffer_image], 1, &dynamic_offset);
			vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
				app_data->materials_[material->getID()]->pipeline_layout_, 
				2, 1, &app_data->opaque_descriptor_sets_[cmd_buffer_image], 1, &opaque_dynamic_offset);
				
			// Draw
			vkCmdDrawIndexed(cmd_buffer,
				app_data->index_buffers_[mesh->getID()]->data_count_, 1, 0, 0, 0);

			++index;
		}
	}

}

// ------------------------------------------------------------------------- //

void SystemDrawObjects::updateUniformBuffers(int current_image, std::vector<Entity*> &entities){

	ParticleEditor::AppData* app_data = ParticleEditor::instance().app_data_;

	// Fill the ubo with the updated data
	SceneUBO scene_ubo{};
	scene_ubo.view = ParticleEditor::instance().getCamera()->getViewMatrix();
	scene_ubo.projection = ParticleEditor::instance().getCamera()->getProjectionMatrix();
	//Map memory to GPU
	app_data->updateUniformBuffer(scene_ubo, app_data->scene_uniform_buffers_[current_image]);

	// Update model matrices
	app_data->models_ubo_.models = getObjectModels(entities);

	// Map the memory from the CPU to GPU
	app_data->updateUniformBuffer(app_data->models_ubo_, getNumberOfObjects(entities), 
		app_data->models_uniform_buffers_[current_image]);

	// Update per object uniforms and textures
	app_data->opaque_ubo_.packed_uniforms = getObjectOpaqueData(entities);

	// Map the memory from the CPU to GPU
	app_data->updateUniformBuffer(app_data->opaque_ubo_, getNumberOfObjects(entities),
		app_data->opaque_uniform_buffers_[current_image]);

}

// ------------------------------------------------------------------------- //

int SystemDrawObjects::getNumberOfObjects(std::vector<Entity*>& entities){

	int objects = 0;

	for (auto entity : entities) {
		if (hasRequiredComponents(entity)) {
			objects++;
		}
	}

	return objects;

}

// ------------------------------------------------------------------------- //

glm::mat4* SystemDrawObjects::getObjectModels(std::vector<Entity*> &entities){

	ParticleEditor::AppData* app_data = ParticleEditor::instance().app_data_;

	glm::mat4* model_mat = nullptr;
	int index = 0;

	// store all the objects models matrices
	for (auto entity : entities) {
		if (hasRequiredComponents(entity)) {

			auto transform = static_cast<ComponentTransform*>
				(entity->getComponent(Component::ComponentKind::kComponentKind_Transform));

			// Do the dynamic offset things
			model_mat = (glm::mat4*)(((uint64_t)app_data->models_ubo_.models + (index * dynamic_alignment_)));

			// Update matrices
			*model_mat = transform->getModelMatrix();

			++index;

		}
	}

	model_mat = (glm::mat4*)((uint64_t)app_data->models_ubo_.models);

	return model_mat;

}

// ------------------------------------------------------------------------- //

glm::mat4* SystemDrawObjects::getObjectOpaqueData(std::vector<Entity*>& entities){

	ParticleEditor::AppData* app_data = ParticleEditor::instance().app_data_;

	glm::mat4* packed_uniforms = nullptr;
	glm::mat4 aux = glm::mat4(0.0f);
	int index = 0;

	// store all the objects colors and texture_ids
	for (auto entity : entities) {
		if (hasRequiredComponents(entity)) {
		
			auto material = static_cast<ComponentMaterial*>
				(entity->getComponent(Component::ComponentKind::kComponentKind_Material));
			auto opaque_data = static_cast<ComponentMaterial::OpaqueData*>(material->getInstanceData());

			// Update color
			aux[0] = opaque_data->color_;
			
			//Update texture ids
			aux[1] = opaque_data->getTextureIDs();

			// Do the dynamic offset things
			packed_uniforms = (glm::mat4*)(((uint64_t)app_data->opaque_ubo_.packed_uniforms + (index * opaque_dynamic_alignment_)));

			// Update uniforms
			*packed_uniforms = aux;

			++index;

		}
	}

	packed_uniforms = (glm::mat4*)((uint64_t)app_data->opaque_ubo_.packed_uniforms);

	return packed_uniforms;

}

// ------------------------------------------------------------------------- //

SystemDrawObjects::~SystemDrawObjects(){



}

// ------------------------------------------------------------------------- //
