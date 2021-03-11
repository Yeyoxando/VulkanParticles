/*
* Author: Diego Ochando Torres
* Date: 31/12/2020
* e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
*/

#ifndef __COMPONENT_PARTICLE_SYSTEM_H__
#define __COMPONENT_PARTICLE_SYSTEM_H__

// ------------------------------------------------------------------------- // 

#include <vector>

#include "component.h"
#include "particles_test/particle.h"

// ------------------------------------------------------------------------- //

class ComponentParticleSystem : public Component {
public:
  ComponentParticleSystem();

	void init(int max_particles);

	// Setters for basic aspects

	// It will use default internal mesh and material for now (quad and Billboard shader)

protected:
  ~ComponentParticleSystem();


	void emit();
	void update(float time);
	void sort();


	std::vector<Particle*> particles_;
  int max_particles_;
	int alive_particles_;
	bool burst_;


};

// ------------------------------------------------------------------------- //

#endif // __COMPONENT_PARTICLE_SYSTEM_H__
