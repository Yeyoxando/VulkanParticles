/*
 *	Author: Diego Ochando Torres
 *  Date: 11/01/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

// ------------------------------------------------------------------------- //

#include "systems/system_draw_particles.h"

// ------------------------------------------------------------------------- //

SystemDrawParticles::SystemDrawParticles(){

	setRequiredArchetype(Entity::kArchetype_ParticleSystem);

}

// ------------------------------------------------------------------------- //

void SystemDrawParticles::drawObjectsCommand(VkCommandBuffer cmd_buffer, std::vector<Entity*>& entities){

	// Record all the draw commands needed for a 3D object

	// The same on system draw objects

}

// ------------------------------------------------------------------------- //

void SystemDrawParticles::updateDynamicBuffer(std::vector<Entity*>& entities){



}

// ------------------------------------------------------------------------- //

std::vector<glm::mat4> SystemDrawParticles::getObjectModels(std::vector<Entity*>& entities){

	//for all the particle systems

	//will store all the particles models matrices in a vector
	std::vector<glm::mat4> models = std::vector<glm::mat4>(0);

	return models;

}

// ------------------------------------------------------------------------- //

SystemDrawParticles::~SystemDrawParticles(){



}

// ------------------------------------------------------------------------- //
