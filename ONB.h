#pragma once

#include "rtweekend.h"

class onb
{
public:
    onb() {}

    vec3 operator[](int i) const { return axis[i]; }
    vec3 &operator[](int i) { return axis[i]; }

    vec3 u() const { return axis[0]; }
    vec3 v() const { return axis[1]; }
    vec3 w() const { return axis[2]; }

    vec3 local(double i, double j, double k) const { return i * u() + j * v() + k * w(); }

    vec3 local(const vec3 &vec) const { return vec.x() * u() + vec.y() * v() + vec.z() * w(); }

    void build_from_w(const vec3& w)
    {
        vec3 w_norm = normalize(w);
        vec3 a = (fabs(w_norm.x()) > 0.9) ? vec3(0, 1, 0) : vec3(1, 0, 0);

        vec3 v = normalize(cross(w_norm, a));
        vec3 u = cross(w_norm, v);

        axis[0] = u;
        axis[1] = v;
        axis[2] = w_norm;
    }

    vec3 axis[3];
};