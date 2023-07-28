#include "Geom.h"
#include "Utils.h"

bool intersect_ray_with_rect(vec2f_t orig, vec2f_t dir, rect_t *rect, f32 *tmin, vec2f_t *normal_out)
{
    vec2f_t r_min = rect->pos;
    vec2f_t r_max = rect->pos + rect->size;

    bool swapped_x = false;
    bool swapped_y = false;

    // Calculate the intersection t-s for all axis-aligned lines
    f32 tx1, tx2, ty1, ty2;
    tx1 = (r_min.x - orig.x) / dir.x;
    tx2 = (r_max.x - orig.x) / dir.x;
    if (tx1 > tx2) {
        swapf(&tx1, &tx2);
        swapped_x = true;
    }
    ty1 = (r_min.y - orig.y) / dir.y;
    ty2 = (r_max.y - orig.y) / dir.y;
    if (ty1 > ty2) {
        swapf(&ty1, &ty2);
        swapped_y = true;
    }

    // Calculate the boundaries of the intersection of the two intervals
    f32 int_min_t = MAX(tx1, ty1);
    f32 int_max_t = MIN(tx2, ty2);

    // The ray's line does not intersect the innerds of the bounds
    // Or the rect is intersected by the other side of the line
    if (int_min_t > int_max_t || int_max_t < 0)
        return false;

    if (int_min_t == tx1) {
        if (swapped_x)
            *normal_out = vec2f_t(1, 0);
        else
            *normal_out = vec2f_t(-1, 0);
    } else {
        if (swapped_y)
            *normal_out = vec2f_t(0, 1);
        else
            *normal_out = vec2f_t(0, -1);
    }

    *tmin = int_min_t;
    return true;
}
