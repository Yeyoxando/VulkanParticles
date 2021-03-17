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

class Buffer;

class ComponentMaterial : public Component{
public:
  ComponentMaterial();

	// Not usable, base for the material instance data
	// This is the material instance (data correspond to its parent descriptor set layout)
  struct MaterialData {
    friend class ParticleEditor::AppData;
    friend class SystemDrawObjects;
  public: 

    int getParentID() { return parent_id_; }
    virtual glm::vec4 getTextureIDs() { return glm::vec4(); }
  
		int parent_id_;
    MaterialData() { parent_id_ = -1; }
    virtual ~MaterialData();
	  std::vector<int> texture_ids_; // textures array, identified later on the specific shader

  };

  // User can create one to set new material instance values for an opaque material
  struct OpaqueData : MaterialData{
  public:
    OpaqueData();

    void loadAlbedoTexture(const char* texture_path);
    // void loadSpecular
    // void loadNormalMap

    virtual glm::vec4 getTextureIDs() override;

    glm::vec4 color_ = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	};

	struct TranslucentData : MaterialData {
	public:
    TranslucentData();

    // loadUniforms
		void loadAlbedoTexture(const char* texture_path);
    // This material will only have one texture for instance

    virtual glm::vec4 getTextureIDs() override;

	};



  void setMaterialParent(ParticleEditor::MaterialParent parent);

  void setInstanceData(MaterialData* instance_data);

  // return the id of the material parent
  int getID() { return parent_id_; }
  MaterialData* getInstanceData();

protected:
  ~ComponentMaterial();
  
  static int loadTexture(const char* texture_path);

  int parent_id_;

  union InstanceData {
    OpaqueData* opaque_data_;
    TranslucentData* translucent_data_;
  } instance_data_;

};

// ------------------------------------------------------------------------- //

#endif // __COMPONENT_MATERIAL_H__
