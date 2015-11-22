#ifndef MAT4_H
#define MAT4_H

#include <GL/glfw3.h>

void mat4_ident(GLfloat *mat);

void mat4_mul(GLfloat *src1, GLfloat *src2);
void mat4_trans(GLfloat *mat, float x, float y, float z);

void mat4_rot_x(GLfloat *mat, float angle);
void mat4_rot_y(GLfloat *mat, float angle);
void mat4_rot_z(GLfloat *mat, float angle);

void mat4_persp(GLfloat *mat, float fov, float aspect, float zNear, float zFar);

void mat4_lookat(GLfloat *mat,GLfloat *eye, GLfloat *center, GLfloat *up);

int mat4_inverse(GLfloat *out_mat, GLfloat *mat);

void mat4_mul_vec4(GLfloat *out_vec, GLfloat *mat, GLfloat *vec);

void cross(GLfloat *out, GLfloat *vec1, GLfloat *vec2);
GLfloat dot(GLfloat *vec1, GLfloat *vec2);
void normalize(GLfloat *arr);
void normalize_d(GLdouble *arr);

#endif
