#pragma once

#include <cmath>
#include <iostream>

using std::sqrt;

class vec3
{
public:
    double e[3];

    vec3() : e{0, 0, 0} {}
    vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
    double operator[](int i) const { return e[i]; }
    double &operator[](int i) { return e[i]; }

    vec3 &operator+=(const vec3 &v)
    {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    vec3 &operator*=(double t)
    {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    vec3 operator/=(double t)
    {
        return *this *= 1 / t;
    }

    bool operator==(const vec3 &v) const
    {
        return v.e[0] == e[0] && v.e[1] == e[1] && v.e[2] == e[2];
    }

    bool operator!=(const vec3 &v) const
    {
        return !(*this == v);
    }

    double length_squared() const
    {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }

    double length() const
    {
        return sqrt(length_squared());
    }

    // Return true if vec3 is around (0, 0, 0)
    bool near_zero() const
    {
        auto s = 1e-16;
        return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
    }

    static vec3 random()
    {
        return vec3(random_double(), random_double(), random_double());
    }

    static vec3 random(double min, double max)
    {
        return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
    }
};

using point3 = vec3;

// Utility Funcs

inline std::ostream &operator<<(std::ostream &out, const vec3 &v)
{
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3 &u, const vec3 &v)
{
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator+(const vec3 &u, double d)
{
    return u + vec3(d, d, d);
}

inline vec3 operator-(const vec3 &u, const vec3 &v)
{
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3 &u, const vec3 &v)
{
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(double t, const vec3 &v)
{
    return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline vec3 operator*(const vec3 &v, double t)
{
    return t * v;
}

inline vec3 operator/(const vec3 &v, double t)
{
    return (1 / t) * v;
}

inline double dot(const vec3 &u, const vec3 &v)
{
    return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3 &u, const vec3 &v)
{
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 normalize(vec3 v)
{
    return v / v.length();
}

inline vec3 random_in_unit_disk()
{
    double r1 = random_double();
    double r2 = random_double();

    auto r = r1;
    auto theta = 2 * PI * r2;

    auto x = r * cos(theta);
    auto y = r * sin(theta);

    return vec3(x, y, 0);
}

inline vec3 random_unit_vector()
{
    double r1 = random_double();
    double r2 = random_double();

    auto x = cos(2 * PI * r1) * 2 * sqrt(r2 * (1 - r2));
    auto y = sin(2 * PI * r1) * 2 * sqrt(r2 * (1 - r2));
    auto z = 1 - 2 * r2;

    return vec3(x, y, z);
}

inline vec3 random_on_hemisphere(const vec3 &normal)
{
    vec3 on_unit_sphere = random_unit_vector();
    if (dot(on_unit_sphere, normal) > 0)
        return on_unit_sphere;
    else
        return -on_unit_sphere;
}

// Sampling direction weighted by cosine on upper hemisphere (z+)
// PDF = cos(phi) / PI or dir.z / PI or dot(w, dir) / PI
inline vec3 random_cosine_direction()
{
    auto r1 = random_double();
    auto r2 = random_double();

    auto phi = 2 * PI * r1;

    auto x = cos(phi) * sqrt(r2);
    auto y = sin(phi) * sqrt(r2);
    auto z = sqrt(1 - r2);

    return vec3(x, y, z);
}

inline vec3 reflect(const vec3 &v, const vec3 &n)
{
    return v - 2 * dot(v, n) * n;
}

inline vec3 refract(const vec3 &uv, const vec3 &n, double etai_over_etat)
{
    auto cos_theta = fmin(dot(-uv, n), 1.0);
    vec3 out_perp = etai_over_etat * (uv + cos_theta * n);
    vec3 out_parallel = -sqrt(fabs(1.0 - out_perp.length_squared())) * n;
    return out_perp + out_parallel;
}