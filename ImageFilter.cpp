#include "ImageFilter.h"

ImageFilter::ImageFilter(Microsoft::WRL::ComPtr<ID3D11Device>& device,
						 Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
						 Microsoft::WRL::ComPtr<ID3D11PixelShader>& pixelShader,
						 int width, int height)
{
	Initialize(device, context, pixelShader, width, height);
}

void ImageFilter::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device,
							Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
							Microsoft::WRL::ComPtr<ID3D11PixelShader>& pixelShader,
							int width, int height)
{
	ThrowIfFailed(pixelShader.CopyTo(m_pixelShader.GetAddressOf()));

	ZeroMemory(&m_viewport, sizeof(D3D11_VIEWPORT));
	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;
	m_viewport.Width = float(width);
	m_viewport.Height = float(height);
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;

	m_constData.dx = 1.0f / width;
	m_constData.dy = 1.0f / height;

	D3D11Utils::CreateConstBuffer(device, m_constData, m_constBuffer);
}

void ImageFilter::UpdateConstantBuffers(Microsoft::WRL::ComPtr<ID3D11Device>& device,
										Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
{
	D3D11Utils::UpdateBuffer(device, context, m_constData, m_constBuffer);
}

void ImageFilter::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context) const
{
	assert(m_SRVs.size() > 0);
	assert(m_RTVs.size() > 0);

	context->RSSetViewports(1, &m_viewport);

	context->OMSetRenderTargets(UINT(m_RTVs.size()), m_RTVs.data(), NULL); // 순서 주의

	context->PSSetShader(m_pixelShader.Get(), 0, 0);
	context->PSSetShaderResources(0, UINT(m_SRVs.size()), m_SRVs.data());
	context->PSSetConstantBuffers(0, 1, m_constBuffer.GetAddressOf());
}

void ImageFilter::SetShaderResources(const std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>& srvs)
{
	m_SRVs.clear();
	for (const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv : srvs)
	{
		m_SRVs.push_back(srv.Get());
	}
}

void ImageFilter::SetRenderTarget(const std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>>& rtvs)
{
	m_RTVs.clear();
	for (const Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv : rtvs)
	{
		m_RTVs.push_back(rtv.Get());
	}
}