#pragma once

#include <directxtk/SimpleMath.h>

#include <string>
#include <vector>

#include "MeshData.h"
#include "Vertex.h"

class GeometryGenerator {
public:
	static std::vector<MeshData> ReadFromFile(std::string basePath,
											  std::string fileName,
											  bool reverseNormal = false);

	static MeshData MakeSquare(const float scale = 1.0f,
							   const DirectX::SimpleMath::Vector2 texScale = DirectX::SimpleMath::Vector2(1.0f));
	
	static MeshData MakeSquareGrid(const int numSlices, const int numStacks, const float scale = 1.0f,
								   const DirectX::SimpleMath::Vector2 texScale = DirectX::SimpleMath::Vector2(1.0f));

	static MeshData MakeBox(const float scale = 1.0f);

	static MeshData MakeCylinder(const float bottomRadius, const float topRadius,
								 float height, int numSlices);

	static MeshData MakeSphere(const float radius, const int numSlices, const int numStacks,
							   const DirectX::SimpleMath::Vector2 texScale = DirectX::SimpleMath::Vector2(1.0f));

	static MeshData MakeTetrahedron();
	static MeshData MakeIcosahedron();
	static MeshData SubdivideToSphere(const float radius, MeshData meshData);
};