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
    // Andrew Kensler's optimized method
    {
        for (int a = 0; a < 3; ++a)
        {
            auto invD = 1.0 / r.direction()[a];
            auto orig = r.origin()[a];

            double t0 = (axis(a).min - orig) * invD;
            double t1 = (axis(a).max - orig) * invD;

            if (invD < 0)
                std::swap(t0, t1);
            
            if (t0 > ray_t.min)
                ray_t.min = t0;
            if (t1 < ray_t.max)
                ray_t.max = t1;
            
            if (ray_t.max <= ray_t.min)
                return false;
        }
        return true;
    }
};
