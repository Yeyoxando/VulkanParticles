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
    friend class BasicPSApp::AppData;
    friend class SystemDrawObjects;
  public: 

    int getParentID() { return parent_id_; }
		std::vector<Buffer*>& getUniformBuffers() { return uniform_buffers_; }
		std::vector<VkDescriptorSet>& getDescriptorSets() { return descriptor_sets_; }

    virtual void populateDescriptorSets() {}
  
  protected: 
		int parent_id_;
    MaterialData() { parent_id_ = -1; }
    virtual ~MaterialData();
    std::vector<VkDescriptorSet> descriptor_sets_; // one per swap chain image.
    std::vector<Buffer*> uniform_buffers_;
	  //std::vector<Buffer*> light_data_; custom_data_... whatever but same as the other data kinds
		std::vector<int> texture_ids_; // textures array, identified later on the specific shader

  };

  // User can create one to set new material instance values for an opaque material
  struct OpaqueData : MaterialData{
  public:
    OpaqueData();

    // loadUniforms
    void loadAlbedoTexture(const char* texture_path);
    // void loadSpecular
    // void loadNormalMap

    virtual void populateDescriptorSets() override;

	};

	struct TranslucentData : MaterialData {
	public:
    TranslucentData();

    // loadUniforms
		void loadAlbedoTexture(const char* texture_path);
    // This material will only have one texture for instance

    virtual void populateDescriptorSets() override;

	};



  void setMaterialParent(BasicPSApp::MaterialParent parent);

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
