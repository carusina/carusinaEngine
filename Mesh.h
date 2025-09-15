#pragma once

#include <DirectXMath.h>
#include <d3d11.h>
#include <Windows.h>
#include <wrl/client.h>

#include <iostream>
#include <vector>

struct Mesh {
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexConstBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pixelConstBuffer;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> albedoTexture;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> emissiveTexture;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> normalTexture;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> heightTexture;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> aoTexture;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> metallicRoughnessTexture;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> albedoSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> emissiveSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> heightSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> aoSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metallicRoughnessSRV;

	UINT indexCount = 0;
	UINT vertexCount = 0;
	UINT stride = 0;
	UINT offset = 0;
};