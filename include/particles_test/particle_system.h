/*
 *	Author: Diego Ochando Torres
 *  Date: 21/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

#ifndef __PARTICLE_SYSTEM_H__
#define __PARTICLE_SYSTEM_H__

#include "particles_test/particle.h"

#include <vector>

class ParticleSystem {
public:
  ParticleSystem();
  ~ParticleSystem();

  void init(int max_particles);

  void emit();
  void update(float time);
  void sort();

  std::vector<Particle> particles_;

private:
  int alive_particles_;
  bool burst_;

};

#endif // __PARTICLE_SYSTEM_H__

