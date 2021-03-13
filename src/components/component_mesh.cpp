/*
 *	Author: Diego Ochando Torres
 *  Date: 31/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

 // ------------------------------------------------------------------------- //

#include "components/component_mesh.h"
#include "../src/internal/internal_app_data.h"

 // ------------------------------------------------------------------------- //

ComponentMesh::ComponentMesh() : Component(Component::kComponentKind_Mesh) {

	mesh_buffer_id_ = -1;

}

// ------------------------------------------------------------------------- //

void ComponentMesh::loadMeshFromFile(const char* model_path){

	// Add model to load it later when empty
	auto it = BasicPSApp::instance().app_data_->loaded_models_.cbegin();
	if (it == BasicPSApp::instance().app_data_->loaded_models_.cend()) {
		// Not any object insert it
		BasicPSApp::instance().app_data_->loaded_models_.insert(
			std::pair<int, const char*>(BasicPSApp::instance().app_data_->default_geometries, model_path));
		// Store new id
		mesh_buffer_id_ = BasicPSApp::instance().app_data_->default_geometries;
		return;
	}

	// Check if model was previously loaded to not load it again
	it = BasicPSApp::instance().app_data_->loaded_models_.cbegin();
	while (it != BasicPSApp::instance().app_data_->loaded_models_.cend()){
		if (!strcmp(model_path, it->second)) {
			printf("\nModel has been loaded earlier. Assigning id");
			mesh_buffer_id_ = it->first;
			return;
		}
		++it;
	}

	// Save it if its not added yet
	BasicPSApp::instance().app_data_->loaded_models_.insert(
		std::pair<int, const char*>(BasicPSApp::instance().app_data_->default_geometries + 
			BasicPSApp::instance().app_data_->loaded_models_.size(), model_path));
	// Store new id
	mesh_buffer_id_ = BasicPSApp::instance().app_data_->default_geometries +
		BasicPSApp::instance().app_data_->loaded_models_.size();

}

// ------------------------------------------------------------------------- //

void ComponentMesh::loadDefaultMesh(BasicPSApp::DefaultMesh default_mesh){

	// it will use one of the default geometries created in the internal resources
	// simply use the parameter as id
	mesh_buffer_id_ = (int)default_mesh;

}

// ------------------------------------------------------------------------- //

ComponentMesh::~ComponentMesh() {

}

// ------------------------------------------------------------------------- //

