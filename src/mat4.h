#ifndef MAT4_H
#define MAT4_H

void mat4_ident(float *mat);

void mat4_persp(float *mat, float fov, float aspect, float zNear, float zFar);

void mat4_lookat(float *mat,float *eye, float *center, float *up);

int mat4_inverse(float *out_mat, float *mat);

void mat4_mul_vec4(float *out_vec, float *mat, float *vec);

void cross(float *out, float *vec1, float *vec2);

float dot(float *vec1, float *vec2);

void normalize(float *arr);

#endif
