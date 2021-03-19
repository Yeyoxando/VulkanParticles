/*
 *	Author: Diego Ochando Torres
 *  Date: 31/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

 // ------------------------------------------------------------------------- //

#include "components/component_particle_system.h"

 // ------------------------------------------------------------------------- //

ComponentParticleSystem::ComponentParticleSystem() : Component(Component::kComponentKind_ParticleSystem) {

	max_particles_ = 0;
	alive_particles_ = 0;
	burst_ = false;

}

// ------------------------------------------------------------------------- //

ComponentParticleSystem::~ComponentParticleSystem() {

}

// ------------------------------------------------------------------------- //

void ComponentParticleSystem::init(int max_particles){

	max_particles_ = max_particles;
	alive_particles_ = 0;
	burst_ = false;

	particles_ = std::vector<Particle*>(max_particles_);

}

// ------------------------------------------------------------------------- //

void ComponentParticleSystem::emit(){

	int initial_alive_particles = alive_particles_;

	while (alive_particles_ < max_particles_) {
		// Activate particle on the first one dead
		for (auto particle : particles_) {
			if (!particle->alive_) {
				particle->alive_ = true;
				particle->position_ = glm::vec3(0.0f, 0.0f, 0.0f);
				particle->velocity_ = glm::vec3(0.0f, 0.0f, 0.0f);
				alive_particles_++;
				break;
			}
		}

		// If burst is not active, spawn max 10 particles in a frame
		if (!burst_) {
			if ((alive_particles_ - initial_alive_particles) > 9) {
				break;
			}
		}
	}

}

// ------------------------------------------------------------------------- //

void ComponentParticleSystem::update(float time){

	for (auto particle : particles_) {
		if (particle->alive_) {
			// If particle has exceeded the max lifetime it marks it as dead
			if (particle->life_time_ > 5.0f) {
				particle->alive_ = false;
				particle->life_time_ = 0.0f;
				continue;
			}

			//Update position and velocity
			particle->life_time_ += time;
			particle->position_.y += 0.001f;
		}
	}

}

// ------------------------------------------------------------------------- //

void ComponentParticleSystem::sort(){



}

// ------------------------------------------------------------------------- //

