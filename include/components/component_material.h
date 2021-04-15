/*
* Author: Diego Ochando Torres
* Date: 31/12/2020
* e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
*/

#ifndef __COMPONENT_MATERIAL_H__
#define __COMPONENT_MATERIAL_H__

// ------------------------------------------------------------------------- // 

#include "component.h"
#include "particle_editor.h"

// ------------------------------------------------------------------------- //

class Buffer;

/**
* @brief Material component for the 3D objects, it only contains data related to its material parent.
*        All GPU resources are managed internally and used depending on the component data.
*/
class ComponentMaterial : public Component {
public:

	// --- MATERIAL KINDS DATA DEFINITION ---
	/**
	* @brief Not usable, base for the material instance data functionality.
	*        This is the material instance (data correspond to its parent GPU resources).
	*/
	struct MaterialData {
	public:
		MaterialData() { parent_id_ = -1; }
		virtual ~MaterialData();

		int getParentID() { return parent_id_; }
		virtual glm::vec4 getTextureIDs() { return glm::vec4(); }

		/// @brief Used to check if the component parent ID is equal to this one to prevent user errors.
		int parent_id_;
		/// @brief Color used to tint objects, white is set by default.
		glm::vec4 color_ = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		/// @brief Store the texture ids used in this material instance, identified later on the specific shader.
		std::vector<int> texture_ids_;


		friend struct ParticleEditor::AppData;

	};

	/// @brief Users can create one to set new material instance values for an opaque material.
	struct OpaqueData : public MaterialData {
	public:
		OpaqueData();

		/// @brief Stores the internal ID of the array of loaded textures for the albedo texture.
		void loadAlbedoTexture(const char* texture_path);
		// void loadSpecular
		// void loadNormalMap

		/// @return Used texture ids packed in a vec 4.
		virtual glm::vec4 getTextureIDs() override;

	};

	/// @brief Users can create one to set new material instance values for a translucent material.
	struct TranslucentData : public MaterialData {
	public:
		TranslucentData();

		/// @brief Stores the internal ID of the array of loaded textures for the albedo texture.
		void loadAlbedoTexture(const char* texture_path);
		/// @return Used texture ids packed in a vec 4.
		virtual glm::vec4 getTextureIDs() override;

	};


	// --- MATERIAL COMPONENT FUNCTIONALITY AND DATA ---
	ComponentMaterial();

	/// @brief Set the material parent kind for the component.
	void setMaterialParent(ParticleEditor::MaterialParent parent);
	/// @brief Set the data for the material, it has to be a child class of MaterialData.
	void setInstanceData(MaterialData* instance_data);

	/// @return Id of the material parent.
	int getID() { return parent_id_; }
	/// @return Data for the current material kind.
	MaterialData* getInstanceData();

protected:
	~ComponentMaterial();

	/** @brief Add a texture to load it internally if it is not previously added to load.
	 *  @return Internal ID corresponding to a unique loaded texture, repeated if it was previously loaded.
	 */
	static int loadTexture(const char* texture_path);

	/// @brief Id of the internal material parent to use the proper GPU resources.
	int parent_id_;

	/// @brief Union to keep the data as small as possible, actual data for the material instance.
	union InstanceData {
		OpaqueData* opaque_data_;
		TranslucentData* translucent_data_;
	} instance_data_;

};

// ------------------------------------------------------------------------- //

#endif // __COMPONENT_MATERIAL_H__
