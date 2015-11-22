#version 400

out vec4 fragColor;
in vec3 m_color;

void main() {
    fragColor = vec4(m_color, 1.0);
    //fragColor = vec4(0.0, 1.0, 0.0, 1.0);
}