#include "./font.h"

#define FONT_RANGE_LO 32 // inclusive
#define FONT_RANGE_HI 128 // exclusive

bool font_init(Font *font, const char *filepath) {
    // Initialize the Freetype library
    if(FT_Init_FreeType(&font->library)) {
        fprintf(stderr, "Error: Could not initialize FreeType\n");
        return false;
    }

    // Load the font from the file
    if(FT_New_Face(
        font->library,
        filepath,
        0,
        &font->face
    )) {
        fprintf(stderr, "Error: Could not load font from file: %s\n", filepath);
        goto fail_lib;
    }

    // Initialize atlas
    font->atlas = (FontAtlas) {0};
    font->atlas.count = FONT_RANGE_HI;
    font->atlas.metrics = (GlyphMetric *) calloc(font->atlas.count, sizeof(GlyphMetric));

    FT_Int32 load_flags = FT_LOAD_RENDER | FT_LOAD_TARGET_(FT_RENDER_MODE_SDF);
    for(int i = FONT_RANGE_LO; i < FONT_RANGE_HI; ++i) {
        if(FT_Load_Char(font->face, i, load_flags)) {
            fprintf(stderr, "Error: Could not load glyph of the character with code %d\n", i);
            goto fail_glyphs;
        }
        if(FT_Render_Glyph(font->face->glyph, FT_RENDER_MODE_SDF)) {
            fprintf(stderr, "Error: Could not render glyph of the character with code %d\n", i);
            goto fail_glyphs;
        }

        font->atlas.width += font->face->glyph->bitmap.width;
        if(font->atlas.height < font->face->glyph->bitmap.rows) {
            font->atlas.height = font->face->glyph->bitmap.rows;
        }
    }

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &font->atlas.texture);
    glBindTexture(GL_TEXTURE_2D, font->atlas.texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        font->atlas.width,
        font->atlas.height,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        NULL
    );

    // Render all ASCII glyphs into a single GL texture
    int x = 0;
    for(int i = FONT_RANGE_LO; i < FONT_RANGE_HI; ++i) {
        if(FT_Load_Char(font->face, i, load_flags)) {
            fprintf(stderr, "Error: Could not load glyph of the character with code %d\n", i);
            goto fail_glyphs;
        }

        if(FT_Render_Glyph(font->face->glyph, FT_RENDER_MODE_SDF)) {
            fprintf(stderr, "Error: Could not render glyph of the character with code %d\n", i);
            goto fail_glyphs;
        }

        font->atlas.metrics[i].advance_x = font->face->glyph->advance.x / 64;
        font->atlas.metrics[i].advance_y = font->face->glyph->advance.y / 64;
        font->atlas.metrics[i].bitmap_width = font->face->glyph->bitmap.width;
        font->atlas.metrics[i].bitmap_height = font->face->glyph->bitmap.rows;
        font->atlas.metrics[i].bitmap_left = font->face->glyph->bitmap_left;
        font->atlas.metrics[i].bitmap_top = font->face->glyph->bitmap_top;
        font->atlas.metrics[i].texture_x = (float) x / (float) font->atlas.width;

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexSubImage2D(
            GL_TEXTURE_2D,
            0,
            x,
            0,
            font->face->glyph->bitmap.width,
            font->face->glyph->bitmap.rows,
            GL_RED,
            GL_UNSIGNED_BYTE,
            font->face->glyph->bitmap.buffer
        );
        x += font->face->glyph->bitmap.width;
    }

    return true;

fail_glyphs:
    free(font->atlas.metrics);
    FT_Done_Face(font->face);
fail_lib:
    FT_Done_FreeType(font->library);
    return false;
}

void font_render_line(
    Font *font,
    Renderer *renderer,
    const char *text,
    size_t text_length,
    Vec2f pos,
    Vec4f color
) {
    renderer_set_shader(renderer, SHADER_TEXT);
    for(size_t i = 0; i < text_length; ++i) {
        size_t glyph = text[i];
        if(glyph < FONT_RANGE_LO || glyph >= FONT_RANGE_HI)
            glyph = '?';
        GlyphMetric metric = font->atlas.metrics[glyph];

        float x = pos.x + metric.bitmap_left;
        float y = pos.y - metric.bitmap_top;
        
        pos.x += metric.advance_x;
        pos.y += metric.advance_y;

        renderer_textured_rect(
            renderer,
            vec2f(x, y),
            vec2f(metric.bitmap_width, metric.bitmap_height),
            vec2f(metric.texture_x, 0),
            vec2f(
                metric.bitmap_width / (float) font->atlas.width,
                metric.bitmap_height / (float) font->atlas.height
            ),
            color
        );
    }
    renderer_flush(renderer);
}

float font_calculate_width(
    Font *font,
    char *text,
    size_t text_length
) {
    float width = 0.0f;
    
    for(size_t i = 0; i < text_length; ++i) {
        size_t glyph = text[i];
        if(glyph < FONT_RANGE_LO || glyph >= FONT_RANGE_HI)
            glyph = '?';
        GlyphMetric metric = font->atlas.metrics[glyph];

        width += metric.advance_x;
    }
    return width;
}

void font_destroy(Font *font) {
    if(!font)
        return;
    
    free(font->atlas.metrics);
    FT_Done_Face(font->face);
    FT_Done_FreeType(font->library);
}
