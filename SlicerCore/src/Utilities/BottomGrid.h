#pragma once
#include "../pch.h"

class BottomGrid 
{
public:
	HRESULT Initialize(BoundingBox& globalBB);
	HRESULT Render();

private:
	float l_side = 40.0f; // lengthPerSide cm
	Microsoft::WRL::ComPtr<ID3D11Buffer> gridVertexBuffer;
	Microsoft::WRL::ComPtr<	ID3D11BlendState> blendState;

};
