#include "PostProcess.h"
#include "GraphicsCommon.h"

using namespace std;
using namespace Microsoft::WRL;

void PostProcess::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device,
							Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
							const std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>& srvs,
							const std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>>& rtvs,
							const int width, const int height, const int bloomLevels)
{
	MeshData meshData = GeometryGenerator::MakeSquare();

	m_mesh = make_shared<Mesh>();
	D3D11Utils::CreateVertexBuffer(device, meshData.vertices, m_mesh->vertexBuffer);
	m_mesh->indexCount = UINT(meshData.indices.size());
	D3D11Utils::CreateIndexBuffer(device, meshData.indices, m_mesh->indexBuffer);

	// Bloom Donw
	m_bloomSRVs.resize(bloomLevels);
	m_bloomRTVs.resize(bloomLevels);
	for (int i = 0; i < bloomLevels; i++)
	{
		int div = int(pow(2, i));
		CreateBuffer(device, context, m_bloomSRVs[i], m_bloomRTVs[i],
					 width / div, height / div);
	}

	m_bloomDownFilters.resize(bloomLevels - 1);
	for (int i = 0; i < bloomLevels - 1; i++)
	{
		int div = int(pow(2, i + 1));
		m_bloomDownFilters[i].Initialize(device, context, Graphics::bloomDownPS,
										width / div, height / div);

		if (i == 0)
		{
			m_bloomDownFilters[i].SetShaderResources({ srvs[0] });
		}
		else
		{
			m_bloomDownFilters[i].SetShaderResources({ m_bloomSRVs[i] });
		}

		m_bloomDownFilters[i].SetRenderTarget({ m_bloomRTVs[i + 1] });
	}

	// Bloom Up
	m_bloomUpFilters.resize(bloomLevels - 1);
	for (int i = 0; i < bloomLevels - 1; i++)
	{
		int level = bloomLevels - 2 - i;
		int div = int(pow(2, level));
		m_bloomUpFilters[i].Initialize(device, context, Graphics::bloomUpPS,
									  width / div, height / div);
		m_bloomUpFilters[i].SetShaderResources({ m_bloomSRVs[level + 1] });
		m_bloomUpFilters[i].SetRenderTarget({ m_bloomRTVs[level] });
	}

	// Combine + ToneMapping
	m_combineFilter.Initialize(device, context, Graphics::combinePS, width, height);
	m_combineFilter.SetShaderResources({ srvs[0], m_bloomSRVs[0] });
	m_combineFilter.SetRenderTarget(rtvs);
	m_combineFilter.m_constData.strength = 0.0f; // Bloom Strength
	m_combineFilter.m_constData.option1 = 1.0f; // Exposure로 사용
	m_combineFilter.m_constData.option2 = 2.2f; // Gamma로 사용
	m_combineFilter.UpdateConstantBuffers(device, context);
}

void PostProcess::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
{
	context->PSSetSamplers(0, 1, Graphics::linearClampSS.GetAddressOf());

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, m_mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(m_mesh->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// Bloom이 필요한 경우만
	if (m_combineFilter.m_constData.strength > 0.0f)
	{
		for (int i = 0; i < m_bloomDownFilters.size(); i++)
		{
			RenderImageFilter(context, m_bloomDownFilters[i]);
		}
		for (int i = 0; i < m_bloomUpFilters.size(); i++)
		{
			RenderImageFilter(context, m_bloomUpFilters[i]);
		}
	}

	RenderImageFilter(context, m_combineFilter);
}

void PostProcess::RenderImageFilter(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
									const ImageFilter& imageFilter)
{
	imageFilter.Render(context);
	context->DrawIndexed(m_mesh->indexCount, 0, 0);
}

void PostProcess::CreateBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& device,
							   Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
							   Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv,
							   Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& rtv,
							   const int width, const int height)
{
	ComPtr<ID3D11Texture2D> texture;

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texDesc.MiscFlags = 0;
	texDesc.CPUAccessFlags = 0;

	ThrowIfFailed(device->CreateTexture2D(&texDesc, NULL, texture.GetAddressOf()));
	ThrowIfFailed(device->CreateRenderTargetView(texture.Get(), NULL, rtv.GetAddressOf()));
	ThrowIfFailed(device->CreateShaderResourceView(texture.Get(), NULL, srv.GetAddressOf()));
}
