#include <stdio.h>
#include "shaders.h"

static void print_shader_info_log(GLuint shader) {
    int max_length = 2048;
    int actual_length = 0;
    char log[2048];
    glGetShaderInfoLog(shader, max_length, &actual_length, log);
    printf("shader info log for GL index %i:\n%s\n", shader, log);
}

static void print_program_info_log(GLuint program) {
    int max_length = 2048;
    int actual_length = 0;
    char log[2048];
    glGetProgramInfoLog(program, max_length, &actual_length, log);
    printf("program info log for GL index %i:\n%s", program, log);
}

static int parse_file_into_str (const char* file_name, char* shader_str, int max_len){
    FILE* file = fopen (file_name , "r");
    if (!file) {
        fprintf(stderr,"ERROR: opening file for reading: %s\n", file_name);
        return 0;
    }
    size_t cnt = fread (shader_str, 1, max_len - 1, file);
    if (cnt >= max_len - 1) {
        fprintf(stderr, "WARNING: file %s too big - truncated.\n", file_name);
        return 0;
    }
    if (ferror (file)) {
        fprintf(stderr, "ERROR: reading shader file %s\n", file_name);
        fclose (file);
        return 0;
    }
    // append \0 to end of file string
    shader_str[cnt] = 0;
    fclose (file);
    return 1;
}

int init_shaders(Shader *shader,
	const char *vertex_shader_loc,
	const char *fragment_shader_loc)
{
	int params = -1;
	int length;
	char vertex_shader_source[1024 * 256];
	char fragment_shader_source[1024 * 256];
	const GLchar *p;

	parse_file_into_str(vertex_shader_loc, vertex_shader_source, 1024 * 256);
    shader->vertexShader = glCreateShader(GL_VERTEX_SHADER);
    p = (const GLchar *)vertex_shader_source;
    glShaderSource(shader->vertexShader, 1, &p, NULL);
    glCompileShader(shader->vertexShader);
    glGetShaderiv(shader->vertexShader, GL_COMPILE_STATUS, &params);
    if (GL_TRUE != params) {
        fprintf(stderr, "ERROR: GL shader index %i did not compile\n", shader->vertexShader);
        print_shader_info_log(shader->vertexShader);
        return 1;
    }

    parse_file_into_str(fragment_shader_loc, fragment_shader_source, 1024 * 256);
    shader->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    p = (const GLchar *)fragment_shader_source;
    glShaderSource(shader->fragmentShader, 1, &p, NULL);
    glCompileShader(shader->fragmentShader);
    glGetShaderiv(shader->fragmentShader, GL_COMPILE_STATUS, &params);
    if (GL_TRUE != params) {
        fprintf(stderr, "ERROR: GL shader index %i did not compile\n", shader->fragmentShader);
        print_shader_info_log(shader->fragmentShader);
        return 1;
    }

    shader->program = glCreateProgram();
    glAttachShader(shader->program, shader->fragmentShader);
    glAttachShader(shader->program, shader->vertexShader);    
    glLinkProgram(shader->program);
    glGetProgramiv(shader->program, GL_LINK_STATUS, &params);
    if (GL_TRUE != params) {
        fprintf(stderr, "ERROR: could not link shader programme GL index %i\n",shader->program);
        print_program_info_log(shader->program);
        return 1;
    }

	shader->projection_matrix_location = glGetUniformLocation(shader->program, "projection");
	shader->view_matrix_location = glGetUniformLocation(shader->program, "view");

    return 0;
}
