#include "BottomGrid.h"
#include "../Rendering/RenderBasics/Direct3D.h"

using namespace Direct3D;
using namespace DirectX;

HRESULT BottomGrid::Initialize(BoundingBox& globalBB)
{
    if (!context)
        return S_FALSE;

    Vector3 bbcenter = globalBB.getCenter();
    origin = DirectX::XMFLOAT2(bbcenter.x, bbcenter.y);

    Vector3 verts[6] =
    {
        { -l_side + bbcenter.x, -l_side + bbcenter.y, 0.0f }, {  l_side + bbcenter.x, -l_side + bbcenter.y, 0.0f }, {  l_side + bbcenter.x, l_side + bbcenter.y, 0.0f },
        { -l_side + bbcenter.x, -l_side + bbcenter.y, 0.0f }, {  l_side + bbcenter.x,  l_side + bbcenter.y, 0.0f }, { -l_side + bbcenter.x, l_side + bbcenter.y, 0.0f }
    };

    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(verts);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA init{};
    init.pSysMem = verts;
    device11->CreateBuffer(&bd, &init, &gridVertexBuffer);
    

    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    device11->CreateBlendState(&blendDesc, &blendState);
    context->OMSetBlendState(blendState.Get(), nullptr, 0xffffffff);

    D3D11_BUFFER_DESC cbd{};
    cbd.ByteWidth = sizeof(GridParams);
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    device11->CreateBuffer(&cbd, nullptr, &gridCB);

    GridParams params;
    params.spacing = 5.0f;
    params.dpi = dpi;
    params.origin = origin;
    D3D11_MAPPED_SUBRESOURCE mapped{};
    context->Map(gridCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    memcpy(mapped.pData, &params, sizeof(params));
    context->Unmap(gridCB.Get(), 0);
    ID3D11Buffer* cb = gridCB.Get();
    context->PSSetConstantBuffers(1, 1, &cb);

    return S_OK;
}

HRESULT BottomGrid::Render()
{
    D3D11_INPUT_ELEMENT_DESC layout[] = 
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    if (SUCCEEDED(Direct3D::BindShadersFromCSO(L"GridVS.cso", L"GridPS.cso", layout, _countof(layout))))
    {
        UINT stride = sizeof(float) * 3;
        UINT offset = 0;
        ID3D11Buffer* vb = gridVertexBuffer.Get();
        context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        ID3D11Buffer* cb = gridCB.Get();
        context->PSSetConstantBuffers(1, 1, &cb);
        context->Draw(6, 0);
    }

    return S_OK;
}

void BottomGrid::SetDpi(float dpiScale)
{
    dpi = dpiScale;
}