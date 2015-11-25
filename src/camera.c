#include "camera.h"
#include "mat4.h"

void camera_init(Camera *camera, int width, int height){
    camera->location[0] = 3;
    camera->location[1] = 3;
    camera->location[2] = 3;

    camera->look_at[0] = 0;
    camera->look_at[1] = 0;
    camera->look_at[2] = 0;

    camera->up[0] = 0;
    camera->up[1] = 1;
    camera->up[2] = 0;
	
    camera->fov = 45.0;
    camera->aspect_ratio = (float) width / height;
    camera->zNear = 0.1;
    camera->zFar = 100.0;

    update_view_matrix(camera);
    update_projection_matrix(camera);
}

void get_forwards_dir(Camera camera, float *out_dir){
	int i;
	for (i = 0; i < 3; i++) {
        out_dir[i] = camera.look_at[i] - camera.location[i];
    }
    normalize(out_dir);
}

void get_side_dir(Camera camera, float *out_dir){
    int i;
    float forwards[3];

    for (i = 0; i < 3; i++) {
        forwards[i] = camera.look_at[i] - camera.location[i];
    }
    normalize(forwards);

    cross(out_dir, forwards, camera.up);
    normalize(out_dir);
}

void move_forwards(Camera *camera, double dt, float direction){
    int i;
    float forwards[3];
    for (i = 0; i < 3; i++) {
        forwards[i] = camera->look_at[i] - camera->location[i];
    }
    normalize(forwards);
    for (i = 0; i < 3; i++) {
        camera->look_at[i] += direction * dt * forwards[i];
        camera->location[i] += direction * dt * forwards[i];
    }
}

void strafe(Camera *camera, double dt, float direction){
    int i;
    float forwards[3], sideways[3];

    for (i = 0; i < 3; i++) {
        forwards[i] = camera->look_at[i] - camera->location[i];
    }
    normalize(forwards);

    cross(sideways, forwards, camera->up);
    normalize(sideways);

    for (i = 0; i < 3; i++) {
        camera->look_at[i] += direction * dt * sideways[i];
        camera->location[i] += direction * dt * sideways[i];
    }
}

void rotate_view(Camera *camera, double side_amt, double up_amt){
    int i;
    float forwards[3], sideways[3];

    for (i = 0; i < 3; i++) {
        forwards[i] = camera->look_at[i] - camera->location[i];
    }
    normalize(forwards);

    cross(sideways, forwards, camera->up); //sideways = cross(forwards, up)
    normalize(sideways);

    for (i = 0; i < 3; i++)
        camera->look_at[i] += side_amt * sideways[i] + up_amt * camera->up[i];
}


void move_vertically(Camera *camera, double dt, float direction){
    camera->look_at[1] += direction * dt;
    camera->location[1] += direction * dt;
}

void update_view_matrix(Camera *camera){
	mat4_ident(camera->view_matrix);
    mat4_lookat(camera->view_matrix, camera->location, camera->look_at, camera->up);
}

void update_projection_matrix(Camera *camera){
    mat4_ident(camera->projection_matrix);
    mat4_persp(camera->projection_matrix, camera->fov, camera->aspect_ratio, camera->zNear, camera->zFar);
}
