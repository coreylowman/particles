#ifndef TICL_PARTICLE_SYSTEM_H
#define TICL_PARTICLE_SYSTEM_H

typedef struct ParticleSystem {
	float *positions;
	float *velocities;
	float *accelerations;
	float *cubes;
	float *colors;
	size_t num_cubes;
	size_t num_particles;
	size_t num_triangles;
	size_t num_floats;
	float attractor[3];
	float friction_coeff;

	float attractor_size, particle_size;	
} ParticleSystem;

ParticleSystem *particle_system_new(int max_particles);
void particle_system_init(ParticleSystem *sys);
void particle_system_free(void *data);

void particle_system_update(ParticleSystem *sys, double dt);

#endif
