/*
 *	Author: Diego Ochando Torres
 *  Date: 31/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

 // ------------------------------------------------------------------------- //

#include "components/component_material.h"

 // ------------------------------------------------------------------------- //

ComponentMaterial::ComponentMaterial() : Component(Component::kComponentKind_Material) {

}

// ------------------------------------------------------------------------- //

void ComponentMaterial::setMaterialParent(BasicPSApp::MaterialParent parent){

	switch (parent) {
	case BasicPSApp::kMaterialParent_Opaque: {
		parent_id_ = 0;
		break;
	};
	case BasicPSApp::kMaterialParent_Translucent: {
		parent_id_ = 1;
		break;
	};
	default: {
		// assert()
		break;
	};
	}

}

void ComponentMaterial::setInstanceData(MaterialData* instance_data){

	if (instance_data->parent_id_ != parent_id_) {
		// assert
	}

	switch (parent_id_) {
	case 0: {
		// cast mat data to mat parent kind 1
		// convert and store data in this class pointers
		break;
	};
	case 1: {
		// cast mat data to mat parent kind 2
		// convert and store data in this class pointers
		break;
	};
	default: {
		// assert()
		break;
	};
	}

}

// ------------------------------------------------------------------------- //

ComponentMaterial::~ComponentMaterial() {

}

// ------------------------------------------------------------------------- //

