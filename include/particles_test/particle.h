/*
 *	Author: Diego Ochando Torres
 *  Date: 21/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include <glm/glm.hpp>

class Particle {
public: 
  Particle();
  ~Particle();

  glm::vec3 position_;
  glm::vec3 velocity_;
  float life_time_;
  float distance_;
  bool alive_;

};

#endif // __PARTICLE_H__

