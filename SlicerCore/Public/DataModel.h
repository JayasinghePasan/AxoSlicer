#pragma once

#include <vector>
#include <cstdint>

/// 3D Vector for position
struct Vertex
{
	float x, y, z;
	Vertex() : x(0), y(0), z(0) {}
	Vertex(float x, float y, float z) : x(x), y(y), z(z) {}
};

/// A body with list of vertices and indices
class MeshBody
{
public:
	MeshBody() = default;
	MeshBody(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
		: vertices(vertices), indices(indices) {}

	std::vector<Vertex> vertices; 
	std::vector<uint32_t> indices;
};

/// A collection of mesh bodies
class MeshModel
{
	std::vector<MeshBody> meshBodies;
};


