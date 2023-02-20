#ifndef VEC_H_
#define VEC_H_

typedef struct {
    float x;
    float y;
} Vec2f;

typedef struct {
    float x;
    float y;
    float z;
    float w;
} Vec4f;

Vec2f vec2f(float x, float y);

Vec4f vec4f(float x, float y, float z, float w);

#endif // VEC_H_