#pragma once

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <iostream>
#include <string>
#include <vector>

#include "MeshData.h"
#include "Vertex.h"

class ModelLoader {
public:
	void Load(std::string basePath, std::string fileName, bool reverseNormals);

	void ProcessNode(aiNode *node, const aiScene *scene, DirectX::SimpleMath::Matrix tr);

	MeshData ProcessMesh(aiMesh *mesh, const aiScene *scene);

	std::string ReadFileName(aiMaterial *material, aiTextureType type);

	void UpdateTangents();

public:
	std::string m_basePath;
	std::vector<MeshData> m_meshes;
	bool m_isGLTF = false; // gltf or fbx
	bool m_reverseNormals = false;
};