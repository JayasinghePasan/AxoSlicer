#pragma once

struct Vertex
{
    float x, y, z;
    float nx, ny, nz; // normals
    Vertex(float xx, float yy, float zz, float nnx, float nny, float nnz)
        : x(xx), y(yy), z(zz), nx(nnx), ny(nny), nz(nnz)
    {}
    Vertex() {}
};

struct Vector3
{
    float x, y, z;
    Vector3(float xx, float yy, float zz)
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


