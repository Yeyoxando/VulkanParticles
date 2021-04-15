/*
 *	Author: Diego Ochando Torres
 *  Date: 31/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

 // ------------------------------------------------------------------------- //

#include "components/component_material.h"
#include "../src/engine_internal/internal_app_data.h"
#include "../src/engine_internal/internal_gpu_resources.h"

#include <stdexcept>

 // ------------------------------------------------------------------------- //

ComponentMaterial::ComponentMaterial() : Component(Component::kComponentKind_Material) {

	parent_id_ = -1;
	instance_data_.opaque_data_ = nullptr;
	instance_data_.translucent_data_ = nullptr;

}

// ------------------------------------------------------------------------- //

void ComponentMaterial::setMaterialParent(ParticleEditor::MaterialParent parent){

	switch (parent) {
	case ParticleEditor::kMaterialParent_Opaque: {
		parent_id_ = 0;
		break;
	};
	case ParticleEditor::kMaterialParent_Translucent: {
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

	auto app_data = ParticleEditor::instance().app_data_;

	// Add texture to load it later when empty
	auto it = app_data->loaded_textures_.cbegin();
	if (it == app_data->loaded_textures_.cend()) {
		// Not any object insert it
		app_data->loaded_textures_.insert(
			std::pair<int, const char*>(0, texture_path));
		// return new id
		return 0;
	}

	// Check if texture was previously loaded to not load it again
	it = app_data->loaded_textures_.cbegin();
	while (it != app_data->loaded_textures_.cend()) {
		if (!strcmp(texture_path, it->second)) {
			printf("\nTexture has been loaded earlier. Assigning correspondent id.");
			// Return previous assigned id
			return it->first;
		}
		++it;
	}

	// Save it if its not added yet
	app_data->loaded_textures_.insert(
		std::pair<int, const char*>(app_data->loaded_textures_.size(),
			texture_path));

	// return new id
	return app_data->loaded_textures_.size() - 1;

}

// ------------------------------------------------------------------------- //

// ------------------------------------------------------------------------- //

// ------------------------------------------------------------------------- //

ComponentMaterial::MaterialData::~MaterialData(){

	

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

glm::vec4 ComponentMaterial::OpaqueData::getTextureIDs(){

	return glm::vec4(texture_ids_[0], -1, -1, -1);

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

glm::vec4 ComponentMaterial::TranslucentData::getTextureIDs(){

	return glm::vec4(texture_ids_[0], -1, -1, -1);

}

// ------------------------------------------------------------------------- //
