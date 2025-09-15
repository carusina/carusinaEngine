#include "Model.h"
#include "GeometryGenerator.h"

using namespace std;
using namespace DirectX::SimpleMath;

Model::Model(Microsoft::WRL::ComPtr<ID3D11Device>& device,
			 Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
			 const std::string& basePath, const std::string& fileName)
{
	this->Initialize(device, context, basePath, fileName);
}

Model::Model(Microsoft::WRL::ComPtr<ID3D11Device>& device,
			 Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
			 const std::vector<MeshData>& meshes)
{
	this->Initialize(device, context, meshes);
}

void Model::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device,
					   Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
					   const std::string& basePath, const std::string& fileName)
{
	vector<MeshData> meshes = GeometryGenerator::ReadFromFile(basePath, fileName);

	Initialize(device, context, meshes);
}

void Model::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device,
					   Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
					   const std::vector<MeshData>& meshes)
{
	// Create ConstantBuffer
	m_meshConstsCPU.world = Matrix();

	D3D11Utils::CreateConstBuffer(device, m_meshConstsCPU, m_meshConstsGPU);
	D3D11Utils::CreateConstBuffer(device, m_materialConstsCPU, m_materialConstsGPU);

	for (const MeshData& meshData : meshes)
	{
		shared_ptr<Mesh> newMesh = make_shared<Mesh>();
		D3D11Utils::CreateVertexBuffer(device, meshData.vertices, newMesh->vertexBuffer);
		newMesh->indexCount = UINT(meshData.indices.size());
		newMesh->vertexCount = UINT(meshData.vertices.size());
		newMesh->stride = UINT(sizeof(Vertex));
		D3D11Utils::CreateIndexBuffer(device, meshData.indices, newMesh->indexBuffer);

		if (!meshData.albedoTextureFileName.empty())
		{
			D3D11Utils::CreateTexture(device, context, meshData.albedoTextureFileName, true,
									  newMesh->albedoTexture, newMesh->albedoSRV);
			m_materialConstsCPU.useAlbedoMap = true;
		}
		if (!meshData.emissiveTextureFileName.empty())
		{
			D3D11Utils::CreateTexture(device, context, meshData.emissiveTextureFileName, true,
									  newMesh->emissiveTexture, newMesh->emissiveSRV);
			m_materialConstsCPU.useEmissiveMap = true;
		}
		if (!meshData.normalTextureFileName.empty())
		{
			D3D11Utils::CreateTexture(device, context, meshData.normalTextureFileName, false,
									  newMesh->normalTexture, newMesh->normalSRV);
			m_materialConstsCPU.useNormalMap = true;
		}
		if (!meshData.heightTextureFileName.empty())
		{
			D3D11Utils::CreateTexture(device, context, meshData.heightTextureFileName, false,
									  newMesh->heightTexture, newMesh->heightSRV);
			m_meshConstsCPU.useHeightMap = true;
		}
		if (!meshData.aoTextureFileName.empty())
		{
			D3D11Utils::CreateTexture(device, context, meshData.aoTextureFileName, false,
									  newMesh->aoTexture, newMesh->aoSRV);
			m_materialConstsCPU.useAoMap = true;
		}

		// GLTF 방식으로 Metallic과 Roughness를 하나의 texture에 넣음
		// Green: Roughness, Blue: Metallic
		if (!meshData.metallicTextureFileName.empty() || !meshData.roughnessTextureFileName.empty())
		{
			D3D11Utils::CreateMetallicRoughnessTexture(device, context,
													   meshData.metallicTextureFileName,
													   meshData.roughnessTextureFileName,
													   newMesh->metallicRoughnessTexture,
													   newMesh->metallicRoughnessSRV);
		}
		if (!meshData.metallicTextureFileName.empty())
		{
			m_materialConstsCPU.useMetallicMap = true;
		}
		if (!meshData.roughnessTextureFileName.empty())
		{
			m_materialConstsCPU.useRoughnessMap = true;
		}

		newMesh->vertexConstBuffer = m_meshConstsGPU;
		newMesh->pixelConstBuffer = m_materialConstsGPU;

		this->m_meshes.push_back(newMesh);
	}
}

void Model::UpdateConstantBuffers(Microsoft::WRL::ComPtr<ID3D11Device>& device,
								  Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
{
	if (m_isVisible)
	{
		D3D11Utils::UpdateBuffer(device, context, m_meshConstsCPU, m_meshConstsGPU);
		D3D11Utils::UpdateBuffer(device, context, m_materialConstsCPU, m_materialConstsGPU);
	}
}

void Model::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
{
	if (m_isVisible)
	{
		for (const shared_ptr<Mesh>& mesh : m_meshes)
		{
			context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &mesh->stride, &mesh->offset);
			context->IASetIndexBuffer(mesh->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

			context->VSSetShaderResources(0, 1, mesh->heightSRV.GetAddressOf());
			context->VSSetConstantBuffers(0, 1, mesh->vertexConstBuffer.GetAddressOf());

			vector<ID3D11ShaderResourceView*> resViews = { mesh->albedoSRV.Get(), mesh->normalSRV.Get(),
														   mesh->aoSRV.Get(), mesh->metallicRoughnessSRV.Get(),
														   mesh->emissiveSRV.Get() };
			context->PSSetShaderResources(0, UINT(resViews.size()), resViews.data());
			context->PSSetConstantBuffers(0, 1, mesh->pixelConstBuffer.GetAddressOf());

			context->DrawIndexed(mesh->indexCount, 0, 0);
;		}
	}
}

void Model::RenderNormals(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
{
	for (const shared_ptr<Mesh>& mesh : m_meshes)
	{
		context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &mesh->stride, &mesh->offset);

		context->GSSetConstantBuffers(0, 1, m_meshConstsGPU.GetAddressOf());

		context->Draw(mesh->vertexCount, 0);
	}
}

void Model::UpdateWorldRow(const DirectX::SimpleMath::Matrix& worldRow)
{
	m_worldRow = worldRow;
 	m_worldITRow = worldRow;
	m_worldITRow.Translation(Vector3(0.0f));
	m_worldITRow = m_worldITRow.Invert().Transpose();

	m_meshConstsCPU.world = worldRow.Transpose();
	m_meshConstsCPU.worldIT = m_worldITRow.Transpose();
}