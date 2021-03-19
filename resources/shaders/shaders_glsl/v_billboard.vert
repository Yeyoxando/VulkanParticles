/*
 *	Author: Diego Ochando Torres
 *  Date: 24/12/2020
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

	vec3 cam_right_world = vec3(scene_ubo.view[0][0], scene_ubo.view[1][0], scene_ubo.view[2][0]);
	vec3 cam_up_world = vec3(scene_ubo.view[0][1], scene_ubo.view[1][1], scene_ubo.view[2][1]);
	vec3 particle_center = vec3(0.0f);

	vec3 vertex_pos = particle_center + 
	cam_right_world * in_position.x + 
	cam_up_world * in_position.y;
	
	gl_Position = scene_ubo.proj * scene_ubo.view * models_ubo.model * vec4(vertex_pos, 1.0f);

}