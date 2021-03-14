/*
 *	Author: Diego Ochando Torres
 *  Date: 09/01/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

// ------------------------------------------------------------------------- //

#include "systems/system_draw_objects.h"
#include "basic_ps_app.h"
#include "../src/internal/internal_app_data.h"
#include "components/component_mesh.h"
#include "components/component_material.h"

// ------------------------------------------------------------------------- //

SystemDrawObjects::SystemDrawObjects(){

	setRequiredArchetype(Entity::kArchetype_3DObject);

}

// ------------------------------------------------------------------------- //

void SystemDrawObjects::drawObjectsCommand(int cmd_buffer_image, VkCommandBuffer& cmd_buffer, std::vector<Entity*>& entities) {

	// This is an example to see that draw commands can be done like this, i think :)
	for (auto entity : entities) {
		if (hasRequiredComponents(entity)) {

			auto mesh = static_cast<ComponentMesh*>(entity->getComponent(Component::kComponentKind_Mesh));
			auto material = static_cast<ComponentMaterial*>(entity->getComponent(Component::kComponentKind_Material));

			// Vertex and index buffers
			VkBuffer vertex_buffers[] = {
				BasicPSApp::instance().app_data_->vertex_buffers_[mesh->getID()]->buffer_ };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(cmd_buffer, 0, 1, vertex_buffers, offsets);
			vkCmdBindIndexBuffer(cmd_buffer,
				BasicPSApp::instance().app_data_->index_buffers_[mesh->getID()]->buffer_, 0, VK_INDEX_TYPE_UINT32);

			// Bind pipeline
			vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
				BasicPSApp::instance().app_data_->materials_[material->getID()]->graphics_pipeline_);

			// Initialize uniform buffers and descriptor sets if they're not
			if (material->getInstanceData()->getDescriptorSets().size() == 0) {
				BasicPSApp::instance().app_data_->createUniformBuffers(material->getInstanceData()->uniform_buffers_);
				material->getInstanceData()->populateDescriptorSets();
			}

			// Bind descriptor set (Uniform and texture data, update is not here)
			vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
				BasicPSApp::instance().app_data_->materials_[material->getID()]->pipeline_layout_, 0, 1,
				&material->getInstanceData()->getDescriptorSets()[cmd_buffer_image], 0, nullptr);

			// Draw
			vkCmdDrawIndexed(cmd_buffer,
				BasicPSApp::instance().app_data_->index_buffers_[mesh->getID()]->data_count_, 1, 0, 0, 0);

		}
	}

}

// ------------------------------------------------------------------------- //

void SystemDrawObjects::updateDynamicBuffer(std::vector<Entity*> &entities){



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

std::vector<glm::mat4> SystemDrawObjects::getObjectModels(std::vector<Entity*> &entities){

	//will store all the objects models matrices in a vector
	std::vector<glm::mat4> models = std::vector<glm::mat4>(0);

	return models;

}

// ------------------------------------------------------------------------- //

SystemDrawObjects::~SystemDrawObjects(){



}

// ------------------------------------------------------------------------- //
