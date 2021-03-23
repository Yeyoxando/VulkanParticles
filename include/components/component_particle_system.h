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
#include <glm/glm.hpp>

// ------------------------------------------------------------------------- //

struct Particle {

	glm::vec3 position_;
	glm::vec3 velocity_;
	glm::vec4 color_;
	float life_time_;
	float distance_;
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

class ComponentParticleSystem : public Component {
public:
  ComponentParticleSystem();

	void init(int max_particles, bool burst = false);

	void loadTexture(const char* texture_path);

	int getMaxParticles() { return max_particles_; }
	int getTextureID() { return texture_id_; }

	std::vector<Particle*>& getAliveParticles();
	std::vector<Particle*>& getAllParticles();

protected:
  ~ComponentParticleSystem();


	void emit();
	void update(float time);
	void sort();


	std::vector<Particle*> particles_;
	glm::vec3 initial_velocity_;
	int alive_particles_;
	int max_particles_;
	float max_life_time_;
	bool burst_;

	int mesh_buffer_id_;
	int material_parent_id_;
	int texture_id_;



	friend class Scene;

};

// ------------------------------------------------------------------------- //

#endif // __COMPONENT_PARTICLE_SYSTEM_H__
