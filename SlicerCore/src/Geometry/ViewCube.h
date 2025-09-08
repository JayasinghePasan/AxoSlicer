#pragma once
#include "../pch.h"
#include "../../Public/iViewCube.h"
#include "../Rendering/RenderBasics/ViewBase.h"
#include <DirectXMath.h>

class ViewCube : public ComRoot<iViewCube>, public ViewBase
{
private:
    RenderState renderState;

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vs;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>  m_ps;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>  m_il;


    // Highlight 
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_highlightCB;
    unsigned int m_highlightMask = 0;

    // picking
    ID3D11PixelShader*  m_pickPS;
    ID3D11RenderTargetView* m_pickRTV;
    ID3D11Texture2D* m_pickTexture;
    ID3D11Texture2D* m_pickTextureStaging;
    ID3D11DepthStencilView* m_pickDepthStencil;

    bool initialized = false;
    bool initializedPick = false;

    bool renderdocLoaded = false;

    HRESULT initializeCube();
    HRESULT initializePick();

public:
    // iRenderView
    HRESULT __stdcall render() override;
    HRESULT __stdcall resize(const int widthPixels, const int heightPixels, const float dpiScale) override;
    HRESULT __stdcall getSurface(void** ppSurface) override;

    // iViewCube
    HRESULT __stdcall rotate(float dx, float dy) override;
    HRESULT __stdcall pick(int x, int y, int* faceId) override;
    HRESULT __stdcall setHighlight(unsigned int faceMask) override;
    HRESULT __stdcall resetView() override;
};