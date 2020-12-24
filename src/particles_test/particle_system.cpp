/*
 *	Author: Diego Ochando Torres
 *  Date: 21/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

 // ------------------------------------------------------------------------- // 

#include "particles_test/particle_system.h"

 // ------------------------------------------------------------------------- // 

ParticleSystem::ParticleSystem() {

  burst_ = false;
  alive_particles_ = 0;

}

// ------------------------------------------------------------------------- // 

ParticleSystem::~ParticleSystem() {



}

// ------------------------------------------------------------------------- // 

void ParticleSystem::init(int max_particles) {

  particles_ = std::vector<Particle>(max_particles);
  alive_particles_ = 0;
  burst_ = false;

}

// ------------------------------------------------------------------------- // 

void ParticleSystem::emit() {
  int initial_alive_particles = alive_particles_;

  while (alive_particles_ < 500) {
    // Activate particle
    for (auto particle : particles_) {
      if (!particle.alive_) {
        particle.alive_ = true;
        particle.position_ = glm::vec3(0.0f, 0.0f, 0.0f);
        particle.velocity_ = glm::vec3(0.0f, 0.0f, 0.0f);
        alive_particles_++;
        break;
      }
    }

    if (!burst_) {
      if ((alive_particles_ - initial_alive_particles) > 9) {
        break;
      }
    }
  }

}

// ------------------------------------------------------------------------- // 

void ParticleSystem::update(float time) {

  for (auto particle : particles_) {
    if (particle.alive_) {
      if (particle.life_time_ > 5.0f) {
        particle.alive_ = false;
        particle.life_time_ = 0.0f;
        continue;
      }

      //Update position and velocity
      particle.life_time_ += time;
      particle.position_.g += 0.001f;
    }
  }

}

// ------------------------------------------------------------------------- // 

void ParticleSystem::sort() {



}

// ------------------------------------------------------------------------- // 
