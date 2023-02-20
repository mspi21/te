#ifndef FONT_H_
#define FONT_H_

#include <stdbool.h>

#define GLEW_STATIC
#include <GL/glew.h>
#define GL_GLEXT_PROTOTYPES

#include <ft2build.h>
#include FT_FREETYPE_H

#include "./vec.h"
#include "./renderer.h"

typedef struct {
    float advance_x;
    float advance_y;
    
    float bitmap_width;
    float bitmap_height;
    
    float bitmap_left;
    float bitmap_top;

    float texture_x;
} GlyphMetric;

typedef struct {
    FT_UInt width;
    FT_UInt height;
    GLuint texture;
    size_t count;
    GlyphMetric *metrics;
} FontAtlas;

typedef struct {
    FT_Library library;
    FT_Face face;
    FontAtlas atlas;
} Font;

bool font_init(Font *font, const char *filepath);

void font_render_line(
    Font *font,
    Renderer *renderer,
    const char *text,
    size_t text_length,
    Vec2f pos,
    Vec4f color
);

float font_calculate_width(
    Font *font,
    char *text,
    size_t text_length
);

void font_destroy(Font *font);

#endif // FONT_H_