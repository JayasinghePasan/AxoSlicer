#pragma once

struct Vertex
{
    float x, y, z;
    Vertex(float xx, float yy, float zz)
        : x(xx), y(yy), z(zz)
    {}
};

struct Triangle
{
    float normal[3];
    float v1[3];
    float v2[3];
    float v3[3];
    uint16_t attributeByteCount;
};


