/*
 *	Author: Diego Ochando Torres
 *  Date: 08/01/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

#ifndef __INTERNAL_COMMANDS_H__
#define __INTERNAL_COMMANDS_H__

 // ------------------------------------------------------------------------- //

#include "vulkan/vulkan.h"

// ------------------------------------------------------------------------- //

// Begin a list of single time commands and return it
static VkCommandBuffer beginSingleTimeCommands(VkDevice logical_device, VkCommandPool cmd_pool) {

  // Temporary command buffer to perform the operations
  VkCommandBufferAllocateInfo temp_cmd_buffer_info{};
  temp_cmd_buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  temp_cmd_buffer_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  temp_cmd_buffer_info.commandPool = cmd_pool;
  temp_cmd_buffer_info.commandBufferCount = 1;

  VkCommandBuffer temp_command_buffer;
  vkAllocateCommandBuffers(logical_device, &temp_cmd_buffer_info, &temp_command_buffer);

  // Start recording the cmd buffer
  VkCommandBufferBeginInfo begin_info{};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  vkBeginCommandBuffer(temp_command_buffer, &begin_info);

  return temp_command_buffer;

}

// ------------------------------------------------------------------------- //

// Closes the list of single time commands
static void endSingleTimeCommands(VkDevice logical_device, VkCommandPool cmd_pool, VkCommandBuffer cmd_buffer, VkQueue submit_queue) {

  // Finish recording command buffer
  vkEndCommandBuffer(cmd_buffer);

  // Submit the buffer to execution
  VkSubmitInfo submit_info{};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &cmd_buffer;

  // Graphics queue support transfer commands, not necessary to have a separated queue
  vkQueueSubmit(submit_queue, 1, &submit_info, VK_NULL_HANDLE);
  vkQueueWaitIdle(submit_queue);

  // Free the temp command buffer
  vkFreeCommandBuffers(logical_device, cmd_pool, 1, &cmd_buffer);

}

// ------------------------------------------------------------------------- //

#endif // __INTERNAL_COMMANDS_H__