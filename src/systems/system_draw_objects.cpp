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

	BasicPSApp::AppData* app_data = BasicPSApp::instance().app_data_;
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


			// Initialize uniform buffers and descriptor sets if they're not
			if (app_data->scene_descriptor_sets_.size() == 0) {
				app_data->createUniformBuffers(sizeof(SceneUBO), app_data->scene_uniform_buffers_);
				app_data->createDynamicUniformBuffers(app_data->models_uniform_buffers_);
				app_data->populateSceneDescriptorSets();
				app_data->populateModelsDescriptorSets();
			}
			if (material->getInstanceData()->getDescriptorSets().size() == 0) {
				app_data->createUniformBuffers(sizeof(OpaqueUBO), material->getInstanceData()->uniform_buffers_);
				material->getInstanceData()->populateDescriptorSets();
			}


			uint32_t dynamic_offset = index * static_cast<uint32_t>(dynamic_alignment_);
			// Bind descriptor set (update is not here)
			vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
				app_data->materials_[material->getID()]->pipeline_layout_, 
				0, 1, &app_data->scene_descriptor_sets_[cmd_buffer_image], 0, nullptr);
			vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
				app_data->materials_[material->getID()]->pipeline_layout_, 
				1, 1, &app_data->models_descriptor_sets_[cmd_buffer_image], 1, &dynamic_offset);
			vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
				app_data->materials_[material->getID()]->pipeline_layout_, 
				2, 1, &material->getInstanceData()->getDescriptorSets()[cmd_buffer_image], 0, nullptr);
				
			// Draw
			vkCmdDrawIndexed(cmd_buffer,
				app_data->index_buffers_[mesh->getID()]->data_count_, 1, 0, 0, 0);

			++index;
		}
	}

}

// ------------------------------------------------------------------------- //

void SystemDrawObjects::updateUniformBuffers(int current_image, std::vector<Entity*> &entities){

	BasicPSApp::AppData* app_data = BasicPSApp::instance().app_data_;

	// Fill the ubo with the updated data
	SceneUBO scene_ubo{};
	scene_ubo.view = BasicPSApp::instance().getCamera()->getViewMatrix();
	scene_ubo.projection = BasicPSApp::instance().getCamera()->getProjectionMatrix();
	//Map memory to GPU
	app_data->updateUniformBuffer(scene_ubo, app_data->scene_uniform_buffers_[current_image]);

	// Update model matrices
	app_data->models_ubo_.models = getObjectModels(entities);

	// Map the memory from the CPU to GPU
	app_data->updateUniformBuffer(app_data->models_ubo_, getNumberOfObjects(entities), 
		app_data->models_uniform_buffers_[current_image]);

	// Update opaque pipeline buffers
	for (auto entity : entities) {
		if (hasRequiredComponents(entity)) {

			auto material = static_cast<ComponentMaterial*>
				(entity->getComponent(Component::kComponentKind_Material));
			auto material_data = material->getInstanceData();

			OpaqueUBO opaque_ubo{};
			opaque_ubo.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

			app_data->updateUniformBuffer(opaque_ubo, material_data->getUniformBuffers()[current_image]);
		}
	}

}

// ------------------------------------------------------------------------- //

void SystemDrawObjects::deleteUniformBuffers(std::vector<Entity*>& entities){

	for (auto entity : entities) {
		if (hasRequiredComponents(entity)) {
			auto material = static_cast<ComponentMaterial*>(entity->getComponent(Component::kComponentKind_Material));
			BasicPSApp::instance().app_data_->cleanUniformBuffers(material->getInstanceData()->uniform_buffers_);
			material->getInstanceData()->getDescriptorSets().clear();
		}
	}

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

	BasicPSApp::AppData* app_data = BasicPSApp::instance().app_data_;

	glm::mat4* model_mat = nullptr;
	int index = 0;

	//will store all the objects models matrices in a vector
	// Prepare the uniform buffer for the scene's 3D objects
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

	return model_mat;

}

// ------------------------------------------------------------------------- //

SystemDrawObjects::~SystemDrawObjects(){



}

// ------------------------------------------------------------------------- //
