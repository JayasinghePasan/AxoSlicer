#pragma once
#include "../pch.h"

struct GridParams 
{ 
	float spacing; 
	float dpi; 
	DirectX::XMFLOAT2 origin; 
};

class BottomGrid 
{
public:
	HRESULT Initialize(BoundingBox& globalBB);
	HRESULT Render();
	void SetDpi(float dpiScale);

private:
	float l_side = 40.0f; // lengthPerSide cm
	DirectX::XMFLOAT2 origin{ 0.f, 0.f };

	float dpi = 1.0f;

	Microsoft::WRL::ComPtr<ID3D11Buffer> gridVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> gridCB;
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;

};
