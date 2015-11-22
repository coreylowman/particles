#ifndef SHADERS_H
#define SHADERS_H

#include <GL/glew.h>
#include <GL/glfw3.h>

typedef struct Shader {
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint program;

	GLint projection_matrix_location;
	GLint view_matrix_location;
} Shader;

int init_shaders(Shader *shader, 
    const char *vertex_shader_loc,
    const char *fragment_shader_loc);

#endif
