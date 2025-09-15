#pragma once

#include "D3D11Utils.h"
#include "GeometryGenerator.h"
#include "Mesh.h"

class ImageFilter {
public:
	ImageFilter() {};
	
	ImageFilter(Microsoft::WRL::ComPtr<ID3D11Device> &device,
				Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context,
				Microsoft::WRL::ComPtr<ID3D11PixelShader> &pixelShader,
				int width, int height);

	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device> &device,
				  Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context,
				  Microsoft::WRL::ComPtr<ID3D11PixelShader> &pixelShader,
				  int width, int height);

	void UpdateConstantBuffers(Microsoft::WRL::ComPtr<ID3D11Device> &device,
							   Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context);

	void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context) const;

	void SetShaderResources(const std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> &srvs);

	void SetRenderTarget(const std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> &rtvs);

public:
	struct ImageFilterConstData {
		float dx;
		float dy;
		float threshold;
		float strength;
		float option1; // exposure in CombinePS.hlsl
		float option2; // gamma in CombinePS.hlsl;
		float option3;
		float option4;
	};

	ImageFilterConstData m_constData = {};

protected:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constBuffer;
	D3D11_VIEWPORT m_viewport = {};

	std::vector<ID3D11ShaderResourceView*> m_SRVs;
	std::vector<ID3D11RenderTargetView*> m_RTVs;
};