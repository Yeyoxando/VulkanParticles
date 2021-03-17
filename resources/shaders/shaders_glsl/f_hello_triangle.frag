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
	vec4 texture_ids;
} opaque_ubo;*/

layout(set = 2, binding = 0) uniform OpaqueUBO{
	mat4 packed_uniforms;
} opaque_ubo;

#define color opaque_ubo.packed_uniforms[0]
#define texture_ids opaque_ubo.packed_uniforms[1]

layout(constant_id = 0) const int NUM_TEXTURES = 1;
layout(set = 2, binding = 1) uniform sampler2D tex_sampler[NUM_TEXTURES];

// Fragments of the framebuffer at index 0
layout(location = 0) out vec4 out_color;

void main(){
  int albedo_id = int(texture_ids.x);

  out_color = texture(tex_sampler[albedo_id], frag_tex_coord) * color;
}