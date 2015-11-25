#ifndef CAMERA_H
#define CAMERA_H

typedef struct Camera {
    float projection_matrix[16];
    float view_matrix[16];

    //view parameters
    float location[3];
    float look_at[3];
    float up[3];

    //perspective projection parameters
    float fov;
    float aspect_ratio;
    float zNear, zFar;
} Camera;


void camera_init(Camera *camera, int width, int height);

void get_forwards_dir(Camera camera, float *out_dir);
void get_side_dir(Camera camera, float *out_dir);

void move_forwards(Camera *camera, double dt, float direction);
void strafe(Camera *camera, double dt, float direction);
void rotate_view(Camera *camera, double side_amt, double up_amt);
void move_vertically(Camera *camera, double dt, float direction);

void update_view_matrix(Camera *camera);
void update_projection_matrix(Camera *camera);

#endif
