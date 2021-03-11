/*
 *	Author: Diego Ochando Torres
 *  Date: 11/01/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

// ------------------------------------------------------------------------- //

#include "systems/system_draw_particles.h"

// ------------------------------------------------------------------------- //

SystemDrawParticles::SystemDrawParticles(){



}

// ------------------------------------------------------------------------- //

void SystemDrawParticles::drawObjectsCommand(VkCommandBuffer cmd_buffer){

	// This is an example to see that draw commands can be done like this, i think :)
	/*VkBufferCopy copy_region{};
	copy_region.srcOffset = 0;
	copy_region.dstOffset = 0;
	copy_region.size = 0;
	vkCmdCopyBuffer(command_buffer, src_buffer.buffer_, buffer_, 1, &copy_region);*/

}

// ------------------------------------------------------------------------- //

void SystemDrawParticles::updateDynamicBuffer(){



}

// ------------------------------------------------------------------------- //

std::vector<glm::mat4> SystemDrawParticles::getObjectModels(){

	//for all the particle systems

	//will store all the particles models matrices in a vector
	std::vector<glm::mat4> models = std::vector<glm::mat4>(0);

	return models;

}

// ------------------------------------------------------------------------- //

SystemDrawParticles::~SystemDrawParticles(){



}

// ------------------------------------------------------------------------- //
