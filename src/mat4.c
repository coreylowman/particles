#include <math.h>
#include "mat4.h"

//https://en.wikipedia.org/wiki/Fast_inverse_square_root
static float q_invsqrt(float n){
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = n * 0.5F;
	y = n;
	i = *(long *)&y;                       // evil floating point bit level hacking
	i = 0x5f3759df - (i >> 1);               // what the fuck? 
	y = *(float *)&i;
	y = y * (threehalfs - (x2 * y * y));   // 1st iteration
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

	return y;
}

void normalize(GLfloat *arr){
	GLfloat x = arr[0];
	GLfloat y = arr[1];
	GLfloat z = arr[2];
	GLfloat length_sqrd = x * x + y * y + z * z;
	GLfloat inv_sqrt_length = q_invsqrt(length_sqrd);
	arr[0] = x * inv_sqrt_length;
	arr[1] = y * inv_sqrt_length;
	arr[2] = z * inv_sqrt_length;
}

void normalize_d(GLdouble *arr){
	GLdouble x = arr[0];
	GLdouble y = arr[1];
	GLdouble z = arr[2];
	GLdouble length = sqrt(x * x + y * y + z * z);
	arr[0] = x / length;
	arr[1] = y / length;
	arr[2] = z / length;
}

void cross(GLfloat *out, GLfloat *vec1, GLfloat *vec2){
    out[0] = vec1[1] * vec2[2] - vec1[2] * vec2[1];
    out[1] = vec1[2] * vec2[0] - vec1[0] * vec2[2];
    out[2] = vec1[0] * vec2[1] - vec1[1] * vec2[0];
}

GLfloat dot(GLfloat *vec1, GLfloat *vec2){
    GLfloat sum = 0;
    int i;
    for(i = 0;i < 3;i++){
        sum += vec1[i] * vec2[i];
    }
    return sum;
}

void mat4_ident(GLfloat *mat){
    mat[0] = 1;     mat[1] = 0;     mat[2] = 0;     mat[3] = 0;
    mat[4] = 0;     mat[5] = 1;     mat[6] = 0;     mat[7] = 0;
    mat[8] = 0;     mat[9] = 0;     mat[10] = 1;    mat[11] = 0;
    mat[12] = 0;    mat[13] = 0;    mat[14] = 0;    mat[15] = 1;
}

void mat4_mul(GLfloat *src1, GLfloat *src2){
    int i,j,k;
    GLfloat dest[16];
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            dest[i * 4 + j] = 0;
            for (k = 0; k < 4; k++) {
                dest[i * 4 + j] += src1[i * 4 + k] * src2[k * 4 + j];
            }
        }
    }
    for (i = 0; i < 16; i++) {
        src1[i] = dest[i];
    }
}

void mat4_trans(GLfloat *mat, float x, float y, float z){
    GLfloat trans_mat[16];
    mat4_ident(trans_mat);
    trans_mat[3] += x;
    trans_mat[7] += y;
    trans_mat[11] += z;
    mat4_mul(mat, trans_mat);
}

void mat4_rot_x(GLfloat *mat, float angle){
    float s = sin(angle); float c = cos(angle);
    GLfloat rot_mat[16];
    mat4_ident(rot_mat);
    rot_mat[0] = 1;
                    rot_mat[5] = c;     rot_mat[6] = -s;
                    rot_mat[9] = s;     rot_mat[10] = c;
    mat4_mul(mat, rot_mat);
}

void mat4_rot_y(GLfloat *mat, float angle){
    float s = sin(angle); float c = cos(angle);
    GLfloat rot_mat[16];
    mat4_ident(rot_mat);
    rot_mat[0] = c;                     rot_mat[2] = s;
                    rot_mat[5] = 1;
    rot_mat[8] = -s;                    rot_mat[10] = c;
    mat4_mul(mat, rot_mat);
}

void mat4_rot_z(GLfloat *mat, float angle){
    float s = sin(angle); float c = cos(angle);
    GLfloat rot_mat[16];
    mat4_ident(rot_mat);
    rot_mat[0] = c; rot_mat[1] = -s;
    rot_mat[4] = s; rot_mat[5] = c;
                                    rot_mat[10] = 1;
    mat4_mul(mat, rot_mat);
}

static float radians(float degrees){
	return degrees * 3.14159265 / 180.0;
}

void mat4_persp(GLfloat *mat, float fov, float aspect, float zNear, float zFar){
	float tanHalf = tan(radians(fov) / 2.0);	

    mat[0] = 1 / (aspect * tanHalf); mat[1] = 0; mat[2] = 0; mat[3] = 0;
    mat[4] = 0; mat[5] = 1 / tanHalf; mat[6] = 0; mat[7] = 0;
	mat[8] = 0; mat[9] = 0; mat[10] = -(zFar + zNear) / (zFar - zNear); mat[11] = -(2 * zFar * zNear) / (zFar - zNear);
	mat[12] = 0; mat[13] = 0; mat[14] = -1; mat[15] = 0;
}

void mat4_lookat(GLfloat *mat,GLfloat *eye, GLfloat *center, GLfloat *up){
    int i;
    GLfloat f[3], s[3], u[3];

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

int mat4_inverse(GLfloat *out_mat, GLfloat *m){
	GLfloat inv[16], det;
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

void mat4_mul_vec4(GLfloat *out_vec, GLfloat *mat, GLfloat *vec){
    int i;
    for(i = 0;i < 4;i++){
        out_vec[i] = 0;
        out_vec[i] += mat[i * 4 + 0] * vec[0];
        out_vec[i] += mat[i * 4 + 1] * vec[1];
        out_vec[i] += mat[i * 4 + 2] * vec[2];
        out_vec[i] += mat[i * 4 + 3] * vec[3];
    }
}
