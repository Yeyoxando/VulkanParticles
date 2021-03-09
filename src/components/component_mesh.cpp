/*
 *	Author: Diego Ochando Torres
 *  Date: 31/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

 // ------------------------------------------------------------------------- //

#include "components/component_mesh.h"

 // ------------------------------------------------------------------------- //

ComponentMesh::ComponentMesh() : Component(Component::kComponentKind_Mesh) {

}

// ------------------------------------------------------------------------- //

void ComponentMesh::loadMeshFromFile(const char* model_path){

	// it will check in a map of loaded models (name and id on singleton), 
	// if is not loaded it will call the internal function to do it and store it in the map
	// with the id which indicates the position in the internal vector of buffers.
	// and then the component will store the id for the place of the model in this vector

}

// ------------------------------------------------------------------------- //

void ComponentMesh::loadDefaultMesh(BasicPSApp::DefaultMesh default_mesh){

	// it will load one of the default geometries created in the internal resources

}

// ------------------------------------------------------------------------- //

ComponentMesh::~ComponentMesh() {

}

// ------------------------------------------------------------------------- //

