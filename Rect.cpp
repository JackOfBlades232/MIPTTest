#include "Rect.h"
#include "Utils.h"

bool intersect_ray_with_rect(vec2f_t orig, vec2f_t dir, rect_t *rect, f32 *tmin, f32 *tmax)
{
    // @TODO: calculate normal (and hit point? and discard t_max?)

    vec2f_t r_min = rect->pos;
    vec2f_t r_max = rect->pos + rect->size;

    // Calculate the intersection t-s for all axis-aligned lines
    f32 tx1, tx2, ty1, ty2;
    tx1 = (r_min.x - orig.x) / dir.x;
    tx2 = (r_max.x - orig.x) / dir.x;
    if (tx1 > tx2)
        swapf(&tx1, &tx2);
    ty1 = (r_min.y - orig.y) / dir.y;
    ty2 = (r_max.y - orig.y) / dir.y;
    if (ty1 > ty2)
        swapf(&ty1, &ty2);

    // Calculate the boundaries of the intersection of the two intervals
    f32 int_min_t = MAX(tx1, ty1);
    f32 int_max_t = MIN(tx2, ty2);

    // The ray's line does not intersect the innerds of the bounds
    // Or the rect is intersected by the other side of the line
    if (int_min_t > int_max_t || int_max_t < 0)
        return false;

    *tmin = int_min_t;
    *tmax = int_max_t;
    return true;
}
