#pragma once

#include "ConstantBuffers.h"
#include "D3D11Utils.h"
#include "Mesh.h"
#include "MeshData.h"

class Model {
public:
	Model() {}
	Model(Microsoft::WRL::ComPtr<ID3D11Device> &device,
		  Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context,
		  const std::string &basePath, const std::string &fileName);
	Model(Microsoft::WRL::ComPtr<ID3D11Device> &device,
		  Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context,
		  const std::vector<MeshData> &meshes);

	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device> &device,
					Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context,
					const std::string &basePath, const std::string &fileName);

	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device> &device,
					Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context,
					const std::vector<MeshData> &meshes);

	void UpdateConstantBuffers(Microsoft::WRL::ComPtr<ID3D11Device> &device,
							   Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context);

	void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context);

	void RenderNormals(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context);

	void UpdateWorldRow(const DirectX::SimpleMath::Matrix &worldRow);

public:
	DirectX::SimpleMath::Matrix m_worldRow = DirectX::SimpleMath::Matrix(); // Model Space -> World Space
	DirectX::SimpleMath::Matrix m_worldITRow = DirectX::SimpleMath::Matrix();

	MeshConstants m_meshConstsCPU;
	MaterialConstants m_materialConstsCPU;

	bool m_drawNormals = false;
	bool m_isVisible = true;
	bool m_castShadow = true;

	std::vector<std::shared_ptr<Mesh>> m_meshes;

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_meshConstsGPU;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_materialConstsGPU;
};