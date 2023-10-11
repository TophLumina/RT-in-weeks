#pragma once

#include "rtweekend.h"

class aabb
{
public:
    interval x, y, z;

    aabb(){} // empty intervals

    aabb(const interval& ix, const interval& iy, const interval& iz) : x(ix), y(iy), z(iz) {}

    aabb(const point3& a, const point3& b)
    // two points serve as extrema for the bounding box
    {
        x = interval(fmin(a.e[0], b.e[0]), fmax(a.e[0], b.e[0]));
        y = interval(fmin(a.e[1], b.e[1]), fmax(a.e[1], b.e[1]));
        z = interval(fmin(a.e[2], b.e[2]), fmax(a.e[2], b.e[2]));
    }

    const interval& axis(int i) const
    {
        if (i == 1)
            return y;
        if (i == 2)
            return z;
        return x;
    }

    bool hit(const ray& r, interval ray_t) const
    {
        for (int a = 0; a < 3; ++a)
        {
            auto t0 = fmin(axis(a).min - r.origin()[a] / r.direction()[a], axis(a).max - r.origin()[a] / r.direction()[a]);
            auto t1 = fmax(axis(a).min - r.origin()[a] / r.direction()[a], axis(a).max - r.origin()[a] / r.direction()[a]);

            ray_t.min = fmax(ray_t.min, t0);
            ray_t.max = fmin(ray_t.max, t1);

            if (ray_t.max <= ray_t.min)
                return false;
        }
        return true;
    }
};
