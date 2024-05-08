#pragma once

#include "vec.h"

struct Vertex
{
    vec3 Position;
    vec3 Normal;
    vec3 Tangent;
    vec3 BiTangent;
    vec2 uv;
};