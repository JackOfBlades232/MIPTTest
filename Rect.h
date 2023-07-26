#pragma once
#include "Vec.h"
#include "Utils.h"

struct rect_t {
    vec2f_t pos;
    vec2f_t size;
    inline rect_t() : pos(), size() {}
    inline rect_t(f32 pos_x, f32 pos_y, f32 size_x, f32 size_y) : pos(pos_x, pos_y), size(size_x, size_y) {}
    inline rect_t(vec2f_t pos, vec2f_t size) : pos(pos), size(size) {}
    inline rect_t(const rect_t &other) : pos(other.pos), size(other.pos) {}
};

bool intersect_ray_with_rect(vec2f_t orig, vec2f_t dir, rect_t *rect, f32 *tmin, vec2f_t *normal_out);

inline bool point_is_in_rect(vec2f_t point, rect_t *r)
{
    return point.x >= r->pos.x && point.x <= r->pos.x + r->size.x &&
           point.y >= r->pos.y && point.y <= r->pos.y + r->size.y;
}

inline bool rects_are_intersecting(rect_t *r1, rect_t *r2)
{
    return (r1->pos.x <= r2->pos.x + r2->size.x && r2->pos.x <= r1->pos.x + r1->size.x) &&
           (r1->pos.y <= r2->pos.y + r2->size.y && r2->pos.y <= r1->pos.y + r1->size.y);
}
