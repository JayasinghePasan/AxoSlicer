#pragma once
#include "TranslateBox.h"
#include "../Rendering/RenderBasics/Direct3D.h"

using namespace Direct3D;
using namespace DirectX;

struct sCBuffer
{
    UINT highlight;
    UINT pad[3];
};

HRESULT TranslateBox::Initialize(const BoundingBox& box)
{
    bbox = box;
    enabled = true;
    return BuildBuffer();
}

void TranslateBox::Disable()
{
    enabled = false;
    vertexBuffer.Reset();
    highlightCB.Reset();
}

HRESULT TranslateBox::BuildBuffer()
{
    if (!device11) return E_FAIL;

    Vector3 c = bbox.getCenter();
    float lenX = bbox.maxX - bbox.minX;
    float lenY = bbox.maxY - bbox.minY;
    float lenZ = bbox.maxZ - bbox.minZ;
    float len = max(max(lenX, lenY), lenZ) * 0.5f;

    // Choose a sensible thickness in world units (tweak to taste)
    const float diag = std::sqrt(lenX * lenX + lenY * lenY + lenZ * lenZ);
    float w = 0.01f * diag;                 // ~1% of model size
    w = max(w, 0.001f * diag);         // clamp min
    w = min(w, 0.15f * len);           // clamp max

    struct Vertex { float x, y, z; float nx, ny, nz; };
    std::vector<Vertex> verts;
    verts.reserve(3 * 36); // 3 boxes * 12 tris * 3 verts

    auto pushTri = [&](XMFLOAT3 a, XMFLOAT3 b, XMFLOAT3 cpos, XMFLOAT3 axisID)
        {
            verts.push_back({ a.x,a.y,a.z, axisID.x,axisID.y,axisID.z });
            verts.push_back({ b.x,b.y,b.z, axisID.x,axisID.y,axisID.z });
            verts.push_back({ cpos.x,cpos.y,cpos.z, axisID.x,axisID.y,axisID.z });
        };

    auto addBox = [&](float x0, float x1, float y0, float y1, float z0, float z1, XMFLOAT3 axisID)
        {
            // 8 corners
            XMFLOAT3 v000{ x0,y0,z0 }, v100{ x1,y0,z0 }, v110{ x1,y1,z0 }, v010{ x0,y1,z0 };
            XMFLOAT3 v001{ x0,y0,z1 }, v101{ x1,y0,z1 }, v111{ x1,y1,z1 }, v011{ x0,y1,z1 };

            // 6 faces (CCW outside)
            // -Z
            pushTri(v000, v100, v110, axisID); pushTri(v000, v110, v010, axisID);
            // +Z
            pushTri(v001, v011, v111, axisID); pushTri(v001, v111, v101, axisID);
            // -Y
            pushTri(v000, v001, v101, axisID); pushTri(v000, v101, v100, axisID);
            // +Y
            pushTri(v010, v110, v111, axisID); pushTri(v010, v111, v011, axisID);
            // -X
            pushTri(v000, v010, v011, axisID); pushTri(v000, v011, v001, axisID);
            // +X
            pushTri(v100, v101, v111, axisID); pushTri(v100, v111, v110, axisID);
        };

    // X bar: length along +X, square cross-section 2w×2w around Y/Z
    addBox(c.x, c.x + len, c.y - w, c.y + w, c.z - w, c.z + w, XMFLOAT3{ 1,0,0 });
    // Y bar
    addBox(c.x - w, c.x + w, c.y, c.y + len, c.z - w, c.z + w, XMFLOAT3{ 0,1,0 });
    // Z bar
    addBox(c.x - w, c.x + w, c.y - w, c.y + w, c.z, c.z + len, XMFLOAT3{ 0,0,1 });

    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = UINT(verts.size() * sizeof(Vertex));
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA init{};
    init.pSysMem = verts.data();

    vertexBuffer.Reset();
    return device11->CreateBuffer(&bd, &init, &vertexBuffer);
}

HRESULT TranslateBox::Render()
{
    if (!enabled || !vertexBuffer)
        return S_FALSE;

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0, 0,D3D11_INPUT_PER_VERTEX_DATA,0},
        {"NORMAL"  ,0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0}
    };

    if (SUCCEEDED(Direct3D::BindShadersFromCSO(L"TranslateAxesVS.cso", L"TranslateAxesPS.cso", layout, _countof(layout))))
    {
        if (!highlightCB)
        {
            D3D11_BUFFER_DESC cbd{};
            cbd.ByteWidth = sizeof(sCBuffer);
            cbd.Usage = D3D11_USAGE_DYNAMIC;
            cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            device11->CreateBuffer(&cbd, nullptr, &highlightCB);
        }
        sCBuffer cb = { 0u,{0,0,0} };
        D3D11_MAPPED_SUBRESOURCE mapped{};
        context->Map(highlightCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
        memcpy(mapped.pData, &cb, sizeof(cb));
        context->Unmap(highlightCB.Get(), 0);
        ID3D11Buffer* hcb = highlightCB.Get();
        context->PSSetConstantBuffers(2, 1, &hcb);

        UINT stride = sizeof(Vertex);
        UINT offset = 0;
        ID3D11Buffer* vb = vertexBuffer.Get();
        context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        D3D11_BUFFER_DESC bdq{};
        vertexBuffer->GetDesc(&bdq);
        UINT vcount = bdq.ByteWidth / sizeof(Vertex);
        context->Draw(vcount, 0);
    }
    return S_OK;
}

static float Distance2D(float px, float py, float x1, float y1, float x2, float y2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    if (dx == 0 && dy == 0)
        return std::sqrt((px - x1) * (px - x1) + (py - y1) * (py - y1));
    float t = ((px - x1) * dx + (py - y1) * dy) / (dx * dx + dy * dy);
    t = max(0.0f, min(1.0f, t));
    float projx = x1 + t * dx;
    float projy = y1 + t * dy;
    return std::sqrt((px - projx) * (px - projx) + (py - projy) * (py - projy));
}

HRESULT TranslateBox::Pick(int x, int y, BoundingBox globalBB, RenderState& rs, eViewDirection& dir)
{
    dir = eViewDirection::Invalid;
    if (!enabled)
        return S_FALSE;

    XMFLOAT3 sMin(globalBB.minX, globalBB.minY, globalBB.minZ);
    XMFLOAT3 sMax(globalBB.maxX, globalBB.maxY, globalBB.maxZ);
    XMVECTOR vmin = XMLoadFloat3(&sMin);
    XMVECTOR vmax = XMLoadFloat3(&sMax);
    XMVECTOR centerV = (vmin + vmax) * 0.5f;

    float cx = XMVectorGetX(centerV);
    float cy = XMVectorGetY(centerV);
    float cz = XMVectorGetZ(centerV);

    XMMATRIX M_center = XMMatrixTranslation(-cx, -cy, -cz);
    XMMATRIX M_rot = XMMatrixRotationRollPitchYaw(rs.yaw, rs.pitch, 0.0f);
    XMMATRIX M_pan = XMMatrixTranslation(-rs.pan.x - cx, -rs.pan.y - cy, -rs.pan.z - cz);
    XMMATRIX M = M_pan * M_rot * M_center;

    float d = 4 * std::sqrt((sMax.x - sMin.x) * (sMax.x - sMin.x) + (sMax.y - sMin.y) * (sMax.y - sMin.y) + (sMax.z - sMin.z) * (sMax.z - sMin.z)) * 0.5f - rs.distance;
    XMVECTOR dirV = XMLoadFloat3(&rs.viewDir);
    dirV = XMVector3Normalize(dirV);
    XMVECTOR eye = XMVectorScale(dirV, d);
    XMVECTOR at = XMVectorSet(-cx, -cy, -cz, 0);
    XMVECTOR up = XMVectorSet(0, 0, 1, 0);
    float dot = XMVectorGetX(XMVector3Dot(dirV, up));
    if (fabsf(dot) > 0.99f)
        up = XMVectorSet(0, 1, 0, 0);
    XMMATRIX V = XMMatrixLookAtLH(eye, at, up);

    float aspect = (rs.height > 0) ? float(rs.width) / float(rs.height) : 1.0f;
    XMMATRIX P;
    if (rs.projection == ProjectionMode::Perspective)
    {
        float fovY = (rs.fovY > 0.f) ? rs.fovY : XM_PIDIV4;
        P = XMMatrixPerspectiveFovLH(fovY, aspect, 0.01f, 1000.0f);
    }
    else
    {
        float halfH = max(rs.distance, 0.01f);
        float halfW = halfH * aspect;
        P = XMMatrixOrthographicLH(2.f * halfW, 2.f * halfH, 0.01f, 1000.0f);
    }

    XMMATRIX MVP = M * V * P;

    Vector3 center = bbox.getCenter();
    float lenX = bbox.maxX - bbox.minX;
    float lenY = bbox.maxY - bbox.minY;
    float lenZ = bbox.maxZ - bbox.minZ;
    float len = max( lenX, lenY, lenZ ) * 0.5f;

    auto project = [&](float px, float py, float pz)
        {
            XMVECTOR p = XMVectorSet(px, py, pz, 1.0f);
            p = XMVector3Transform(p, MVP);
            float sx = XMVectorGetX(p) / XMVectorGetW(p);
            float sy = XMVectorGetY(p) / XMVectorGetW(p);
            sx = (sx * 0.5f + 0.5f) * rs.width;
            sy = (-sy * 0.5f + 0.5f) * rs.height;
            return XMFLOAT2(sx, sy);
        };

    XMFLOAT2 c2d = project(center.x, center.y, center.z);
    XMFLOAT2 x2d = project(center.x + len, center.y, center.z);
    XMFLOAT2 y2d = project(center.x, center.y + len, center.z);
    XMFLOAT2 z2d = project(center.x, center.y, center.z + len);

    const float threshold = 10.0f;
    float distX = Distance2D((float)x, (float)y, c2d.x, c2d.y, x2d.x, x2d.y);
    float distY = Distance2D((float)x, (float)y, c2d.x, c2d.y, y2d.x, y2d.y);
    float distZ = Distance2D((float)x, (float)y, c2d.x, c2d.y, z2d.x, z2d.y);

    float minDist = min( distX, distY, distZ );
    if (minDist > threshold)
        return S_OK;

    if (minDist == distX)
        dir = eViewDirection::X_pos;
    else if (minDist == distY)
        dir = eViewDirection::Y_pos;
    else
        dir = eViewDirection::Z_pos;
    return S_OK;
}