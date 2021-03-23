/*
 *	Author: Diego Ochando Torres
 *  Date: 31/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

 // ------------------------------------------------------------------------- //

#include "components/component_particle_system.h"
#include "../src/engine_internal/internal_app_data.h"

 // ------------------------------------------------------------------------- //

ComponentParticleSystem::ComponentParticleSystem() : Component(Component::kComponentKind_ParticleSystem) {

	particles_ = std::vector<Particle*>();
	initial_velocity_ = glm::vec3(0.0f, 0.0f, 1.0f);
	max_particles_ = 0;
	alive_particles_ = 0;
	max_life_time_ = 8.0f;
	emission_rate_ = 0.2f;
	burst_ = false;

	mesh_buffer_id_ = 0;
	material_parent_id_ = 2;
	texture_id_ = -1;

}

// ------------------------------------------------------------------------- //

ComponentParticleSystem::~ComponentParticleSystem() {

	for (int i = 0; i < max_particles_; ++i) {
		delete particles_[i];
	}
	particles_.clear();

}

// ------------------------------------------------------------------------- //

void ComponentParticleSystem::init(int max_particles, float emission_rate, float max_lifetime, bool burst){

	max_particles_ = max_particles;
	emission_rate_ = emission_rate;
	max_life_time_ = max_lifetime;
	burst_ = burst;

	alive_particles_ = 0;

	particles_ = std::vector<Particle*>(max_particles_);
	for (int i = 0; i < max_particles_; ++i) {
		particles_[i] = new Particle();
	}

}

// ------------------------------------------------------------------------- //

void ComponentParticleSystem::emit(){

	static float time = 0.0f;
	time -= 0.01666f;
	int initial_alive_particles = alive_particles_;

	if (!burst_) {
		if (time > 0.0f) {
			return;
		}
		else {
			time = emission_rate_;
		}
	}

	// Activate particle on the first one dead
	for (auto particle : particles_) {
		if (!particle->alive_) {
			particle->alive_ = true;
			particle->position_ = glm::vec3(0.0f, 0.0f, 0.0f);
			float rz = randFloat(-0.1f, 0.1f);
			float rx = randFloat(-0.1f, 0.1f);
			particle->velocity_ = glm::vec3(rz, rx, 0.1f);
			alive_particles_++;
			if(!burst_) break;
		}
	}
	
}

// ------------------------------------------------------------------------- //

void ComponentParticleSystem::update(float time){

	for (auto particle : particles_) {
		if (particle->alive_) {
			// If particle has exceeded the max lifetime it marks it as dead
			if (particle->life_time_ > max_life_time_) {
				particle->alive_ = false;
				particle->life_time_ = 0.0f;
				particle->position_ = glm::vec3(0.0f, 0.0f, -10000.0f);
				--alive_particles_;
				continue;
			}

			//Update position and velocity
			particle->life_time_ += time;
			particle->position_ += particle->velocity_ * time;
		}
	}

}

// ------------------------------------------------------------------------- //

void ComponentParticleSystem::sort(){



}

// ------------------------------------------------------------------------- //

void ComponentParticleSystem::loadTexture(const char* texture_path){

	auto app_data = ParticleEditor::instance().app_data_;

	// Add texture to load it later when empty
	auto it = app_data->loaded_textures_.cbegin();
	if (it == app_data->loaded_textures_.cend()) {
		// Not any object insert it
		app_data->loaded_textures_.insert(
			std::pair<int, const char*>(0, texture_path));
		// return new id
		texture_id_ = 0;
	}

	// Check if texture was previously loaded to not load it again
	it = app_data->loaded_textures_.cbegin();
	while (it != app_data->loaded_textures_.cend()) {
		if (!strcmp(texture_path, it->second)) {
			printf("\nTexture has been loaded earlier. Assigning correspondent id.");
			// Return previous assigned id
			texture_id_ = it->first;
		}
		++it;
	}

	// Save it if its not added yet
	app_data->loaded_textures_.insert(
		std::pair<int, const char*>(app_data->loaded_textures_.size(),
			texture_path));

	// return new id
	texture_id_ = app_data->loaded_textures_.size() - 1;

}

std::vector<Particle*>& ComponentParticleSystem::getAliveParticles(){

	std::vector<Particle*> alive_particles = std::vector<Particle*>(0);

	for (auto particle : particles_) {
		if (particle->alive_) alive_particles.push_back(particle);
	}

	return alive_particles;

}

// ------------------------------------------------------------------------- //

std::vector<Particle*>& ComponentParticleSystem::getAllParticles(){

	return particles_;

}

// ------------------------------------------------------------------------- //

