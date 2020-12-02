/*
 *	Author: Diego Ochando Torres
 *  Date: 02/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 *  Description: Vertex shader for drawing the first triangle with Vulkan
 *		Fixed vertex buffer to skip its creation for now.
 */

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec3 frag_color;

vec3 colors[3] = {
	vec3(1.0f, 0.0f, 0.0f),
	vec3(0.0f, 1.0f, 0.0f),
	vec3(0.0f, 0.0f, 1.0f)
};

vec2 positions[3] = {
	vec2( 0.0f, -0.5f),
	vec2( 0.5f,  0.5f),
	vec2(-0.5f,  0.5f)
};


void main(){
	frag_color = colors[gl_VertexIndex];
	gl_Position = vec4(positions[gl_VertexIndex], 0.0f, 1.0f);
}