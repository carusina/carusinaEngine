#pragma once

#include "ImageFilter.h"

class PostProcess {
public:
	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device> &device,
				   Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context,
				   const std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> &srvs,
				   const std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> &rtvs,
				   const int width, const int height, const int bloomLevels);

	void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context);

	void RenderImageFilter(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context,
						   const ImageFilter &imageFilter);

	void CreateBuffer(Microsoft::WRL::ComPtr<ID3D11Device> &device,
					  Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context,
					  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> &srv,
					  Microsoft::WRL::ComPtr<ID3D11RenderTargetView> &rtv,
					  const int width, const int height);

public:
	ImageFilter m_combineFilter;
	std::vector<ImageFilter> m_bloomDownFilters;
	std::vector<ImageFilter> m_bloomUpFilters;

	std::shared_ptr<Mesh> m_mesh;

private:
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_bloomSRVs;
	std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> m_bloomRTVs;
};