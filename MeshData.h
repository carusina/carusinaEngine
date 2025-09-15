#pragma once

#include <directxtk/SimpleMath.h>

#include <string>
#include <vector>

#include "Vertex.h"

struct MeshData {
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	std::string albedoTextureFileName;
	std::string emissiveTextureFileName;
	std::string normalTextureFileName;
	std::string heightTextureFileName;
	std::string aoTextureFileName;
	std::string metallicTextureFileName;
	std::string roughnessTextureFileName;
};