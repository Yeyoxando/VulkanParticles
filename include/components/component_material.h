/*
* Author: Diego Ochando Torres
* Date: 31/12/2020
* e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
*/

#ifndef __COMPONENT_MATERIAL_H__
#define __COMPONENT_MATERIAL_H__

// ------------------------------------------------------------------------- // 

#include "component.h"
#include "basic_ps_app.h"

// ------------------------------------------------------------------------- //

/*

  - Like unreal engine material structure

  - Will have a pipeline and a descriptor set associated to it (Material parent)
  - Uniform buffer data will be associated to material instances, as well as a reference to material parent

*/

// Should this be the material instance?
class ComponentMaterial : public Component{
public:
  ComponentMaterial();

  // Example (These structs will be internal and interpreted depending on mat parent)
  struct MaterialData { int parent_id_; };
  struct Mat1 : MaterialData{};

  void setMaterialParent(BasicPSApp::MaterialParent parent);

  void setInstanceData(MaterialData* instance_data);

  // should be another only for model matrix? to update in runtime (as vp will be by scene)

protected:
  ~ComponentMaterial();
  
  int parent_id_;

  // This is the material instance (data correspond to its parent descriptor set)
  float* data_; // uniforms packed and unpacked later on the specific shader 
  int* texture_data_; // textures array, identified later on the specific shader

  //float* light_data_; custom_data_... whatever but same as the other data kinds



  // This is internal (Material parent)
  /*int pipeline_id_;
  int descriptor_set_id_;*/

};

// ------------------------------------------------------------------------- //

#endif // __COMPONENT_MATERIAL_H__
