/*
 *	Author: Diego Ochando Torres
 *  Date: 24/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 *  Description: Vertex shader to learn how Vulkan works
 */

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject{
	mat4 model;
	mat4 view;
	mat4 proj;
} ubo;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;
layout(location = 2) in vec2 in_tex_coord;

layout(location = 0) out vec3 frag_color;
layout(location = 1) out vec2 frag_tex_coord;

void main(){

	frag_color = in_color;
	frag_tex_coord = in_tex_coord;

	vec3 cam_right_world = vec3(ubo.view[0][0], ubo.view[1][0], ubo.view[2][0]);
	vec3 cam_up_world = vec3(ubo.view[0][1], ubo.view[1][1], ubo.view[2][1]);
	vec3 particle_center = vec3(0.0f);

	vec3 vertex_pos = particle_center + 
	cam_right_world * in_position.x + 
	cam_up_world * in_position.y;
	
	gl_Position = ubo.proj * ubo.view * ubo.model * vec4(vertex_pos, 1.0f);

}