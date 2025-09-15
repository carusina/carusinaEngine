#pragma once

#include <directxtk/SimpleMath.h>

#include <vector>

struct Vertex {
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector3 normalModel;
	DirectX::SimpleMath::Vector2 texcoord;
	DirectX::SimpleMath::Vector3 tangentModel;
};