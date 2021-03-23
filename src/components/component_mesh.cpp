/*
 *	Author: Diego Ochando Torres
 *  Date: 31/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

 // ------------------------------------------------------------------------- //

#include "components/component_mesh.h"
#include "../src/engine_internal/internal_app_data.h"

 // ------------------------------------------------------------------------- //

ComponentMesh::ComponentMesh() : Component(Component::kComponentKind_Mesh) {

	mesh_buffer_id_ = -1;

}

// ------------------------------------------------------------------------- //

void ComponentMesh::loadMeshFromFile(const char* model_path){

	auto app_data = ParticleEditor::instance().app_data_;

	// Add model to load it later when empty
	auto it = app_data->loaded_models_.cbegin();
	if (it == app_data->loaded_models_.cend()) {
		// Not any object insert it
		app_data->loaded_models_.insert(
			std::pair<int, const char*>(app_data->default_geometries, model_path));
		// Store new id
		mesh_buffer_id_ = app_data->default_geometries;
		return;
	}

	// Check if model was previously loaded to not load it again
	it = app_data->loaded_models_.cbegin();
	while (it != app_data->loaded_models_.cend()){
		if (!strcmp(model_path, it->second)) {
			printf("\nModel has been loaded earlier. Assigning correspondent id.");
			mesh_buffer_id_ = it->first;
			return;
		}
		++it;
	}

	// Save it if its not added yet
	app_data->loaded_models_.insert(
		std::pair<int, const char*>(app_data->default_geometries +
			app_data->loaded_models_.size(), model_path));
	// Store new id
	mesh_buffer_id_ = app_data->default_geometries +
		app_data->loaded_models_.size();

}

// ------------------------------------------------------------------------- //

void ComponentMesh::loadDefaultMesh(ParticleEditor::DefaultMesh default_mesh){

	// it will use one of the default geometries created in the engine_internal resources
	// simply use the parameter as id
	mesh_buffer_id_ = (int)default_mesh;

}

// ------------------------------------------------------------------------- //

ComponentMesh::~ComponentMesh() {

}

// ------------------------------------------------------------------------- //

