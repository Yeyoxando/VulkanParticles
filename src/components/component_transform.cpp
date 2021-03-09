/*
* Author: Diego Ochando Torres
* Date: 31/12/2020
* e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
*/


#include "components/component_transform.h"

// ------------------------------------------------------------------------- //

ComponentTransform::ComponentTransform() : Component(Component::kComponentKind_Transform) {

}

// ------------------------------------------------------------------------- //

glm::mat4 ComponentTransform::getModelMatrix(){

	return glm::mat4();

}

// ------------------------------------------------------------------------- //

ComponentTransform::~ComponentTransform() {

}

// ------------------------------------------------------------------------- //

void ComponentTransform::computeModelMatrix(){

}

// ------------------------------------------------------------------------- //