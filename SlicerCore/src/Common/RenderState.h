#pragma once
#include "DirectXMath.h"

enum class ProjectionMode
{
    Perspective = 0,
    Orthographic = 1
};

struct RenderState
{
    float width;      // viewport width
    float height;     // viewport height

    float yaw = 0.0f;      // horizontal rotation
    float pitch = 0.0f;    // vertical rotation
    float distance = 3.0f; // zoom or distance from target

    DirectX::XMFLOAT2 pan{ 0.f, 0.f }; // pan offset
    ProjectionMode projection = ProjectionMode::Perspective;
};


