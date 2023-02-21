#ifndef RENDERER_H_
#define RENDERER_H_

#include <stdbool.h>
#include <GL/glew.h>

#include "./vec.h"

typedef enum {
    SHADER_TEXT = 0,
    SHADER_SOLID,
    COUNT_SHADERS
} Shader;

typedef struct {
    Vec2f position;
    Vec2f uv;
    Vec4f color;
} Vertex;

#define VERTEX_BUFFER_SIZE (3*10000)

typedef enum {
    VERTEX_ATTR_POSITION = 0,
    VERTEX_ATTR_UV,
    VERTEX_ATTR_COLOR,
} VertexAttr;

typedef enum {
    UNIFORM_RESOLUTION,
    UNIFORM_SCROLL_POS,
    COUNT_UNIFORMS
} Uniform;

typedef struct {
    GLuint vao;
    GLuint vbo;
    GLuint programs[COUNT_SHADERS];
    Shader current_shader;

    Vertex vertices[VERTEX_BUFFER_SIZE];
    size_t vertices_count;

    GLint uniforms[COUNT_UNIFORMS];

    Vec2f resolution;
    Vec2f scroll_pos;
    float scale;
} Renderer;

bool renderer_init(Renderer *renderer);

void renderer_vertex(
    Renderer *renderer,
    Vec2f p, Vec4f c, Vec2f uv
);

void renderer_triangle(
    Renderer *renderer,
    Vec2f p0, Vec2f p1, Vec2f p2,
    Vec4f c0, Vec4f c1, Vec4f c2,
    Vec2f uv0, Vec2f uv1, Vec2f uv2
);

void renderer_quad(
    Renderer *renderer,
    Vec2f p0, Vec2f p1, Vec2f p2, Vec2f p3,
    Vec4f c0, Vec4f c1, Vec4f c2, Vec4f c3,
    Vec2f uv0, Vec2f uv1, Vec2f uv2, Vec2f uv3
);

void renderer_solid_rect(
    Renderer *renderer,
    Vec2f position,
    Vec2f size,
    Vec4f color
);

void renderer_textured_rect(
    Renderer *renderer,
    Vec2f position,
    Vec2f size,
    Vec2f uv_position,
    Vec2f uv_size,
    Vec4f color
);

void renderer_set_resolution(Renderer *renderer, int width, int height);

void renderer_set_shader(Renderer *renderer, Shader shader);

void renderer_flush(Renderer *renderer);

void renderer_draw(Renderer *renderer);

void renderer_destroy(Renderer *renderer);

#endif // RENDERER_H_