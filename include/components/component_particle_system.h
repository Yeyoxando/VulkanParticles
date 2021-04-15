/*
* Author: Diego Ochando Torres
* Date: 31/12/2020
* e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
*/

#ifndef __COMPONENT_PARTICLE_SYSTEM_H__
#define __COMPONENT_PARTICLE_SYSTEM_H__

// ------------------------------------------------------------------------- // 

#include "component.h"
#include "particle_editor.h"

#include <vector>
#include <glm.hpp>

// ------------------------------------------------------------------------- //

// This should be divided into an Structure of Arrays when DOD will be implemented.
/// @brief Structure containing the necessary data to simulate the behaviour of a particle.
struct Particle {

	glm::vec3 position_;
	glm::vec3 velocity_;
	glm::vec4 color_;
	float life_time_;
	float distance_; // From particle to camera position.
	bool alive_;

	Particle() {
		position_ = glm::vec3(0.0f, 0.0f, -10000.0f);
		velocity_ = glm::vec3();
		color_ = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		life_time_ = 0.0f;
		distance_ = 0.0f;
		alive_ = false;
	}

};

// ------------------------------------------------------------------------- //

/**
* @brief Particle system component for an entity.
*				 Its material and mesh are set by default, likewise that with a 3D object.
*/
class ComponentParticleSystem : public Component {
public:
	ComponentParticleSystem();

	/// @brief Initializes the particle system and its max particles.
	void init(int max_particles);
	/// @brief Add a texture to load it internally if it is not previously added to load and store its id.
	void loadTexture(const char* texture_path);


	// -- These setters will be part of the modules, for now they are hard-coded --
	/// @brief Spawn particles in a box area. If not called particles will spawn in the same position.
	void setSpawnArea(float length, float width, float height);
	/// @brief Time between two particles spawn.
	void setEmissionRate(float emission_rate);
	/// @brief Time that a particle will live. If set to 0 it won't die.
	void setLifetime(float lifetime);
	/// @brief Set a constant velocity for all the particles.
	void setConstantVelocity(glm::vec3 constant_velocity);
	/// @brief Set a random initial velocity between the two given vectors.
	void setInitialVelocity(glm::vec3 min_velocity, glm::vec3 max_velocity);
	/// @brief Interpolates initial velocity with final velocity over lifetime.
	void setVelocityOverTime(glm::vec3 final_velocity);
	/// @brief If called it will spawn all the particles in the same frame.
	void setBurst();
	/// @brief Set a constant color for all the particles.
	void setParticleColor(glm::vec4 color);
	/// @brief Set a color to change the particle over their life time.
	void setParticleColorOverTime(glm::vec4 final_color);
	/// @brief Set an alpha value to change the particles over their life time.
	void setAlphaColorOverTime(float final_alpha);


	int getMaxParticles() { return max_particles_; }
	/// @return Current used texture id.
	int getTextureID() { return texture_id_; }

	std::vector<Particle*>& getAliveParticles();
	std::vector<Particle*>& getAllParticles();

protected:
	~ComponentParticleSystem();

	/// @brief Emit stage where particles get spawned and initialized.
	void emit(double deltatime);
	/// @brief Particles update and die if they surpass their lifetime.
	void update(double deltatime);
	/// @brief Particles get sorted by their distance to the camera if the blending mode requires it.
	void sort();


	std::vector<Particle*> particles_;
	int alive_particles_;
	int max_particles_;
	float max_life_time_;


	glm::vec4 initial_color_;

	glm::vec3 initial_velocity_;
	/// @brief Min velocity for the random distribution.
	glm::vec3 min_velocity_;
	/// @brief Max velocity for the random distribution.
	glm::vec3 max_velocity_;

	/// @brief Indicates if the color should interpolate over time.
	bool lerp_color_;
	/// @brief Indicates if the alpha value should interpolate over time.
	bool lerp_alpha_;
	/// @brief Color to lerp.
	glm::vec4 final_color_;
	/// @brief Indicates if the speed should interpolate over time.
	bool lerp_speed_;
	/// @brief Speed to lerp.
	glm::vec3 final_speed_;

	/// @brief Time that passes between two particles spawning.
	float emission_rate_;
	/// @brief If true all particles will spawn at the same time ignoring the emission rate.
	bool burst_;
	/// @brief If set, all the particles will share the same velocity.
	bool constant_velocity_;

	/// @brief Internal ID of the mesh used in the particle system. For now is always a quad.
	int mesh_buffer_id_;
	/// @brief Internal ID of the material parent. For now is fixed to a particle specific one.
	int material_parent_id_;
	/// @brief Internal ID of the texture used in the particle system.
	int texture_id_;

	/// @brief Time passed since last spawned particle
	float last_time_;

	friend class Scene;

};

// ------------------------------------------------------------------------- //

#endif // __COMPONENT_PARTICLE_SYSTEM_H__
