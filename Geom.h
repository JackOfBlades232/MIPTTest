#pragma once
#include "Vec.h"
#include "Utils.h"

struct rect_t {
    vec2f_t pos;
    vec2f_t size;
    inline rect_t() : pos(), size() {}
    inline rect_t(f32 pos_x, f32 pos_y, f32 size_x, f32 size_y) : pos(pos_x, pos_y), size(size_x, size_y) {}
    inline rect_t(vec2f_t pos, vec2f_t size) : pos(pos), size(size) {}
    inline rect_t(const rect_t &other) : pos(other.pos), size(other.size) {}
};

struct circle_t {
    vec2f_t center;
    f32 rad;

    inline circle_t() : center(), rad(0) {}
    inline circle_t(f32 center_x, f32 center_y, f32 rad) : center(center_x, center_y), rad(rad) {}
    inline circle_t(vec2f_t center, f32 rad) : center(center), rad(rad) {}
    inline circle_t(const circle_t &other) : center(other.center), rad(other.rad) {}
};

bool intersect_ray_with_rect(vec2f_t orig, vec2f_t dir, rect_t *rect, f32 *tmin, vec2f_t *normal_out);

inline bool rects_are_intersecting(rect_t *r1, rect_t *r2)
{
    return (r1->pos.x <= r2->pos.x + r2->size.x && r2->pos.x <= r1->pos.x + r1->size.x) &&
           (r1->pos.y <= r2->pos.y + r2->size.y && r2->pos.y <= r1->pos.y + r1->size.y);
}

inline bool circle_and_rect_are_intersecting(circle_t *c, rect_t *r)
{
    return (c->center.x + c->rad >= r->pos.x && c->center.x - c->rad <= r->pos.x + r->size.x) &&
           (c->center.y + c->rad >= r->pos.y && c->center.y - c->rad <= r->pos.y + r->size.y);
}
