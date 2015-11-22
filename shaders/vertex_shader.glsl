#version 400

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_color;

uniform mat4 projection;
uniform mat4 view;

out vec3 m_color;

void main() {
    gl_Position = projection * view * vec4(vertex_position, 1.0);

    m_color = vertex_color;
}