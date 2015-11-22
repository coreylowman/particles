#ifdef _WIN32
#include <Windows.h>
#endif
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <GL/glew.h>
#include <GL/glfw3.h>
#include "particle_system.h"
#include "mat4.h"
#include "shaders.h"
#include "inputs.h"
#include "camera.h"

static GLFWwindow *window;

static Shader shader;
static Inputs inputs;
static Camera camera;
static ParticleSystem *psystem;
static GLuint psys_vbo[2], psys_vao;
static GLuint lines_vbo[2], lines_vao;

static double last_update_seconds;
static double last_fps_seconds;
static int draw_count;

static int width = 1280,height = 960;

static double mouse_start_pos[2];
static int paused;
static int attractor_captured;
static float attractor_capture_dist;

static float lines_vertices[] = {
    0.0, 0.0, 0.0,      //attractor x line
    0.0, 0.0, 0.0,      //attractor x line
    0.0, 0.0, 0.0,      //attractor y line
    0.0, 0.0, 0.0,      //attractor y line
    0.0, 0.0, 0.0,      //attractor z line
    0.0, 0.0, 0.0,      //attractor z line
	-1000.0, 0.0, 0.0, //x axis left
    1000.0, 0.0, 0.0, //x axis right
    0.0, -1000.0, 0.0, //y axis left
    0.0, 1000.0, 0.0, //y axis right
    0.0, 0.0, -1000.0, //z axis left
    0.0, 0.0, 1000.0 //z axis right
};

static float lines_colors[] = {
	1.0, 0.0, 0.0,
    1.0, 0.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 0.0, 1.0,
	0.0, 0.0, 1.0,
    1.0, 0.0, 0.0,
    1.0, 0.0, 0.0,
	0.0, 1.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 0.0, 1.0,
	0.0, 0.0, 1.0
};

static void get_ray_dir(const double *screen, GLfloat *out_ray){
	//http://antongerdelan.net/opengl/raycasting.html
	GLfloat ray_clip[4], ray_eye[4], ray_world[4];
	GLfloat projection_inverse[16], view_inverse[16];
	mat4_inverse(projection_inverse, camera.projection_matrix);
	mat4_inverse(view_inverse, camera.view_matrix);

	ray_clip[0] = screen[0]; ray_clip[1] = screen[1]; ray_clip[2] = 1.0; ray_clip[3] = 1.0;

	mat4_mul_vec4(ray_eye, projection_inverse, ray_clip);// ray_eye = projection_inverse * ray_clip
	mat4_mul_vec4(ray_world, view_inverse, ray_eye); //ray_world = view_inverse * ray_eye

	ray_world[0] /= ray_world[3];
	ray_world[1] /= ray_world[3];
	ray_world[2] /= ray_world[3];

	normalize(ray_world); // only normalizes the first 3 components

	out_ray[0] = ray_world[0];
	out_ray[1] = ray_world[1];
	out_ray[2] = ray_world[2];
}

static int raycast(const double *screen){
    GLfloat ray_world[3];

	get_ray_dir(screen, ray_world);

    //http://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms
    float lb[3], rt[3], t[6];
    int i;
    for(i = 0;i < 3;i++){
        lb[i] = psystem->attractor[i] - psystem->attractor_size;
        rt[i] = psystem->attractor[i] + psystem->attractor_size;
    }

	double T_1[3], T_2[3];
	double t_near = -DBL_MAX;
	double t_far = DBL_MAX;

	for (i = 0; i < 3; i++) {
		if (ray_world[i] == 0) {
			if (camera.location[i] < lb[i] || camera.location[i] > rt[i]){
				printf("NOPE1!\n");
				return 0; // parallel AND outside box : no intersection possible
			}
		} else { // ray not parallel to planes in this direction
			T_1[i] = (lb[i] - camera.location[i]) / ray_world[i];
			T_2[i] = (rt[i] - camera.location[i]) / ray_world[i];

			if (T_1[i] > T_2[i]) {
				double q = T_2[i];
				T_2[i] = T_1[i];
				T_1[i] = q;
			}

			if (T_1[i] > t_near) {
				t_near = T_1[i];
			}
			if (T_2[i] < t_far) {
				t_far = T_2[i];
			}
			if ((t_near > t_far) || (t_far < 0)) {
				printf("NOPE2!\n");
				return 0;
			}
		}
	}

	attractor_capture_dist = t_near;
	return 1;
}

static void mouse_callback(GLFWwindow *w,int button, int action, int mods)
{
    update_mouse(&inputs, w, button, action, mods);
    if(inputs.left_mouse_down){
        double mouse[2];
        glfwGetCursorPos(w, &mouse_start_pos[0], &mouse_start_pos[1]);      
        mouse[0] = 2 * mouse_start_pos[0] / (float)width - 1;
        mouse[1] = -2 * mouse_start_pos[1] / (float)height + 1;
        if(raycast(mouse)){
            attractor_captured = 1;
            printf("HIT!\n");
        }
    }else{
        attractor_captured = 0;
    }    
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mod){
	update_keys(&inputs, window, key, scancode, action, mod);
    if(inputs.p_pressed){
        paused = !paused;
        if(paused) printf("PAUSED!\n");
        else printf("UN-PAUSED!\n");
    }

    if(inputs.r_pressed){
        camera_init(&camera, width, height);
        particle_system_init(psystem);
        printf("RESET!\n");
        paused = 1;
        printf("PAUSED!\n");
    }

    if(inputs.l_pressed){
		camera_init(&camera, width, height);
        particle_system_init(psystem);
        printf("RESET!\n");
        init_shaders(&shader, "vertex_shader.glsl", "fragment_shader.glsl");
        printf("RELOADED SHADERS!\n");
        paused = 1;
        printf("PAUSED!\n");
    }
}

void window_size_callback (GLFWwindow* window, int _width, int _height) {
    width = _width;
    height = _height;
	glViewport(0, 0, width, height);
    camera.aspect_ratio = (float)width / height;
    update_projection_matrix(&camera);
}

static void init_glfw(){
    if (!glfwInit())
        exit(EXIT_FAILURE);

    window = glfwCreateWindow(width, height, "particles", NULL, NULL);
    
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    //callbacks
    glfwSetMouseButtonCallback(window, mouse_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);
	
    glfwGetFramebufferSize(window, &width, &height);

	glEnable(GL_DEPTH_TEST);
}

static void init_glew(){
    // get version info
    glewInit();
	glewExperimental = 1; // Needed for core profile
    const GLubyte* renderer = glGetString (GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString (GL_VERSION); // version as a string
    printf ("Renderer: %s\n", renderer);
    printf ("OpenGL version supported %s\n", version);
}

static void update_fps(double total_time){
	double fps_dt = total_time - last_fps_seconds;
	if (fps_dt > 0.25) {
		last_fps_seconds = total_time;
		char title[64];
		sprintf(title, "particles (fps: %.2f)", (double)draw_count / fps_dt);
		glfwSetWindowTitle(window, title);

		draw_count = 0;
	}
}

static void drag_attractor(){
	double mouse[2];
	glfwGetCursorPos(window, &mouse_start_pos[0], &mouse_start_pos[1]);
	mouse[0] = 2 * mouse_start_pos[0] / (float)width - 1;
	mouse[1] = -2 * mouse_start_pos[1] / (float)height + 1;
	GLfloat ray_dir[3];
	get_ray_dir(mouse, ray_dir);

	int i;
	for (i = 0; i < 3; i++) {
		psystem->attractor[i] = camera.location[i] + ray_dir[i] * attractor_capture_dist;
	}
}

static void update(double total_time){
	double update_dt = total_time - last_update_seconds;
	if (update_dt > 0.01) {
        if(!paused){
    		particle_system_update(psystem, update_dt);
        }		

		if (inputs.space_down) { //move vertically
            if(inputs.space_shift_down){ //move down
                move_vertically(&camera, update_dt, -1.0);
                if(!paused && attractor_captured) psystem->attractor[1] -= update_dt;
            }else{ //move up
                move_vertically(&camera, update_dt, 1.0);
                if(!paused && attractor_captured) psystem->attractor[1] += update_dt;
            }
		}

		if (!paused && attractor_captured) {
			drag_attractor();
		}else if (inputs.left_mouse_down) {
			double mx, my;
			glfwGetCursorPos(window, &mx, &my);
			double dx = mx - mouse_start_pos[0];
			double dy = my - mouse_start_pos[1];
            rotate_view(&camera, dx / 100.0,-dy / 100.0);

            mouse_start_pos[0] = mx;
            mouse_start_pos[1] = my;
		}

        int attrac_forwards = 0, attrac_side = 0;

		if (inputs.w_down){
            move_forwards(&camera, update_dt, 1.0);
            if(attractor_captured) attrac_forwards = 1;
        }
        
        if (inputs.s_down){
            move_forwards(&camera, update_dt, -1.0);
            if(attractor_captured) attrac_forwards = -1;
        }
		
        if (inputs.a_down){
            strafe(&camera, update_dt, -1.0);
            if(attractor_captured) attrac_side = -1;
        }
		
        if (inputs.d_down){
            strafe(&camera, update_dt, 1.0);
            if(attractor_captured) attrac_side = 1;
        }

        if(attrac_forwards != 0){
            GLfloat forwards[3];
            get_forwards_dir(camera, forwards);
            psystem->attractor[0] += attrac_forwards * update_dt * forwards[0];
            psystem->attractor[1] += attrac_forwards * update_dt * forwards[1];
            psystem->attractor[2] += attrac_forwards * update_dt * forwards[2];
        }

        if(attrac_side != 0){
            GLfloat side[3];
            get_side_dir(camera, side);
            psystem->attractor[0] += attrac_side * update_dt * side[0];
            psystem->attractor[1] += attrac_side * update_dt * side[1];
            psystem->attractor[2] += attrac_side * update_dt * side[2];
        }

		if (attractor_captured){
			lines_vertices[0] = psystem->attractor[0];
			lines_vertices[1] = 0.0;
			lines_vertices[2] = 0.0;
			lines_vertices[3] = psystem->attractor[0];
			lines_vertices[4] = 0.0;
			lines_vertices[5] = psystem->attractor[2];

			lines_vertices[6] = psystem->attractor[0];
			lines_vertices[7] = 0.0;
			lines_vertices[8] = psystem->attractor[2];
			lines_vertices[9] = psystem->attractor[0];
			lines_vertices[10] = psystem->attractor[1];
			lines_vertices[11] = psystem->attractor[2];

			lines_vertices[12] = 0.0;
			lines_vertices[13] = 0.0;
			lines_vertices[14] = psystem->attractor[2];
			lines_vertices[15] = psystem->attractor[0];
			lines_vertices[16] = 0.0;
			lines_vertices[17] = psystem->attractor[2];
		}

		glBindBuffer(GL_ARRAY_BUFFER, lines_vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, 2 * 2 * 3 * 3 * sizeof(float), lines_vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, NULL);

		glBindBuffer(GL_ARRAY_BUFFER, psys_vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, psystem->num_floats * sizeof(float), psystem->cubes, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, NULL);	

		update_view_matrix(&camera);

		last_update_seconds = total_time;
	}
}

static void render(){
	draw_count += 1;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shader.program);

	glUniformMatrix4fv(shader.projection_matrix_location, 1, GL_TRUE, camera.projection_matrix);
	glUniformMatrix4fv(shader.view_matrix_location, 1, GL_TRUE, camera.view_matrix);

	glBindVertexArray(lines_vao);
	glDrawArrays(GL_LINES, 0, 3 * 3 * 2);
	
	glBindVertexArray(psys_vao);
	glDrawArrays(GL_TRIANGLES, 0, psystem->num_triangles);

	glBindVertexArray(NULL);
}

int main(int argc, char **argv)
{
    int mp;
    double total_time = 0;
	double temp_time = 0;

    if(argc >= 2){
        mp = atoi(argv[1]);
    }else{
        mp = 10000;
    }

    init_glfw();
    init_glew();
    init_shaders(&shader, "vertex_shader.glsl", "fragment_shader.glsl");

    srand(time(NULL));
    psystem = particle_system_new(mp);
    particle_system_init(psystem);

	glGenVertexArrays(1, &psys_vao);
	glBindVertexArray(psys_vao);
    glGenBuffers(2, psys_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, psys_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, psystem->num_floats * sizeof(float), psystem->cubes, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);	
	glBindBuffer(GL_ARRAY_BUFFER, psys_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, psystem->num_floats * sizeof(float), psystem->colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);    

	glGenVertexArrays(1, &lines_vao);
	glBindVertexArray(lines_vao);
	glGenBuffers(2, lines_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, lines_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 2 * 2 * 3 * 3 * sizeof(float), lines_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, lines_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, 2 * 2 * 3 * 3 * sizeof(float), lines_colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

    camera_init(&camera, width, height);

    total_time = glfwGetTime();
    last_update_seconds = total_time;
    last_fps_seconds = total_time;
    while (!glfwWindowShouldClose(window))
    {
        total_time = glfwGetTime();

        update_fps(total_time);
        update(total_time);
        render();

		glfwSwapBuffers(window);
		temp_time = glfwGetTime();
		glfwPollEvents();
		temp_time = glfwGetTime() - temp_time;
		last_update_seconds += temp_time;
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    particle_system_free(psystem);

    return 0;
}
