/*
 *	Author: Diego Ochando Torres
 *  Date: 02/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 *  Description: Vertex shader to learn how Vulkan works
 */

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform SceneUBO{
	mat4 view;
	mat4 proj;
} scene_ubo;

layout(set = 1, binding = 0) uniform ModelsUBO{
	mat4 model;
} models_ubo;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_tex_coord;

layout(location = 0) out vec2 frag_tex_coord;

void main(){

	frag_tex_coord = in_tex_coord;

	gl_Position = scene_ubo.proj * scene_ubo.view * models_ubo.model * vec4(in_position, 1.0f);

}