#ifndef INPUTS_H
#define INPUTS_H

#include <GL/glfw3.h>

typedef struct Inputs {
    int a_down, d_down, w_down, s_down;
    int space_down, space_shift_down;
    
    int p_pressed, r_pressed, l_pressed;
    
    int left_mouse_down;    
} Inputs;

void update_mouse(Inputs *i, GLFWwindow *w,int button, int action, int mods);
void update_keys(Inputs *i, GLFWwindow *window, int key, int scancode, int action, int mod);

#endif
