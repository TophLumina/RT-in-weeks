#pragma once

#include <cmath>
#include <limits>

class interval
{
public:
    double min, max;

    interval() : min(+std::numeric_limits<double>::infinity()), max(-std::numeric_limits<double>::infinity()) {} // Default interval is empty

    interval(double _min, double _max) : min(_min), max(_max) {}

    interval(const interval &a, const interval &b) : min(fmin(a.min, b.min)), max(fmax(a.max, b.max)) {}

    bool contains(double x) const
    {
        return min <= x && x <= max;
    }

    bool surrounds(double x) const
    {
        return min < x && x < max;
    }

    double clamp(double x) const
    {
        if (x < min)
            return min;
        if (x > max)
            return max;

        return x;
    }

    double size() const { return max - min; }

    interval expand(double delta) const
    {
        auto padding = delta / 2;
        return interval(min - padding, max + padding);
    }

    static const interval empty, universe;
};

const static interval empty(+std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity());
const static interval universe(-std::numeric_limits<double>::infinity(), +std::numeric_limits<double>::infinity());

inline interval operator+(const interval &ival, double offset)
{
    return interval(ival.min + offset, ival.max + offset);
}

inline interval operator+(double offset, const interval &ival)
{
    return ival + offset;
}