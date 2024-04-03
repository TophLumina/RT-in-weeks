#pragma once

#include "rtweekend.h"

class aabb
{
public:
    interval x, y, z;

    aabb() {} // empty intervals

    aabb(const interval &ix, const interval &iy, const interval &iz) : x(ix), y(iy), z(iz) {}

    aabb(const point3 &a, const point3 &b)
    // two points serve as extrema for the bounding box
    {
        x = interval(fmin(a.x, b.x), fmax(a.x, b.x));
        y = interval(fmin(a.y, b.y), fmax(a.y, b.y));
        z = interval(fmin(a.z, b.z), fmax(a.z, b.z));
    }

    aabb(const aabb &a, const aabb &b)
    {
        x = interval(a.x, b.x);
        y = interval(a.y, b.y);
        z = interval(a.z, b.z);
    }

    aabb pad()
    {
        // if any side narrower than delta, get padding
        double delta = 1e-4;
        interval _x = (x.size() >= delta) ? x : x.expand(delta);
        interval _y = (y.size() >= delta) ? y : y.expand(delta);
        interval _z = (z.size() >= delta) ? z : z.expand(delta);

        return aabb(_x, _y, _z);
    }

    const interval &axis(int i) const
    {
        if (i == 1)
            return y;
        if (i == 2)
            return z;
        return x;
    }

    bool hit(const ray &r, interval ray_t) const
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

inline aabb operator+(const aabb &bbox, const vec3 &offset)
{
    return aabb(bbox.x + offset.x, bbox.y + offset.y, bbox.z + offset.z);
}

inline aabb operator+(const vec3 &offset, const aabb &bbox)
{
    return bbox + offset;
}