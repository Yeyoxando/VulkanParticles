/*
 *	Author: Diego Ochando Torres
 *  Date: 09/01/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

// ------------------------------------------------------------------------- //

#include "systems/system_draw_objects.h"

// ------------------------------------------------------------------------- //

SystemDrawObjects::SystemDrawObjects(){



}

// ------------------------------------------------------------------------- //

void SystemDrawObjects::drawObjectsCommand(VkCommandBuffer cmd_buffer){

	// This is an example to see that draw commands can be done like this, i think :)
	/*VkBufferCopy copy_region{};
	copy_region.srcOffset = 0;
	copy_region.dstOffset = 0;
	copy_region.size = 0;
	vkCmdCopyBuffer(command_buffer, src_buffer.buffer_, buffer_, 1, &copy_region);*/

}

// ------------------------------------------------------------------------- //

void SystemDrawObjects::updateDynamicBuffer(){



}

// ------------------------------------------------------------------------- //

std::vector<glm::mat4> SystemDrawObjects::getObjectModels(){

	//will store all the objects models matrices in a vector
	std::vector<glm::mat4> models = std::vector<glm::mat4>(0);

	return models;

}

// ------------------------------------------------------------------------- //

SystemDrawObjects::~SystemDrawObjects(){



}

// ------------------------------------------------------------------------- //
