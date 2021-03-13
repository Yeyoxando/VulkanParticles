/*
 *	Author: Diego Ochando Torres
 *  Date: 09/01/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

// ------------------------------------------------------------------------- //

#include "systems/system_draw_objects.h"
#include "basic_ps_app.h"
#include "../src/internal/internal_app_data.h"
#include "components/component_material.h"

// ------------------------------------------------------------------------- //

SystemDrawObjects::SystemDrawObjects(){

	setRequiredArchetype(Entity::kArchetype_3DObject);

}

// ------------------------------------------------------------------------- //

void SystemDrawObjects::drawObjectsCommand(VkCommandBuffer cmd_buffer, std::vector<Entity*> &entities){

	// This is an example to see that draw commands can be done like this, i think :)
	
	/*auto first_entity = BasicPSApp::instance().active_scene_->getEntities()[0];
	auto mesh = static_cast<ComponentMesh*>(first_entity->getComponent(Component::kComponentKind_Mesh));

	// The draw system must return this list of commands for each object
	VkBuffer vertex_buffers[] = { vertex_buffers_[mesh->getMeshID()]->buffer_ };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(command_buffers_[i], 0, 1, vertex_buffers, offsets);
	vkCmdBindIndexBuffer(command_buffers_[i], index_buffers_[mesh->getMeshID()]->buffer_, 0, VK_INDEX_TYPE_UINT32);

	vkCmdBindPipeline(command_buffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_);

	// Per object (uniforms and draw)
	//for (int i = 0; i < entities.size(); i++) {
	vkCmdBindDescriptorSets(command_buffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
		pipeline_layout_, 0, 1, &descriptor_sets_[i], 0, nullptr);

	vkCmdDrawIndexed(command_buffers_[i], index_buffers_[mesh->getMeshID()]->data_count_, 1, 0, 0, 0);
	*/

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

void SystemDrawObjects::resetUniformBuffers(std::vector<Entity*>& entities){

	/*for (auto entity : entities) {
		if (hasRequiredComponents(entity)) {
			auto material = static_cast<ComponentMaterial*>(entity->getComponent(Component::kComponentKind_Material));
			BasicPSApp::instance().app_data_->createUniformBuffers(material->getInstanceData()->uniform_buffers_);
		}
	}*/

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
