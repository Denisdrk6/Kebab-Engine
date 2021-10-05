#pragma once

#include "MathGeoLib/src/Math/float3.h"

#include <vector>

class Geometry
{
public:
	Geometry(math::float3 pos) : position(pos) {}
	virtual ~Geometry() {};

	inline void SetPos(const math::float3& pos) { position = pos; }
	inline const math::float3& GetPosition() { return position; }

	inline const std::vector<float>& GetVertices() { return vertices; }
	inline const std::vector<float>& GetNormals() { return normals; }
	inline const std::vector<float>& GetTextureCoords() { return texCoords; }
	inline const std::vector<uint32_t>& GetIndices() { return indices; }

protected:
	std::vector<float>vertices;
	std::vector<float>normals;
	std::vector<float>texCoords;
	std::vector<uint32_t>indices;

	math::float3 position;
};