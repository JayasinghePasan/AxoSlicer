#pragma once
#include "../pch.h"
#include "../../Public/iViewCube.h"
#include "../Rendering/RenderBasics/ViewBase.h"
#include <DirectXMath.h>

class ViewCube : public ComRoot<iViewCube>, public ViewBase
{
private:
    RenderState renderState;
    int highlightFace = -1;

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_cbState;

    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vs;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>  m_ps;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>  m_psPick;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>  m_il;

    bool initialized = false;
    HRESULT initializeCube();

public:
    // iRenderView
    HRESULT __stdcall render() override;
    HRESULT __stdcall resize(const int widthPixels, const int heightPixels, const float dpiScale) override;
    HRESULT __stdcall getSurface(void** ppSurface) override;

    // iViewCube
    HRESULT __stdcall rotate(float dx, float dy) override;
    HRESULT __stdcall pick(int x, int y, int* faceId) override;
    HRESULT __stdcall setHighlight(int faceId) override;
};