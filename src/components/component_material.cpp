/*
 *	Author: Diego Ochando Torres
 *  Date: 31/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

 // ------------------------------------------------------------------------- //

#include "components/component_material.h"
#include "../src/internal/internal_app_data.h"
#include "../src/internal/internal_gpu_resources.h"

#include <stdexcept>

 // ------------------------------------------------------------------------- //

ComponentMaterial::ComponentMaterial() : Component(Component::kComponentKind_Material) {

	parent_id_ = -1;
	instance_data_.opaque_data_ = nullptr;
	instance_data_.translucent_data_ = nullptr;

}

// ------------------------------------------------------------------------- //

void ComponentMaterial::setMaterialParent(BasicPSApp::MaterialParent parent){

	switch (parent) {
	case BasicPSApp::kMaterialParent_Opaque: {
		parent_id_ = 0;
		break;
	};
	case BasicPSApp::kMaterialParent_Translucent: {
		parent_id_ = 1;
		break;
	};
	default: {
		throw std::runtime_error("\n Undefined material parent ID.");
		break;
	};
	}

}

void ComponentMaterial::setInstanceData(MaterialData* instance_data){

	if (instance_data->getParentID() != parent_id_) {
		throw std::runtime_error("\n Material data instance kind not correspond to material parent.");
	}

	switch (parent_id_) {
	case 0: {
		instance_data_.opaque_data_ = static_cast<OpaqueData*>(instance_data);
		break;
	};
	case 1: {
		instance_data_.translucent_data_ = static_cast<TranslucentData*>(instance_data);
		break;
	};
	default: {
		throw std::runtime_error("\n Undefined material parent ID.");
		break;
	};
	}

}

// ------------------------------------------------------------------------- //

ComponentMaterial::MaterialData* ComponentMaterial::getInstanceData(){

	switch (parent_id_) {
	case 0: {
		return instance_data_.opaque_data_;
		break;
	};
	case 1: {
		return instance_data_.translucent_data_;
		break;
	};
	default: {
		throw std::runtime_error("\n Undefined material parent ID.");
		break;
	};
	}

}

// ------------------------------------------------------------------------- //

ComponentMaterial::~ComponentMaterial() {

	switch (parent_id_) {
	case 0: {
		delete instance_data_.opaque_data_;
		break;
	};
	case 1: {
		delete instance_data_.translucent_data_;
		break;
	};
	default: {
		throw std::runtime_error("\n Undefined material parent ID.");
		break;
	};
	}

}

// ------------------------------------------------------------------------- //

int ComponentMaterial::loadTexture(const char* texture_path){

	// Add texture to load it later when empty
	auto it = BasicPSApp::instance().app_data_->loaded_textures_.cbegin();
	if (it == BasicPSApp::instance().app_data_->loaded_textures_.cend()) {
		// Not any object insert it
		BasicPSApp::instance().app_data_->loaded_textures_.insert(
			std::pair<int, const char*>(0, texture_path));
		// return new id
		return 0;
	}

	// Check if texture was previously loaded to not load it again
	it = BasicPSApp::instance().app_data_->loaded_textures_.cbegin();
	while (it != BasicPSApp::instance().app_data_->loaded_textures_.cend()) {
		if (!strcmp(texture_path, it->second)) {
			printf("\Texture has been loaded earlier. Assigning id");
			// Return previous assigned id
			return it->first;
		}
		++it;
	}

	// Save it if its not added yet
	BasicPSApp::instance().app_data_->loaded_textures_.insert(
		std::pair<int, const char*>(BasicPSApp::instance().app_data_->loaded_textures_.size(), 
			texture_path));
	// return new id
	return BasicPSApp::instance().app_data_->loaded_textures_.size() - 1;

}

// ------------------------------------------------------------------------- //

// ------------------------------------------------------------------------- //

// ------------------------------------------------------------------------- //

ComponentMaterial::MaterialData::~MaterialData(){

	BasicPSApp::instance().app_data_->cleanUniformBuffers(uniform_buffers_);

}

// ------------------------------------------------------------------------- //

// ------------------------------------------------------------------------- //

ComponentMaterial::OpaqueData::OpaqueData(){

	parent_id_ = 0;
	texture_ids_ = std::vector<int>(1);

}

// ------------------------------------------------------------------------- //

void ComponentMaterial::OpaqueData::loadAlbedoTexture(const char* texture_path){

	// stored id, and texture saved in singleton for loading them together later
	texture_ids_[0] = loadTexture(texture_path);

}

// ------------------------------------------------------------------------- //

void ComponentMaterial::OpaqueData::populateDescriptorSets(){

	BasicPSApp::instance().app_data_->allocateDescriptorSets(descriptor_sets_, 0);
	int albedo_id = 0;
	//int specular_id = 0;
	//int normal_map_id = 0;

	for (int i = 0; i < descriptor_sets_.size(); i++) {
		VkDescriptorBufferInfo buffer_info{};
		buffer_info.buffer = uniform_buffers_[i]->buffer_;
		buffer_info.offset = 0;
		buffer_info.range = sizeof(UniformBufferObject);

		VkDescriptorImageInfo image_info{};
		image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		image_info.imageView = BasicPSApp::instance().app_data_->
			texture_images_[texture_ids_[albedo_id]]->image_view_;
		image_info.sampler = BasicPSApp::instance().app_data_->
			texture_images_[texture_ids_[albedo_id]]->texture_sampler_;

		std::array<VkWriteDescriptorSet, 2> write_descriptors{};
		write_descriptors[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptors[0].dstSet = descriptor_sets_[i];
		write_descriptors[0].dstBinding = 0;
		write_descriptors[0].dstArrayElement = 0;
		write_descriptors[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		write_descriptors[0].descriptorCount = 1;
		write_descriptors[0].pBufferInfo = &buffer_info;
		write_descriptors[0].pImageInfo = nullptr; // Image data
		write_descriptors[0].pTexelBufferView = nullptr; // Buffer views

		write_descriptors[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptors[1].dstSet = descriptor_sets_[i];
		write_descriptors[1].dstBinding = 1;
		write_descriptors[1].dstArrayElement = 0;
		write_descriptors[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		write_descriptors[1].descriptorCount = 1;
		write_descriptors[1].pBufferInfo = nullptr; // Buffer data
		write_descriptors[1].pImageInfo = &image_info;
		write_descriptors[1].pTexelBufferView = nullptr; // Buffer views

		vkUpdateDescriptorSets(BasicPSApp::instance().app_data_->logical_device_,
			static_cast<uint32_t>(write_descriptors.size()),
			write_descriptors.data(), 0, nullptr);
	}

}

// ------------------------------------------------------------------------- //

// ------------------------------------------------------------------------- //

ComponentMaterial::TranslucentData::TranslucentData(){

	parent_id_ = 1;
	texture_ids_ = std::vector<int>(1);

}

// ------------------------------------------------------------------------- //

void ComponentMaterial::TranslucentData::loadAlbedoTexture(const char* texture_path){

	// stored id, and texture saved in singleton for loading them together later
	texture_ids_[0] = loadTexture(texture_path);

}

// ------------------------------------------------------------------------- //

void ComponentMaterial::TranslucentData::populateDescriptorSets(){

	BasicPSApp::instance().app_data_->allocateDescriptorSets(descriptor_sets_, 1);
	int albedo_id = 0;

	for (int i = 0; i < descriptor_sets_.size(); i++) {
		VkDescriptorBufferInfo buffer_info{};
		buffer_info.buffer = uniform_buffers_[i]->buffer_;
		buffer_info.offset = 0;
		buffer_info.range = sizeof(UniformBufferObject);

		VkDescriptorImageInfo image_info{};
		image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		image_info.imageView = BasicPSApp::instance().app_data_->
			texture_images_[texture_ids_[albedo_id]]->image_view_;
		image_info.sampler = BasicPSApp::instance().app_data_->
			texture_images_[texture_ids_[albedo_id]]->texture_sampler_;

		std::array<VkWriteDescriptorSet, 2> write_descriptors{};
		write_descriptors[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptors[0].dstSet = descriptor_sets_[i];
		write_descriptors[0].dstBinding = 0;
		write_descriptors[0].dstArrayElement = 0;
		write_descriptors[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		write_descriptors[0].descriptorCount = 1;
		write_descriptors[0].pBufferInfo = &buffer_info;
		write_descriptors[0].pImageInfo = nullptr; // Image data
		write_descriptors[0].pTexelBufferView = nullptr; // Buffer views

		write_descriptors[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptors[1].dstSet = descriptor_sets_[i];
		write_descriptors[1].dstBinding = 1;
		write_descriptors[1].dstArrayElement = 0;
		write_descriptors[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		write_descriptors[1].descriptorCount = 1;
		write_descriptors[1].pBufferInfo = nullptr; // Buffer data
		write_descriptors[1].pImageInfo = &image_info;
		write_descriptors[1].pTexelBufferView = nullptr; // Buffer views

		vkUpdateDescriptorSets(BasicPSApp::instance().app_data_->logical_device_, 
			static_cast<uint32_t>(write_descriptors.size()),
			write_descriptors.data(), 0, nullptr);
	}

}