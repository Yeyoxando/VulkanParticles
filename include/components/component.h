/*
* Author: Diego Ochando Torres
* Date: 31/12/2020
* e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
*/

#ifndef __COMPONENT_H__
#define __COMPONENT_H__

// ------------------------------------------------------------------------- // 



// ------------------------------------------------------------------------- //

/**
* @brief Base for all the components implemented in the ECS.
*/
class Component {
public:

	/// @brief All the component kinds available for the ECS.
	enum ComponentKind {
		kComponentKind_Transform = 0, // Always included in any created entity by default.
		kComponentKind_Mesh = 1,
		kComponentKind_Material = 2,
		kComponentKind_ParticleSystem = 3,
	};

	/// @brief Special constructor which requires to always pass the component kind as parameter.
	explicit Component(ComponentKind comp_kind) : component_kind_(comp_kind) {};
	virtual ~Component();

	/// @brief Current kind of the component.
	ComponentKind component_kind_;

};

// ------------------------------------------------------------------------- //

#endif // __COMPONENT_H__
