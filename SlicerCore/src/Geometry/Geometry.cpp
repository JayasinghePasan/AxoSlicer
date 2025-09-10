#pragma once
#include "Geometry.h"
#include "../pch.h"
#include "../Rendering/RenderBasics/Direct3D.h"

using namespace Direct3D;

HRESULT createGeometry(const void* buffer, size_t length, iGeometry** ppGeometry)
{
    if (!buffer )
        return E_POINTER;

    CComPtr<CComObject<Geometry>> geometry;
    HRESULT hr = createInstance(geometry);
    if (FAILED(hr))
        return hr;

    hr = geometry->LoadFromBuffer(buffer, length);
    if (FAILED(hr))
        return hr;

    *ppGeometry = geometry.Detach();
    return S_OK;
}


HRESULT Geometry::LoadFromBuffer(const void* buffer, size_t length)
{
    if (!buffer || length < 84)
        return E_FAIL;

    // Assign a unique geometry id
    if (FAILED(CoCreateGuid(&geometryID)))
        return E_FAIL;

    const auto* base = static_cast<const std::uint8_t*>(buffer);
    const auto* p = base;
    const auto* end = base + length;

    // --- Skip 80-byte header
    p += 80;

    // --- Read triangle count (little-endian)
    std::uint32_t triCount = 0;
    if (p + 4 > end) return E_FAIL;
    std::memcpy(&triCount, p, 4);
    p += 4;

    // --- Validate total size: 84 + 50 * triCount
    // (Use math that can’t overflow and also checks remaining bytes)
    if (length < 84ull || triCount >(length - 84ull) / 50ull)
        return E_FAIL;

    const size_t need = 84ull + 50ull * static_cast<size_t>(triCount);
    if (length < need)
        return E_FAIL;

    // --- On-disk facet is EXACTLY 50 bytes. Use a packed POD to read safely.
#pragma pack(push, 1)
    struct StlFacet50 {
        float n[3];     // normal (often junk in STL, usually recompute later)
        float v1[3];
        float v2[3];
        float v3[3];
        std::uint16_t attr; // attribute byte count (often 0)
    };
#pragma pack(pop)
    static_assert(sizeof(StlFacet50) == 50, "STL facet must be 50 bytes");

    // Prepare containers
    triangles.clear();
    triangles.reserve(triCount);

    boundingBox.minX = boundingBox.minY = boundingBox.minZ = std::numeric_limits<float>::infinity();
    boundingBox.maxX = boundingBox.maxY = boundingBox.maxZ = -std::numeric_limits<float>::infinity();

    // Small cleaner: zero NaN and subnormal floats (avoid #DEN, weird bbox)
    auto clean = [](float& x) {
        if (!std::isfinite(x) || std::fpclassify(x) == FP_SUBNORMAL) x = 0.0f;
        };
    auto clean3 = [&](float v[3]) { clean(v[0]); clean(v[1]); clean(v[2]); };

    // --- Read facets one by one
    for (std::uint32_t i = 0; i < triCount; ++i)
    {
        if (p + sizeof(StlFacet50) > end)
            return E_FAIL; // defensive per-iteration bound

        StlFacet50 facet;
        std::memcpy(&facet, p, sizeof(facet));
        p += sizeof(facet);

        // sanitize vertices
        clean3(facet.v1);
        clean3(facet.v2);
        clean3(facet.v3);

        // Convert to your runtime Triangle (don’t rely on sizeof(Triangle)==50)
        Triangle t{};
        t.v1[0] = facet.v1[0]; t.v1[1] = facet.v1[1]; t.v1[2] = facet.v1[2];
        t.v2[0] = facet.v2[0]; t.v2[1] = facet.v2[1]; t.v2[2] = facet.v2[2];
        t.v3[0] = facet.v3[0]; t.v3[1] = facet.v3[1]; t.v3[2] = facet.v3[2];
        triangles.push_back(t);

        // Expand bounding box
        boundingBox.expandToInclude(t.v1[0], t.v1[1], t.v1[2]);
        boundingBox.expandToInclude(t.v2[0], t.v2[1], t.v2[2]);
        boundingBox.expandToInclude(t.v3[0], t.v3[1], t.v3[2]);
    }

    // Optional integrity check: consumed exactly the STL payload
    if (p != base + need)
        return E_FAIL;

    UploadToGPUBuffers();
    return S_OK;
}

HRESULT __stdcall Geometry::Render()
{
    if (!visible) 
        return S_OK;

    if (!context || !vertexBuffer)
        return E_FAIL;

    UINT stride = sizeof(float) * 6;
    UINT offset = 0;
    ID3D11Buffer* vb = vertexBuffer.Get();

    context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
    context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->Draw(vertexCount, 0);
    return S_OK;
}

HRESULT __stdcall Geometry::GetGuid(GUID& guid)
{
    guid = geometryID;
    return S_OK;
}

void Geometry::UploadToGPUBuffers()
{
    // if already uploaded
    if (vertexBuffer)
        return;

    if (triangles.empty())
        return;
    
    std::vector<Vertex> vertices;
    vertices.reserve(triangles.size() * 3);

    for (const Triangle& tri : triangles)
    {
        float ux = tri.v2[0] - tri.v1[0];
        float uy = tri.v2[1] - tri.v1[1];
        float uz = tri.v2[2] - tri.v1[2];
        float vx = tri.v3[0] - tri.v1[0];
        float vy = tri.v3[1] - tri.v1[1];
        float vz = tri.v3[2] - tri.v1[2];
        float nx = uy * vz - uz * vy;
        float ny = uz * vx - ux * vz;
        float nz = ux * vy - uy * vx;
        float len = std::sqrt(nx * nx + ny * ny + nz * nz);
        if (len > 0.0f)
        {
            nx /= len; ny /= len; nz /= len;
        }

        vertices.push_back(Vertex(tri.v1[0], tri.v1[1], tri.v1[2], nx, ny, nz));
        vertices.push_back(Vertex(tri.v2[0], tri.v2[1], tri.v2[2], nx, ny, nz));
        vertices.push_back(Vertex(tri.v3[0], tri.v3[1], tri.v3[2], nx, ny, nz));
    }

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = static_cast<UINT>(vertices.size() * sizeof(Vertex));
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices.data();

    HRESULT hr = device11->CreateBuffer(&bd, &initData, &vertexBuffer);
    if (FAILED(hr))
    {
        vertexBuffer.Reset();
        return;
    }
    vertexCount = (UINT)vertices.size();
    triangles.clear();
    return;
}

void Geometry::SetVisibility(bool vis)
{
    visible = vis;
}

HRESULT Geometry::GetBoundingBox(BoundingBox& box)
{
    box = boundingBox;
    return S_OK;
}

HRESULT __stdcall Geometry::Translate(float dx, float dy, float dz)
{
    if (!vertexBuffer)
        return E_FAIL;

    // new staging buffer to get vertices from GPU
    D3D11_BUFFER_DESC desc = {};
    vertexBuffer->GetDesc(&desc);
    desc.Usage = D3D11_USAGE_STAGING;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
    desc.BindFlags = 0;
    desc.MiscFlags = 0;

    Microsoft::WRL::ComPtr<ID3D11Buffer> staging;
    device11->CreateBuffer(&desc, nullptr, &staging);

    // copy data to the staging buffer
    context->CopyResource(staging.Get(), vertexBuffer.Get());

    D3D11_MAPPED_SUBRESOURCE mapped = {};
    context->Map(staging.Get(), 0, D3D11_MAP_READ_WRITE, 0, &mapped);
    Vertex* vertices = reinterpret_cast<Vertex*>(mapped.pData);
    
    for (size_t i = 0; i < vertexCount; i++)
    {
        vertices[i].x += dx;
        vertices[i].y += dy;
        vertices[i].z += dz;
    }

    context->Unmap(staging.Get(), 0);

    // copy new vertices to the vertexBuffer back
    context->CopyResource(vertexBuffer.Get(), staging.Get());

    return S_OK;
}
