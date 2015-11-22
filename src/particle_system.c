#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "particle_system.h"
#include "mat4.h"

//returns a float between -1 and 1
static float next_random(){
    int r = rand();
    float q = ((float)r)/RAND_MAX; //between 0 and 1
    return 2.0 * q - 1.0; //between -1 and 1
}

static const float cube_offset_data[] = {
    -1.0f,  -1.0f,  -1.0f,
    -1.0f,  -1.0f,   1.0f,
    -1.0f,  1.0f,    1.0f,
    1.0f,   1.0f,   -1.0f,
    -1.0f,  -1.0f,  -1.0f,
    -1.0f,  1.0f,   -1.0f,
    1.0f,   -1.0f,   1.0f,
    -1.0f,  -1.0f,  -1.0f,
    1.0f,   -1.0f,  -1.0f,
    1.0f,   1.0f,   -1.0f,
    1.0f,   -1.0f,  -1.0f,
    -1.0f,  -1.0f,  -1.0f,
    -1.0f,  -1.0f,  -1.0f,
    -1.0f,  1.0f,    1.0f,
    -1.0f,  1.0f,   -1.0f,
    1.0f,   -1.0f,   1.0f,
    -1.0f,  -1.0f,   1.0f,
    -1.0f,  -1.0f,  -1.0f,
    -1.0f,  1.0f,    1.0f,
    -1.0f,  -1.0f,   1.0f,
    1.0f,   -1.0f,   1.0f,
    1.0f,   1.0f,    1.0f,
    1.0f,   -1.0f,  -1.0f,
    1.0f,   1.0f,   -1.0f,
    1.0f,   -1.0f,  -1.0f,
    1.0f,   1.0f,    1.0f,
    1.0f,   -1.0f,   1.0f,
    1.0f,   1.0f,    1.0f,
    1.0f,   1.0f,   -1.0f,
    -1.0f,  1.0f,   -1.0f,
    1.0f,   1.0f,    1.0f,
    -1.0f,  1.0f,   -1.0f,
    -1.0f,  1.0f,    1.0f,
    1.0f,   1.0f,    1.0f,
    -1.0f,  1.0f,    1.0f,
    1.0f,   -1.0f,   1.0f
 };

static void load_cubes(const float *pos, float *cubes, float offset){
	float x = pos[0]; float y = pos[1]; float z = pos[2];
	int tri, point, j,k,l;
    for(tri = 0; tri < 12; tri++){
        for(point = 0; point < 3;point++){
            j = tri * 9 + point * 3 + 0;
            k = tri * 9 + point * 3 + 1;
            l = tri * 9 + point * 3 + 2;
            cubes[j] = x + offset * cube_offset_data[j];
            cubes[k] = y + offset * cube_offset_data[k];
            cubes[l] = z + offset * cube_offset_data[l];
        }
    }
}

static void load_colors(float *colors){
	int tri, point, j, k, l;
	float r = next_random() * 0.5 + 0.5;
	float g = next_random() * 0.5 + 0.5;
	float b = next_random() * 0.5 + 0.5;
	for (tri = 0; tri < 12; tri++) {
		for (point = 0; point < 3; point++) {
			j = tri * 9 + point * 3 + 0;
			k = tri * 9 + point * 3 + 1;
			l = tri * 9 + point * 3 + 2;
			colors[j] = r;
			colors[k] = g;
			colors[l] = b;
		}
	}
}

ParticleSystem *particle_system_new(int max_particles){    
    ParticleSystem *sys = malloc(sizeof(*sys));

    sys->positions = malloc(3 * sizeof(float) * max_particles);
    sys->velocities = malloc(3 * sizeof(float) * max_particles);
    sys->accelerations = malloc(3 * sizeof(float) * max_particles);
	sys->cubes = malloc((max_particles + 1) * 6 * 2 * 3 * 3 * sizeof(float));
	sys->colors = malloc((max_particles + 1) * 6 * 2 * 3 * 3 * sizeof(float));

    sys->num_particles = max_particles;

    return sys;
}

void particle_system_init(ParticleSystem *sys){
    sys->num_cubes = sys->num_particles + 1;
    sys->num_triangles = 6 * 2 * 3 * sys->num_cubes;
    sys->num_floats = 6 * 2 * 3 * 3 * sys->num_cubes;

    sys->attractor_size = 0.03;
    sys->particle_size = 0.003;
    
    sys->attractor[0] = 0;
    sys->attractor[1] = 0;
    sys->attractor[2] = 0;

    int i;
    load_cubes(sys->attractor, sys->cubes, sys->attractor_size);
    for (i = 0; i < 3 * 3 * 12; i++) {
        sys->colors[i] = 1.0; //want the cube to be all white
    }

    int j,k,l;
    for(i = 0;i < sys->num_particles;i++){
        j = i * 3 + 0;
        k = i * 3 + 1;
        l = i * 3 + 2;
        sys->positions[j] = next_random();
        sys->positions[k] = next_random();
        sys->positions[l] = next_random();

        load_cubes(sys->positions + j, sys->cubes + (i+1) * 3 * 36, sys->particle_size);
        load_colors(sys->colors + (i+1) * 3 * 36);

        sys->velocities[j] = next_random();
        sys->velocities[k] = next_random();
        sys->velocities[l] = next_random();
        
        sys->accelerations[j] = 0.0;
        sys->accelerations[k] = 0.0;
        sys->accelerations[l] = 0.0;
    }

    sys->friction_coeff = 0.25;
}

void particle_system_free(void *data){
    if(data == NULL) return;
    ParticleSystem *sys = data;
    free(sys->positions);
    free(sys->velocities);
    free(sys->accelerations);
	free(sys->colors);
    free(sys);
}

void particle_system_update(ParticleSystem *sys, double dt){
    int i,j,k,l;
	
	//update attractor cube with the attractor position
	load_cubes(sys->attractor, sys->cubes, sys->attractor_size);

	float attra_dist, attra_dir[3];

    for(i = 0;i < sys->num_particles;i++){
        j = 3 * i + 0;
        k = 3 * i + 1;
		l = 3 * i + 2;

		attra_dir[0] = (sys->attractor[0] - sys->positions[j]);
		attra_dir[1] = (sys->attractor[1] - sys->positions[k]);
		attra_dir[2] = (sys->attractor[2] - sys->positions[l]);		
		attra_dist = attra_dir[0] * attra_dir[0] + attra_dir[1] * attra_dir[1] + attra_dir[2] * attra_dir[2];
		attra_dist = 1 / (attra_dist * attra_dist);
		normalize(attra_dir);

		sys->accelerations[j] += attra_dir[0] * attra_dist;
		sys->accelerations[k] += attra_dir[1] * attra_dist;
		sys->accelerations[l] += attra_dir[2] * attra_dist;
		normalize(sys->accelerations + j);
	
        sys->velocities[j] -= sys->velocities[j] * sys->friction_coeff * dt;
		sys->velocities[k] -= sys->velocities[k] * sys->friction_coeff * dt;
		sys->velocities[l] -= sys->velocities[l] * sys->friction_coeff * dt;

        sys->velocities[j] += dt * sys->accelerations[j];
        sys->velocities[k] += dt * sys->accelerations[k];
		sys->velocities[l] += dt * sys->accelerations[l];

        sys->positions[j] += dt * sys->velocities[j];
        sys->positions[k] += dt * sys->velocities[k];
		sys->positions[l] += dt * sys->velocities[l];

		load_cubes(sys->positions + j, sys->cubes + (i+1) * 3 * 36, sys->particle_size);
    }
}
