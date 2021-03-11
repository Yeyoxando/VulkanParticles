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

	// it will check in a map of loaded models (name and id on singleton), 
	// if is not loaded it will call the internal function to do it and store it in the map
	// with the id which indicates the position in the internal vector of buffers.
	// and then the component will store the id for the place of the model in this vector
	auto it = BasicPSApp::instance().app_data_->loaded_models_.cbegin();
	if (it == BasicPSApp::instance().app_data_->loaded_models_.cend()) {
		// Not any object insert it
		// Load it on internal buffers
		// Store new id
		mesh_buffer_id_ = 1;
	}
	else {
		// Compare the ones in the vector
		// If is not insert it 
		// load it
		// Store new id
	}

	// the vector of buffers will be shared, models loaded will be put at the end of the default geometries

	

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

