#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec4 color;

uniform vec2 resolution;
uniform vec2 scroll_pos;

out vec2 out_uv;
out vec4 out_color;

void main() {
    gl_Position = vec4(
        2 * ((position.x - scroll_pos.x) / resolution.x - 0.5),
        -2 * ((position.y - scroll_pos.y) / resolution.y - 0.5),
        0,
        1
    );
    out_color = color;
    out_uv = uv;
}
