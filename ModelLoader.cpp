#include "ModelLoader.h"

#include <DirectXMesh.h>
#include <filesystem>
#include <vector>

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

string GetExtension(const string fileName)
{
	string ext(filesystem::path(fileName).extension().string());
	transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

	return ext;
}

void ModelLoader::Load(std::string basePath, std::string fileName, bool reverseNormals)
{
	if (GetExtension(fileName) == ".gltf")
	{
		m_isGLTF = true;
		m_reverseNormals = reverseNormals;
	}

	m_basePath = basePath;

	Assimp::Importer importer;
	
	const aiScene* pScene = importer.ReadFile(m_basePath + fileName,
											  aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
	if (!pScene)
	{
		cout << "Failed to read file: " << m_basePath + fileName << endl;
	}
	else
	{
		Matrix tr; // Initalize Transformation
		ProcessNode(pScene->mRootNode, pScene, tr);
	}

	UpdateTangents();
}

void ModelLoader::ProcessNode(aiNode* node, const aiScene* scene, DirectX::SimpleMath::Matrix tr)
{
	Matrix m;
	ai_real* temp = &node->mTransformation.a1;
	float* mTemp = &m._11;
	for (int t = 0; t < 16; t++)
	{
		mTemp[t] = float(temp[t]);
	}
	m = m.Transpose() * tr;

	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		MeshData newMesh = this->ProcessMesh(mesh, scene);

		for (Vertex& v : newMesh.vertices)
		{
			v.position = Vector3::Transform(v.position, m);
		}

		m_meshes.push_back(newMesh);
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		this->ProcessNode(node->mChildren[i], scene, m);
	}
}

MeshData ModelLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	vector<Vertex> vertices;
	vector<uint32_t> indices;

	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;

		vertex.normalModel.x = mesh->mNormals[i].x;
		if (m_isGLTF)
		{
			vertex.normalModel.y = mesh->mNormals[i].z;
			vertex.normalModel.z = -mesh->mNormals[i].y;
		}
		else
		{
			vertex.normalModel.y = mesh->mNormals[i].y;
			vertex.normalModel.z = mesh->mNormals[i].z;
		}

		if (m_reverseNormals)
		{
			vertex.normalModel *= -1.0f;
		}

		vertex.normalModel.Normalize();

		if (mesh->mTextureCoords[0])
		{
			vertex.texcoord.x = float(mesh->mTextureCoords[0][i].x);
			vertex.texcoord.y = float(mesh->mTextureCoords[0][i].y);
		}

		vertices.push_back(vertex);
	}

	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (UINT j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	MeshData newMesh;
	newMesh.vertices = vertices;
	newMesh.indices = indices;

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		newMesh.albedoTextureFileName = ReadFileName(material, aiTextureType_BASE_COLOR);
		if (newMesh.aoTextureFileName.empty())
		{
			newMesh.aoTextureFileName = ReadFileName(material, aiTextureType_DIFFUSE);
		}

		newMesh.emissiveTextureFileName = ReadFileName(material, aiTextureType_EMISSIVE);
		newMesh.heightTextureFileName = ReadFileName(material, aiTextureType_HEIGHT);
		newMesh.normalTextureFileName = ReadFileName(material, aiTextureType_NORMALS);
		newMesh.metallicTextureFileName = ReadFileName(material, aiTextureType_METALNESS);
		newMesh.roughnessTextureFileName = ReadFileName(material, aiTextureType_DIFFUSE_ROUGHNESS);
		newMesh.aoTextureFileName = ReadFileName(material, aiTextureType_AMBIENT_OCCLUSION);
		if (newMesh.aoTextureFileName.empty())
		{
			newMesh.aoTextureFileName = ReadFileName(material, aiTextureType_LIGHTMAP);
		}
	}

	return newMesh;
}

std::string ModelLoader::ReadFileName(aiMaterial* material, aiTextureType type)
{
	if (material->GetTextureCount(type) > 0)
	{
		aiString filePath;
		material->GetTexture(type, 0, &filePath);

		string fullPath = m_basePath + string(filesystem::path(filePath.C_Str()).filename().string());

		return fullPath;
	}

	return "";
}

void ModelLoader::UpdateTangents()
{
	for (MeshData& m : m_meshes)
	{
		vector<XMFLOAT3> positions(m.vertices.size());
		vector<XMFLOAT3> normals(m.vertices.size());
		vector<XMFLOAT2> texcoords(m.vertices.size());
		vector<XMFLOAT3> tangents(m.vertices.size());
		vector<XMFLOAT3> bitangents(m.vertices.size());

		for (size_t i = 0; i < m.vertices.size(); i++)
		{
			Vertex& v = m.vertices[i];
			positions[i] = v.position;
			normals[i] = v.normalModel;
			texcoords[i] = v.texcoord;
		}

		ComputeTangentFrame(m.indices.data(), m.indices.size() / 3,
							positions.data(), normals.data(), texcoords.data(),
							m.vertices.size(), tangents.data(), bitangents.data());

		for (size_t i = 0; i < m.vertices.size(); i++)
		{
			m.vertices[i].tangentModel = tangents[i];
		}
	}
}
