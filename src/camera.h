#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glew.h>
#include <GL/glfw3.h>

typedef struct Camera {
    GLfloat projection_matrix[16];
    GLfloat view_matrix[16];

    //view parameters
    GLfloat location[3];
    GLfloat look_at[3];
    GLfloat up[3];

    //perspective projection parameters
    GLfloat fov;
    GLfloat aspect_ratio;
    GLfloat zNear, zFar;
} Camera;


void camera_init(Camera *camera);

void get_forwards_dir(Camera camera, GLfloat *out_dir);
void get_side_dir(Camera camera, GLfloat *out_dir);

void move_forwards(Camera *camera, double dt, float direction);
void strafe(Camera *camera, double dt, float direction);
void rotate_view(Camera *camera, double side_amt, double up_amt);
void move_vertically(Camera *camera, double dt, float direction);

void update_view_matrix(Camera *camera);
void update_projection_matrix(Camera *camera);

#endif
