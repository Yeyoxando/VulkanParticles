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
	initial_color_ = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	initial_velocity_ = glm::vec3(0.0f, 0.0f, 0.1f);
	min_velocity_ = glm::vec3(-0.1f, -0.1f, 0.1f);
	max_velocity_ = glm::vec3(0.1f, 0.1f, 0.12f);
	max_particles_ = 0;
	alive_particles_ = 0;
	max_life_time_ = 5.0f;
	emission_rate_ = 0.2f;
	burst_ = false;
	constant_velocity_ = false;

	mesh_buffer_id_ = 0;
	material_parent_id_ = 2;
	texture_id_ = -1;

	last_time_ = 0.0f;

	lerp_color_ = false;
	lerp_alpha_ = false;
	final_color_ = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	lerp_speed_ = false;
	final_speed_ = glm::vec3(0.0f, 0.0f, 0.1f);

}

// ------------------------------------------------------------------------- //

ComponentParticleSystem::~ComponentParticleSystem() {

	for (int i = 0; i < max_particles_; ++i) {
		delete particles_[i];
	}
	particles_.clear();

}

// ------------------------------------------------------------------------- //

void ComponentParticleSystem::init(int max_particles) {
	
	max_particles_ = max_particles;
	emission_rate_ = 0.2f;
	max_life_time_ = 5.0f;
	burst_ = false;

	alive_particles_ = 0;

	particles_ = std::vector<Particle*>(max_particles_);
	for (int i = 0; i < max_particles_; ++i) {
		particles_[i] = new Particle();
	}

}

// ------------------------------------------------------------------------- //

void ComponentParticleSystem::emit(double deltatime) {

	if (alive_particles_ == max_particles_) return;

	last_time_ -= deltatime;
	int initial_alive_particles = alive_particles_;

	if (!burst_ && last_time_ > 0.0f) return;

	// Activate particle on the first one dead
	for (auto particle : particles_) {
		if (!particle->alive_) {
			particle->alive_ = true;
			particle->position_ = glm::vec3(0.0f, 0.0f, 0.0f);
			if (!constant_velocity_) {
				float rz = randFloat(min_velocity_.x, max_velocity_.x);
				float rx = randFloat(min_velocity_.y, max_velocity_.y);
				float ry = randFloat(min_velocity_.z, max_velocity_.z);
				particle->velocity_ = glm::vec3(rz, rx, ry);
			}
			else {
				particle->velocity_ = initial_velocity_;
			}
			alive_particles_++;
			last_time_ = emission_rate_;
			if (!burst_) break;
		}
	}

}

// ------------------------------------------------------------------------- //

void ComponentParticleSystem::update(double deltatime) {

	for (auto particle : particles_) {
		if (particle->alive_) {
			// If particle has exceeded the max lifetime it marks it as dead
			if (particle->life_time_ > max_life_time_ && max_life_time_ > 0.0f) {
				particle->alive_ = false;
				particle->life_time_ = 0.0f;
				particle->position_ = glm::vec3(0.0f, 0.0f, -10000.0f);
				--alive_particles_;
				continue;
			}

			float lerp_value = particle->life_time_ / max_life_time_;
			
			//Update color
			if (lerp_color_) {
				glm::vec4 color;
				color.r = glm::smoothstep(initial_color_.r, final_color_.r, lerp_value);
				color.g = glm::smoothstep(initial_color_.g, final_color_.g, lerp_value);
				color.b = glm::smoothstep(initial_color_.b, final_color_.b, lerp_value);
				color.a = glm::smoothstep(initial_color_.a, final_color_.a, lerp_value);
				particle->color_ = color;
			}
			if (lerp_alpha_) {
				float alpha = glm::smoothstep(initial_color_.a, final_color_.a, lerp_value);
				particle->color_.a = alpha;
			}

			//Update speed
			if (lerp_speed_) {
				glm::vec3 speed;
				speed.x = glm::smoothstep(initial_color_.x, final_color_.x, lerp_value);
				speed.y = glm::smoothstep(initial_color_.y, final_color_.y, lerp_value);
				speed.z = glm::smoothstep(initial_color_.z, final_color_.z, lerp_value);
				particle->velocity_ = speed;
			}

			//Update position and velocity
			particle->life_time_ += static_cast<float>(deltatime);
			particle->position_ += particle->velocity_ * static_cast<float>(deltatime);
		}
	}

}

// ------------------------------------------------------------------------- //

void ComponentParticleSystem::sort() {



}

// ------------------------------------------------------------------------- //

void ComponentParticleSystem::loadTexture(const char* texture_path) {

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

// ------------------------------------------------------------------------- //

void ComponentParticleSystem::setSpawnArea(float length, float width, float height){



}

// ------------------------------------------------------------------------- //

void ComponentParticleSystem::setEmissionRate(float emission_rate) {

	if (emission_rate <= 0.0f) return;
	
	emission_rate_ = emission_rate;

}

// ------------------------------------------------------------------------- //

void ComponentParticleSystem::setLifetime(float lifetime) {

	max_life_time_ = lifetime;

}

// ------------------------------------------------------------------------- //

void ComponentParticleSystem::setConstantVelocity(glm::vec3 constant_velocity) {

	initial_velocity_ = constant_velocity;
	constant_velocity_ = true;

}

// ------------------------------------------------------------------------- //

void ComponentParticleSystem::setInitialVelocity(glm::vec3 min_velocity, glm::vec3 max_velocity) {

	min_velocity_ = min_velocity;
	max_velocity_ = max_velocity;
	constant_velocity_ = false;

}

// ------------------------------------------------------------------------- //

void ComponentParticleSystem::setVelocityOverTime(glm::vec3 final_velocity){

	lerp_speed_ = true;
	final_speed_ = final_velocity;

}

// ------------------------------------------------------------------------- //

void ComponentParticleSystem::setBurst() {

	burst_ = true;

}

// ------------------------------------------------------------------------- //

void ComponentParticleSystem::setParticleColor(glm::vec4 color) {

	initial_color_ = color;

	for (auto particle : particles_) {
		particle->color_ = color;
	}

}

// ------------------------------------------------------------------------- //

void ComponentParticleSystem::setParticleColorOverTime(glm::vec4 final_color){

	lerp_color_ = true;
	final_color_ = final_color;

}

// ------------------------------------------------------------------------- //

void ComponentParticleSystem::setAlphaColorOverTime(float final_alpha){

	lerp_alpha_ = true;
	final_color_.a = final_alpha;

}

// ------------------------------------------------------------------------- //

std::vector<Particle*>& ComponentParticleSystem::getAliveParticles() {

	std::vector<Particle*> alive_particles = std::vector<Particle*>(0);

	for (auto particle : particles_) {
		if (particle->alive_) alive_particles.push_back(particle);
	}

	return alive_particles;

}

// ------------------------------------------------------------------------- //

std::vector<Particle*>& ComponentParticleSystem::getAllParticles() {

	return particles_;

}

// ------------------------------------------------------------------------- //

