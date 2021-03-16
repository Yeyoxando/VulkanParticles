/*
 *	Author: Diego Ochando Torres
 *  Date: 02/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 *  Description: Fragment shader to learn how Vulkan works
 */

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 frag_color;
layout(location = 1) in vec2 frag_tex_coord;

/*layout(set = 2, binding = 0) uniform OpaqueUBO{
	vec4 color;
} opaque_ubo;*/

layout(set = 2, binding = 1) uniform sampler2D tex_sampler;

// Fragments of the framebuffer at index 0
layout(location = 0) out vec4 out_color;

void main(){
  out_color = texture(tex_sampler, frag_tex_coord) /* opaque_ubo.color*/;
}