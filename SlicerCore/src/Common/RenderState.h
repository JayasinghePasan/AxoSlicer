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

    float dpi = 1.0f; // dpi scale for rendering

    float fovY = float(M_PI)/4;

    DirectX::XMFLOAT3 pan{ 0.f, 0.f, 0.f };     // pan offset
    DirectX::XMFLOAT3 viewDir{ 1.f, 1.f, 1.f }; // camera direction vector
    ProjectionMode projection = ProjectionMode::Perspective;
};

enum class ViewMode
{
    Front,  // ViewCube pick faceID 0 -> +X
    Back,   // ViewCube pick faceID 1 -> -X
    Left,   // ViewCube pick faceID 2 -> +Y
    Right,  // ViewCube pick faceID 3 -> -Y
    Top,    // ViewCube pick faceID 4 -> -Z
    Bottom, // ViewCube pick faceID 5 -> +Z
};

enum class eViewDirection
{
    X_pos,
    Y_pos,
    Z_pos,
    X_neg,
    Y_neg,
    Z_neg,
    Invalid
};
