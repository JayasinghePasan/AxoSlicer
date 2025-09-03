#pragma once
#include "../../Public/iGeometry.h"
#include "../pch.h"

class Geometry : public ComRoot<iGeometry>
{
public:
	// iGeometry
	HRESULT __stdcall GetGuid(GUID& guid) override;
	HRESULT __stdcall Render() override;
	HRESULT __stdcall GetBoundingBox(BoundingBox& box) override;

	HRESULT LoadFromBuffer(const void* buffer, size_t length);
	void UploadToGPUBuffers();

private:
	GUID geometryID;					// set when loading from the input read buffer
	
	UINT trianglesCount;
	std::vector<Triangle> triangles;    // gets cleared soon after uploaded to buffer
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;

	BoundingBox boundingBox;

};
