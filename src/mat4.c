#include <math.h>
#include "mat4.h"

void mat4_ident(float *mat){
    mat[0] = 1;     mat[1] = 0;     mat[2] = 0;     mat[3] = 0;
    mat[4] = 0;     mat[5] = 1;     mat[6] = 0;     mat[7] = 0;
    mat[8] = 0;     mat[9] = 0;     mat[10] = 1;    mat[11] = 0;
    mat[12] = 0;    mat[13] = 0;    mat[14] = 0;    mat[15] = 1;
}

static float radians(float degrees){
	return degrees * 3.14159265 / 180.0;
}

void mat4_persp(float *mat, float fov, float aspect, float zNear, float zFar){
    float tanHalf = tan(radians(fov) / 2.0);    

    mat[0] = 1 / (aspect * tanHalf); mat[1] = 0; mat[2] = 0; mat[3] = 0;
    mat[4] = 0; mat[5] = 1 / tanHalf; mat[6] = 0; mat[7] = 0;
    mat[8] = 0; mat[9] = 0; mat[10] = -(zFar + zNear) / (zFar - zNear); mat[11] = -(2 * zFar * zNear) / (zFar - zNear);
    mat[12] = 0; mat[13] = 0; mat[14] = -1; mat[15] = 0;
}

void mat4_lookat(float *mat,float *eye, float *center, float *up){
    int i;
    float f[3], s[3], u[3];

    for(i = 0;i < 3;i++){
        f[i] = center[i] - eye[i];
    }
    normalize(f);

    cross(s, f, up);
    normalize(s);

    cross(u, s, f);

	mat[0] = s[0];          mat[1] = s[1];          mat[2] = s[2];		mat[3] = -dot(s, eye);
	mat[4] = u[0];          mat[5] = u[1];          mat[6] = u[2];		mat[7] = -dot(u, eye);
	mat[8] = -f[0];         mat[9] = -f[1];        mat[10] = -f[2];		mat[11] = dot(f, eye);
}

int mat4_inverse(float *out_mat, float *m){
	float inv[16], det;
	int i;

	inv[0] = m[5] * m[10] * m[15] -
		m[5] * m[11] * m[14] -
		m[9] * m[6] * m[15] +
		m[9] * m[7] * m[14] +
		m[13] * m[6] * m[11] -
		m[13] * m[7] * m[10];

	inv[4] = -m[4] * m[10] * m[15] +
		m[4] * m[11] * m[14] +
		m[8] * m[6] * m[15] -
		m[8] * m[7] * m[14] -
		m[12] * m[6] * m[11] +
		m[12] * m[7] * m[10];

	inv[8] = m[4] * m[9] * m[15] -
		m[4] * m[11] * m[13] -
		m[8] * m[5] * m[15] +
		m[8] * m[7] * m[13] +
		m[12] * m[5] * m[11] -
		m[12] * m[7] * m[9];

	inv[12] = -m[4] * m[9] * m[14] +
		m[4] * m[10] * m[13] +
		m[8] * m[5] * m[14] -
		m[8] * m[6] * m[13] -
		m[12] * m[5] * m[10] +
		m[12] * m[6] * m[9];

	inv[1] = -m[1] * m[10] * m[15] +
		m[1] * m[11] * m[14] +
		m[9] * m[2] * m[15] -
		m[9] * m[3] * m[14] -
		m[13] * m[2] * m[11] +
		m[13] * m[3] * m[10];

	inv[5] = m[0] * m[10] * m[15] -
		m[0] * m[11] * m[14] -
		m[8] * m[2] * m[15] +
		m[8] * m[3] * m[14] +
		m[12] * m[2] * m[11] -
		m[12] * m[3] * m[10];

	inv[9] = -m[0] * m[9] * m[15] +
		m[0] * m[11] * m[13] +
		m[8] * m[1] * m[15] -
		m[8] * m[3] * m[13] -
		m[12] * m[1] * m[11] +
		m[12] * m[3] * m[9];

	inv[13] = m[0] * m[9] * m[14] -
		m[0] * m[10] * m[13] -
		m[8] * m[1] * m[14] +
		m[8] * m[2] * m[13] +
		m[12] * m[1] * m[10] -
		m[12] * m[2] * m[9];

	inv[2] = m[1] * m[6] * m[15] -
		m[1] * m[7] * m[14] -
		m[5] * m[2] * m[15] +
		m[5] * m[3] * m[14] +
		m[13] * m[2] * m[7] -
		m[13] * m[3] * m[6];

	inv[6] = -m[0] * m[6] * m[15] +
		m[0] * m[7] * m[14] +
		m[4] * m[2] * m[15] -
		m[4] * m[3] * m[14] -
		m[12] * m[2] * m[7] +
		m[12] * m[3] * m[6];

	inv[10] = m[0] * m[5] * m[15] -
		m[0] * m[7] * m[13] -
		m[4] * m[1] * m[15] +
		m[4] * m[3] * m[13] +
		m[12] * m[1] * m[7] -
		m[12] * m[3] * m[5];

	inv[14] = -m[0] * m[5] * m[14] +
		m[0] * m[6] * m[13] +
		m[4] * m[1] * m[14] -
		m[4] * m[2] * m[13] -
		m[12] * m[1] * m[6] +
		m[12] * m[2] * m[5];

	inv[3] = -m[1] * m[6] * m[11] +
		m[1] * m[7] * m[10] +
		m[5] * m[2] * m[11] -
		m[5] * m[3] * m[10] -
		m[9] * m[2] * m[7] +
		m[9] * m[3] * m[6];

	inv[7] = m[0] * m[6] * m[11] -
		m[0] * m[7] * m[10] -
		m[4] * m[2] * m[11] +
		m[4] * m[3] * m[10] +
		m[8] * m[2] * m[7] -
		m[8] * m[3] * m[6];

	inv[11] = -m[0] * m[5] * m[11] +
		m[0] * m[7] * m[9] +
		m[4] * m[1] * m[11] -
		m[4] * m[3] * m[9] -
		m[8] * m[1] * m[7] +
		m[8] * m[3] * m[5];

	inv[15] = m[0] * m[5] * m[10] -
		m[0] * m[6] * m[9] -
		m[4] * m[1] * m[10] +
		m[4] * m[2] * m[9] +
		m[8] * m[1] * m[6] -
		m[8] * m[2] * m[5];

	det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

	if (det == 0)
		return 0;

	det = 1.0 / det;

	for (i = 0; i < 16; i++)
		out_mat[i] = inv[i] * det;

	return 1;
}

void mat4_mul_vec4(float *out_vec, float *mat, float *vec){
    int i;
    for(i = 0;i < 4;i++){
        out_vec[i] = 0;
        out_vec[i] += mat[i * 4 + 0] * vec[0];
        out_vec[i] += mat[i * 4 + 1] * vec[1];
        out_vec[i] += mat[i * 4 + 2] * vec[2];
        out_vec[i] += mat[i * 4 + 3] * vec[3];
    }
}

void cross(float *out, float *vec1, float *vec2){
    out[0] = vec1[1] * vec2[2] - vec1[2] * vec2[1];
    out[1] = vec1[2] * vec2[0] - vec1[0] * vec2[2];
    out[2] = vec1[0] * vec2[1] - vec1[1] * vec2[0];
}

float dot(float *vec1, float *vec2){
    float sum = 0;
    int i;
    for(i = 0;i < 3;i++){
        sum += vec1[i] * vec2[i];
    }
    return sum;
}

void normalize(float *arr){
    float x = arr[0];
    float y = arr[1];
    float z = arr[2];
    float length = sqrt(x * x + y * y + z * z);
    float inv_length = 1 / length;
    arr[0] = x * inv_length;
    arr[1] = y * inv_length;
    arr[2] = z * inv_length;
}
