/*
 *	Author: Diego Ochando Torres
 *  Date: 02/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 *  Description: Vertex shader for drawing the first triangle with Vulkan
 *		Fixed vertex buffer to skip its creation for now.
 */

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 in_position;
layout(location = 1) in vec3 in_color;

layout(location = 0) out vec3 frag_color;

void main(){

	frag_color = in_color;
	gl_Position = vec4(in_position, 0.0f, 1.0f);

}