#pragma once
#include "math.h"
#include "DirectXMath.h"
# define M_PI  

enum class ProjectionMode
{
    Perspective = 0,
    Orthographic = 1
};

struct RenderState
{
    float width  = 1.0f; // viewport width
    float height = 1.0f; // viewport height

    float yaw = 0.0f;      // horizontal rotation
    float pitch = 0.0f;    // vertical rotation
    float distance = -1000.0f; // zoom or distance from target

    float fovY = float(M_PI)/4;

    DirectX::XMFLOAT3 pan{ 0.f, 0.f, 0.f }; // pan offset
    ProjectionMode projection = ProjectionMode::Perspective;
};


