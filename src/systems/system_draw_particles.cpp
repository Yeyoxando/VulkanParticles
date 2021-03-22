/*
 *	Author: Diego Ochando Torres
 *  Date: 11/01/2021
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

// ------------------------------------------------------------------------- //

#include "systems/system_draw_particles.h"

// ------------------------------------------------------------------------- //

SystemDrawParticles::SystemDrawParticles(){

	setRequiredArchetype(Entity::kArchetype_ParticleSystem);

}

// ------------------------------------------------------------------------- //

void SystemDrawParticles::drawObjectsCommand(int cmd_buffer_image, VkCommandBuffer& cmd_buffer, 
	std::vector<Entity*>& entities){



}

// ------------------------------------------------------------------------- //

void SystemDrawParticles::updateUniformBuffers(int current_image, std::vector<Entity*>& entities) {



}

// ------------------------------------------------------------------------- //

glm::mat4* SystemDrawParticles::getObjectModels(std::vector<Entity*>& entities) {

	return nullptr;

}

// ------------------------------------------------------------------------- //

glm::mat4* SystemDrawParticles::getObjectTranslucentData(std::vector<Entity*>& entities) {

	return nullptr;

}

// ------------------------------------------------------------------------- //

SystemDrawParticles::~SystemDrawParticles(){



}

// ------------------------------------------------------------------------- //
