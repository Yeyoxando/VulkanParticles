/*
* Author: Diego Ochando Torres
* Date: 31/12/2020
* e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
*/

#ifndef __COMPONENT_MESH_H__
#define __COMPONENT_MESH_H__

// ------------------------------------------------------------------------- // 

#include "component.h"
#include "basic_ps_app.h"

// ------------------------------------------------------------------------- //

class ComponentMesh : public Component {
public:
  ComponentMesh();
  
  void loadMeshFromFile(const char* model_path);

  void loadDefaultMesh(BasicPSApp::DefaultMesh default_mesh);

protected:
  ~ComponentMesh();

  /*
  - Will have a index and vertex buffer associated with the same id to the internal vector of models ( stored as vertex and index buffers)
  */
  int mesh_buffer_id_;

};

// ------------------------------------------------------------------------- //

#endif // __COMPONENT_MESH_H__
