/*
* Author: Diego Ochando Torres
* Date: 31/12/2020
* e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
*/

#ifndef __COMPONENT_MESH_H__
#define __COMPONENT_MESH_H__

// ------------------------------------------------------------------------- // 

#include "component.h"
#include "particle_editor.h"

// ------------------------------------------------------------------------- //

/**
* @brief Mesh component for the 3D objects, it contains a reference to a 3D mesh.
*/
class ComponentMesh : public Component {
public:
	ComponentMesh();

	/// @brief Add a model to load it internally if it is not previously added to load and store its id.
	void loadMeshFromFile(const char* model_path);
	/// @brief Load an internal engine predefined mesh.
	void loadDefaultMesh(ParticleEditor::DefaultMesh default_mesh);

	int getID() { return mesh_buffer_id_; }

protected:
	~ComponentMesh();

	/// @brief Id referencing to a position of the internal loaded meshes vector.
	int mesh_buffer_id_;

};

// ------------------------------------------------------------------------- //

#endif // __COMPONENT_MESH_H__
