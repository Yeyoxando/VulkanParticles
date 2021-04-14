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

	/// @brief 
	void setEmissionRate(float emission_rate);
	/// @brief 
	void setLifetime(float lifetime);
	/// @brief 
	void setInitialVelocity(glm::vec3 constant_velocity);
	/// @brief 
	void setInitialVelocity(glm::vec3 min_velocity, glm::vec3 max_velocity);
	/// @brief 
	void setBurst();
	/// @brief 
	void setParticleColor(glm::vec4 color);
	/// @brief 


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
	glm::vec3 initial_velocity_;
	int alive_particles_;
	int max_particles_;
	float max_life_time_;
	/// @brief Time that passes between two particles spawning.
	float emission_rate_;
	/// @brief If true all particles will spawn at the same time ignoring the emission rate.
	bool burst_;

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
