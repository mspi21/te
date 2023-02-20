#include "./vec.h"

Vec2f vec2f(float x, float y) {
    return (Vec2f) {
        .x = x,
        .y = y
    };
}

Vec4f vec4f(float x, float y, float z, float w) {
    return (Vec4f) {
        .x = x,
        .y = y,
        .z = z,
        .w = w
    };
}