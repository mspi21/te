#include <stdio.h>
#include <assert.h>

#include "renderer.h"
#include "file.h"

#define VERT_SHADER_FILE_PATH "./shaders/simple.vert"

typedef struct {
    Uniform id;
    const char *name;
} UniformInfo;

static const UniformInfo uniform_info[COUNT_UNIFORMS] = {
    [UNIFORM_RESOLUTION] = {
        .id = UNIFORM_RESOLUTION,
        .name = "resolution"
    },
    [UNIFORM_SCROLL_POS] = {
        .id = UNIFORM_SCROLL_POS,
        .name = "scroll_pos"
    }
};

static const char *fragment_shader_paths[COUNT_SHADERS] = {
    [SHADER_SOLID] = "./shaders/simple_color.frag",
    [SHADER_TEXT] = "./shaders/simple_text.frag",
};

bool compile_shader(
    const char *source_file,
    GLenum shader_type,
    GLuint *shader
) {
    char *shader_source;
    size_t shader_src_len;

    if(!file_read(source_file, &shader_source, &shader_src_len))
        return false;

    *shader = glCreateShader(shader_type);
    glShaderSource(*shader, 1, (const char * const *) &shader_source, NULL);
    glCompileShader(*shader);

    GLint success = 0;
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);

    if(!success) {
        GLchar message[1024];
        GLsizei message_len = 0;
        glGetShaderInfoLog(*shader, sizeof(message), &message_len, message);
        fprintf(stderr, "Error: Could not compile shader from file %s\n", source_file);
        fprintf(stderr, "%.*s\n", message_len, message);
    }

    file_destroy(shader_source);
    return success;
}

bool link_program(GLuint program) {
    glLinkProgram(program);

    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        GLsizei message_length;
        GLchar message[1024];

        glGetProgramInfoLog(program, sizeof(message), &message_length, message);
        fprintf(stderr, "GL Program Linking Error: %.*s\n", message_length, message);
    }
    return success;
}

bool renderer_init(Renderer *renderer) {
    *renderer = (Renderer) {0};
    renderer->vertices_count = 0;
    renderer->scale = 2.0f;
    renderer->resolution = vec2f(1.0f, 1.0f);

    glGenVertexArrays(1, &renderer->vao);
    glBindVertexArray(renderer->vao);

    glGenBuffers(1, &renderer->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(renderer->vertices), renderer->vertices, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glVertexAttribPointer(
        VERTEX_ATTR_POSITION,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (GLvoid *) offsetof(Vertex, position)
    );

    glEnableVertexAttribArray(VERTEX_ATTR_UV);
    glVertexAttribPointer(
        VERTEX_ATTR_UV,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (GLvoid *) offsetof(Vertex, uv)
    );

    glEnableVertexAttribArray(VERTEX_ATTR_COLOR);
    glVertexAttribPointer(
        VERTEX_ATTR_COLOR,
        4,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (GLvoid *) offsetof(Vertex, color)
    );

    GLuint vert_shader, frag_shader;
    if(!compile_shader(VERT_SHADER_FILE_PATH, GL_VERTEX_SHADER, &vert_shader)) {
        return false;
    }

    for(int i = 0; i < COUNT_SHADERS; ++i) {
        if(!compile_shader(fragment_shader_paths[i], GL_FRAGMENT_SHADER, &frag_shader)) {
            return false;
        }
        renderer->programs[i] = glCreateProgram();
        glAttachShader(renderer->programs[i], vert_shader);
        glAttachShader(renderer->programs[i], frag_shader);
        
        if(!link_program(renderer->programs[i])) {
            glDeleteShader(frag_shader);
            glDeleteShader(vert_shader);
            return false;
        }
        
        glDeleteShader(frag_shader);
    }
    glDeleteShader(vert_shader);
    return true;
}

void renderer_vertex(
    Renderer *renderer,
    Vec2f p, Vec4f c, Vec2f uv
) {
    assert(renderer->vertices_count < VERTEX_BUFFER_SIZE);
    
    Vertex *new = &renderer->vertices[renderer->vertices_count++];
    *new = (Vertex) {
        .position = p,
        .uv = uv,
        .color = c
    };
}


void renderer_triangle(
    Renderer *renderer,
    Vec2f p0, Vec2f p1, Vec2f p2,
    Vec4f c0, Vec4f c1, Vec4f c2,
    Vec2f uv0, Vec2f uv1, Vec2f uv2
) {
    renderer_vertex(renderer, p0, c0, uv0);
    renderer_vertex(renderer, p1, c1, uv1);
    renderer_vertex(renderer, p2, c2, uv2);
}

// 2-3
// |\|
// 0-1
void renderer_quad(
    Renderer *renderer,
    Vec2f p0, Vec2f p1, Vec2f p2, Vec2f p3,
    Vec4f c0, Vec4f c1, Vec4f c2, Vec4f c3,
    Vec2f uv0, Vec2f uv1, Vec2f uv2, Vec2f uv3
) {
    renderer_triangle(renderer, p0, p1, p2, c0, c1, c2, uv0, uv1, uv2);
    renderer_triangle(renderer, p1, p2, p3, c1, c2, c3, uv1, uv2, uv3);
}

void renderer_solid_rect(
    Renderer *renderer,
    Vec2f position,
    Vec2f size,
    Vec4f color
) {
    renderer_quad(
        renderer,

        position,
        (Vec2f) {position.x + size.x, position.y}, 
        (Vec2f) {position.x, position.y + size.y},
        (Vec2f) {position.x + size.x, position.y + size.y},

        color, color, color, color,
        vec2f(0, 0), vec2f(0, 0), vec2f(0, 0), vec2f(0, 0)
    );
}

void renderer_textured_rect(
    Renderer *renderer,
    Vec2f position,
    Vec2f size,
    Vec2f uv_position,
    Vec2f uv_size,
    Vec4f color
) {
    renderer_quad(
        renderer,
        
        position,
        (Vec2f) {position.x + size.x, position.y}, 
        (Vec2f) {position.x, position.y + size.y},
        (Vec2f) {position.x + size.x, position.y + size.y},

        color, color, color, color,

        uv_position,
        (Vec2f) {uv_position.x + uv_size.x, uv_position.y}, 
        (Vec2f) {uv_position.x, uv_position.y + uv_size.y},
        (Vec2f) {uv_position.x + uv_size.x, uv_position.y + uv_size.y}
    );
}

void renderer_set_resolution(Renderer *renderer, int width, int height) {
    renderer->resolution.x = (float) width;
    renderer->resolution.y = (float) height;
}

void renderer_set_shader(Renderer *renderer, Shader shader) {
    glUseProgram(renderer->programs[shader]);
    for(Uniform u = 0; u < COUNT_UNIFORMS; ++u) {
        renderer->uniforms[u] = glGetUniformLocation(
            renderer->programs[renderer->current_shader],
            uniform_info[u].name
        );
    }
    glUniform2f(renderer->uniforms[UNIFORM_RESOLUTION], renderer->resolution.x, renderer->resolution.y);
    glUniform1f(renderer->uniforms[UNIFORM_SCROLL_POS], renderer->scroll_pos);
    renderer->current_shader = shader;
}

void renderer_flush(Renderer *renderer) {
    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        renderer->vertices_count * sizeof(Vertex),
        renderer->vertices
    );
    renderer_draw(renderer);
    renderer->vertices_count = 0;
}

void renderer_draw(Renderer *renderer) {
    glDrawArrays(GL_TRIANGLES, 0, renderer->vertices_count);
}

void renderer_destroy(Renderer *renderer) {
    (void) renderer;
}
