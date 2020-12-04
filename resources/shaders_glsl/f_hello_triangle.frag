/*
 *	Author: Diego Ochando Torres
 *  Date: 02/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 *  Description: Fragment shader for drawing the first triangle with Vulkan
 */

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 frag_color;
// Fragments of the framebuffer at index 0
layout(location = 0) out vec4 out_color;

void main(){
  out_color = vec4(frag_color, 1.0f);
}