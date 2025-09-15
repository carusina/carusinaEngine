#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <windows.h>
#include <wrl/client.h> // ComPtr

inline void ThrowIfFailed(HRESULT hr) {
	if (FAILED(hr)) {
		throw std::exception();
	}
}

class D3D11Utils {
public:
	static void CreateVertexShaderAndInputLayout(
		Microsoft::WRL::ComPtr<ID3D11Device> &device, const std::wstring &fileName,
		const std::vector<D3D11_INPUT_ELEMENT_DESC> &inputElements,
		Microsoft::WRL::ComPtr<ID3D11VertexShader> &vertexShader,
		Microsoft::WRL::ComPtr<ID3D11InputLayout> &inputLayout);

	static void CreateHullShader(Microsoft::WRL::ComPtr<ID3D11Device> &device,
							     const std::wstring &fileName,
								 Microsoft::WRL::ComPtr<ID3D11HullShader> &hullShader);
	
	static void CreateDomainShader(Microsoft::WRL::ComPtr<ID3D11Device> &device,
								   const std::wstring &fileName,
								   Microsoft::WRL::ComPtr<ID3D11DomainShader> &domainShader);

	static void CreateGeometryShader(Microsoft::WRL::ComPtr<ID3D11Device> &device,
									 const std::wstring &fileName,
									 Microsoft::WRL::ComPtr<ID3D11GeometryShader> &geometryShader);

	static void CreatePixelShader(Microsoft::WRL::ComPtr<ID3D11Device> &device,
								  const std::wstring &fileName,
								  Microsoft::WRL::ComPtr<ID3D11PixelShader> &pixelShader);

	static void CreateIndexBuffer(Microsoft::WRL::ComPtr<ID3D11Device> &device,
								  const std::vector<uint32_t> &indices,
							      Microsoft::WRL::ComPtr<ID3D11Buffer> &indexBuffer);

	template <typename T_VERTEX>
	static void CreateVertexBuffer(Microsoft::WRL::ComPtr<ID3D11Device> &device,
								   const std::vector<T_VERTEX> &vertices,
								   Microsoft::WRL::ComPtr<ID3D11Buffer> &vertexBuffer)
	{	
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE; // 초기화 이후 변경 X, vertex는 변경 사항 없음
		bufferDesc.ByteWidth = UINT(sizeof(T_VERTEX) * vertices.size());
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0; // No CPU access is necessary
		bufferDesc.StructureByteStride = sizeof(T_VERTEX);

		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		vertexBufferData.pSysMem = vertices.data();
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;

		ThrowIfFailed(device->CreateBuffer(&bufferDesc, &vertexBufferData,
										   vertexBuffer.GetAddressOf()));
	}

	template <typename T_CONSTANT>
	static void CreateConstBuffer(Microsoft::WRL::ComPtr<ID3D11Device> &device,
								  const T_CONSTANT &constantBufferData,
								  Microsoft::WRL::ComPtr<ID3D11Buffer> &constantBuffer)
	{
		static_assert((sizeof(T_CONSTANT) % 16) == 0,
					  "Constant Buffer size must be 16-byte aligend.");

		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.ByteWidth = sizeof(constantBufferData);
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC; // need Update
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA initData = { 0 };
		initData.pSysMem = &constantBufferData;
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;

		ThrowIfFailed(device->CreateBuffer(&bufferDesc, &initData,
										   constantBuffer.GetAddressOf()));
	}

	template <typename T_DATA>
	static void UpdateBuffer(Microsoft::WRL::ComPtr<ID3D11Device> &device,
							 Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context,
							 const T_DATA &bufferData,
							 Microsoft::WRL::ComPtr<ID3D11Buffer> &buffer)
	{	
		if (!buffer) {
			std::cout << "UpdateBuffer() buffer was not initalized. \n";
		}

		D3D11_MAPPED_SUBRESOURCE ms;
		context->Map(buffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
		memcpy(ms.pData, &bufferData, sizeof(bufferData));
		context->Unmap(buffer.Get(), NULL);
	}

	static void CreateTexture(Microsoft::WRL::ComPtr<ID3D11Device> &device,
							  Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context,
							  const std::string fileName,
							  const bool useSRGB,
							  Microsoft::WRL::ComPtr<ID3D11Texture2D> &texture,
							  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> &srv);

	static void CreateMetallicRoughnessTexture(Microsoft::WRL::ComPtr<ID3D11Device> &device,
											   Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context,
											   const std::string metallicFimeName,
											   const std::string roughnessFileName,
											   Microsoft::WRL::ComPtr<ID3D11Texture2D> &texture,
											   Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> &srv);

	static void CreateTextureArray(Microsoft::WRL::ComPtr<ID3D11Device> &device,
								   Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context,
								   const std::vector<std::string> fileNames,
								   Microsoft::WRL::ComPtr<ID3D11Texture2D> &texture,
								   Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> &srv);

	static void CreateDDSTexture(Microsoft::WRL::ComPtr<ID3D11Device> &device,
								 const wchar_t *fileName,
								 const bool isCubeMap,
								 Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> &srv);

	static void WriteToFile(Microsoft::WRL::ComPtr<ID3D11Device> &device,
							Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context,
							Microsoft::WRL::ComPtr<ID3D11Texture2D> &textureToWrite,
							const std::string fileName);
};