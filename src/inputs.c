#include "inputs.h"

void update_mouse(Inputs *i, GLFWwindow *w,int button, int action, int mods){
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        i->left_mouse_down = 1;        
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        i->left_mouse_down = 0;
    }
}

void update_keys(Inputs *i, GLFWwindow *window, int key, int scancode, int action, int mod){
    i->p_pressed = 0;
    i->r_pressed = 0;
    i->l_pressed = 0;

    if(key == GLFW_KEY_P && action == GLFW_PRESS){
        i->p_pressed = 1;
    }

    if(key == GLFW_KEY_R && action == GLFW_PRESS){
        i->r_pressed = 1;
    }

    if(key == GLFW_KEY_L && action == GLFW_PRESS){
        i->l_pressed = 1;
    }

    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        i->a_down = 1;
    }else if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
        i->a_down = 0;
    }

    if (key == GLFW_KEY_D && action == GLFW_PRESS) {
        i->d_down = 1;
    }else if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
        i->d_down = 0;
    }
    
    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        i->w_down = 1;
    }else if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
        i->w_down = 0;
    }

    if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        i->s_down = 1;
    }else if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
        i->s_down = 0;
    }

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        if(mod == GLFW_MOD_SHIFT) i->space_shift_down = 1;
        i->space_down = 1;
    } else if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
        i->space_down = 0;
        i->space_shift_down = 0;
    }
}
