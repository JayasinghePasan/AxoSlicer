#pragma once
#include "../pch.h"
#include "../Common/BoundingBox.h"
#include "../Common/RenderState.h"


class TranslateBox
{
public:
    HRESULT Initialize(const BoundingBox& box);
    void Disable();
    HRESULT Render();
    HRESULT Pick(int x, int y, BoundingBox globalBB, RenderState& rs, eViewDirection& dir);
private:
    BoundingBox bbox{};
    bool enabled = false;
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> highlightCB;
    UINT vertexCount = 0;
    HRESULT BuildBuffer();
};