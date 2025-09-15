#include "GraphicsCommon.h"

using namespace std;
using namespace Microsoft::WRL;

namespace Graphics {
	// Samplers
	ComPtr<ID3D11SamplerState> linearWrapSS;
	ComPtr<ID3D11SamplerState> linearClampSS;
	ComPtr<ID3D11SamplerState> shadowPointSS;
	ComPtr<ID3D11SamplerState> shadowCompareSS;
	vector<ID3D11SamplerState*> sampleStates;

	// Rasterizer States
	ComPtr<ID3D11RasterizerState> solidRS;
	ComPtr<ID3D11RasterizerState> solidCCWRS; // Counter-ClockWise
	ComPtr<ID3D11RasterizerState> wireRS;
	ComPtr<ID3D11RasterizerState> wireCCWRS;
	ComPtr<ID3D11RasterizerState> postProcessingRS;

	// Depth Stencil States
	ComPtr<ID3D11DepthStencilState> drawDSS; // �Ϲ������� �׸���
	ComPtr<ID3D11DepthStencilState> maskDSS; // Stencil Buffer�� 1 ǥ��
	ComPtr<ID3D11DepthStencilState> drawMaskedDSS; // Stencil Buffer�� ǥ�õ� ���� �׸���

	// Shaders
	ComPtr<ID3D11VertexShader> basicVS;
	ComPtr<ID3D11VertexShader> skyboxVS;
	ComPtr<ID3D11VertexShader> samplingVS;
	ComPtr<ID3D11VertexShader> normalVS;
	ComPtr<ID3D11VertexShader> depthOnlyVS;

	ComPtr<ID3D11GeometryShader> normalGS;

	ComPtr<ID3D11PixelShader> basicPS;
	ComPtr<ID3D11PixelShader> skyboxPS;
	ComPtr<ID3D11PixelShader> combinePS;
	ComPtr<ID3D11PixelShader> bloomDownPS;
	ComPtr<ID3D11PixelShader> bloomUpPS;
	ComPtr<ID3D11PixelShader> normalPS;
	ComPtr<ID3D11PixelShader> depthOnlyPS;
	ComPtr<ID3D11PixelShader> postEffectsyPS;

	// Input Layouts
	ComPtr<ID3D11InputLayout> basicIL;
	ComPtr<ID3D11InputLayout> samplingIL;
	ComPtr<ID3D11InputLayout> skyboxIL;
	ComPtr<ID3D11InputLayout> postProcessingIL;

	// Blend States
	ComPtr<ID3D11BlendState> mirrorBS;

	// Grphics Pipeline States
	GraphicsPSO defaultSolidPSO;
	GraphicsPSO defaultWirePSO;
	GraphicsPSO stencilMaskPSO;
	GraphicsPSO reflectSolidPSO;
	GraphicsPSO reflectWirePSO;
	GraphicsPSO mirrorBlendSolidPSO;
	GraphicsPSO mirrorBlendWirePSO;
	GraphicsPSO skyboxSolidPSO;
	GraphicsPSO skyboxWirePSO;
	GraphicsPSO reflectSkyboxSolidPSO;
	GraphicsPSO reflectSkyboxWirePSO;
	GraphicsPSO normalsPSO;
	GraphicsPSO depthOnlyPSO;
	GraphicsPSO postEffectsPSO;
	GraphicsPSO postProcessingPSO;
}

void Graphics::InitCommonStates(ComPtr<ID3D11Device>& device)
{
	InitSamplers(device);
	InitRasterizerStates(device);
	InitDepthStencilStates(device);
	InitShaders(device);
	InitBlendStates(device);
	InitPipelineStates(device);
}

void Graphics::InitSamplers(ComPtr<ID3D11Device>& device)
{
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&sampDesc, linearWrapSS.GetAddressOf());
	
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	device->CreateSamplerState(&sampDesc, linearClampSS.GetAddressOf());

	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.BorderColor[0] = 1.0f; // ū z��
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	device->CreateSamplerState(&sampDesc, shadowPointSS.GetAddressOf());

	sampDesc.BorderColor[0] = 100.0f;
	sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT; // COMPARISON
	sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	device->CreateSamplerState(&sampDesc, shadowCompareSS.GetAddressOf());

	// Sampler ������ Common.hlsli�� �Ȱ��ƾ� ��
	sampleStates.push_back(linearWrapSS.Get());
	sampleStates.push_back(linearClampSS.Get());
	sampleStates.push_back(shadowPointSS.Get());
	sampleStates.push_back(shadowCompareSS.Get());
}

void Graphics::InitRasterizerStates(ComPtr<ID3D11Device>& device)
{
	D3D11_RASTERIZER_DESC rastDesc;
	ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
	rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	rastDesc.FrontCounterClockwise = false;
	rastDesc.DepthClipEnable = true;
	rastDesc.MultisampleEnable = true;
	ThrowIfFailed(device->CreateRasterizerState(&rastDesc, solidRS.GetAddressOf()));

	// �ſ￡ �ݻ���� �� => CCW
	rastDesc.FrontCounterClockwise = true;
	ThrowIfFailed(device->CreateRasterizerState(&rastDesc, solidCCWRS.GetAddressOf()));

	// Wire, CCW
	rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
	ThrowIfFailed(device->CreateRasterizerState(&rastDesc, wireCCWRS.GetAddressOf()));

	// Wire, CW
	rastDesc.FrontCounterClockwise = false;
	ThrowIfFailed(device->CreateRasterizerState(&rastDesc, wireRS.GetAddressOf()));

	ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
	rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
	rastDesc.FrontCounterClockwise = false;
	rastDesc.DepthClipEnable = false;
	ThrowIfFailed(device->CreateRasterizerState(&rastDesc, postProcessingRS.GetAddressOf()));
}

void Graphics::InitDepthStencilStates(ComPtr<ID3D11Device>& device)
{
	// drawDSS: Default
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	ZeroMemory(&dsDesc, sizeof(dsDesc));
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	// �ո鿡 ���� ����
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS; // Stencil �� Func => �׻� Pass
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP; // Stencil, Depth �� �� Fail�� ��� => ���� Stencil�� ����
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP; // Stencil�� Pass, Depth�� Fail�� ��� => ���� Stencil�� ����
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP; // Stencil, Depth �� �� Pass�� ��� => ���� Stencil�� ����
	// �޸�(�׸��ٸ�)�� ���� ����
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE; // Stencil, Depth �� �� Pass�� ��� => Stencil���� StencilRef�� ��ü
	ThrowIfFailed(device->CreateDepthStencilState(&dsDesc, drawDSS.GetAddressOf()));

	// maskDSS: Stencil���� 1�� ǥ�����ִ� DSS
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = true; // Stencil �ʼ�
	dsDesc.StencilReadMask = 0xFF; // ��� ��Ʈ �� ���
	dsDesc.StencilWriteMask = 0xFF;
	// �ո鿡 ���� ����
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;	// Stencil �� Func => �׻� Pass
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP; // Stencil, Depth �� �� Fail�� ��� => ���� Stencil�� ����
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP; // Stencil�� Pass, Depth�� Fail�� ��� => ���� Stencil�� ����
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE; // Stencil, Depth �� �� Pass�� ��� => Stencil���� StencilRef�� ��ü
	ThrowIfFailed(device->CreateDepthStencilState(&dsDesc, maskDSS.GetAddressOf()));

	// drawMaskDSS: Stencil���� 1�� ���� �׸��� DSS
	dsDesc.DepthEnable = true;
	dsDesc.StencilEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	// �ո鿡 ���� ����
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;	// Stencil �� Func => ���� ���� Pass
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP; // Stencil, Depth �� �� Fail�� ��� => ���� Stencil�� ����
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP; // Stencil�� Pass, Depth�� Fail�� ��� => ���� Stencil�� ����
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP; // Stencil, Depth �� �� Pass�� ��� => ���� Stencil�� ����
	ThrowIfFailed(device->CreateDepthStencilState(&dsDesc, drawMaskedDSS.GetAddressOf()));
}

void Graphics::InitShaders(ComPtr<ID3D11Device>& device)
{
	// Input Layouts
	vector<D3D11_INPUT_ELEMENT_DESC> basicIE{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	vector<D3D11_INPUT_ELEMENT_DESC> samplingIE{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	vector<D3D11_INPUT_ELEMENT_DESC> skyboxIE{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	// Shaders
	D3D11Utils::CreateVertexShaderAndInputLayout(device, L"BasicVS.hlsl",
												 basicIE, basicVS, basicIL);
	D3D11Utils::CreateVertexShaderAndInputLayout(device, L"NormalVS.hlsl",
												 basicIE, normalVS, basicIL);
	D3D11Utils::CreateVertexShaderAndInputLayout(device, L"SamplingVS.hlsl",
												 samplingIE, samplingVS, samplingIL);
	D3D11Utils::CreateVertexShaderAndInputLayout(device, L"SkyboxVS.hlsl",
												 skyboxIE, skyboxVS, skyboxIL);
	D3D11Utils::CreateVertexShaderAndInputLayout(device, L"DepthOnlyVS.hlsl",
												 basicIE, depthOnlyVS, skyboxIL);

	D3D11Utils::CreateGeometryShader(device, L"NormalGS.hlsl", normalGS);

	D3D11Utils::CreatePixelShader(device, L"BasicPS.hlsl", basicPS);
	D3D11Utils::CreatePixelShader(device, L"NormalPS.hlsl", normalPS);
	D3D11Utils::CreatePixelShader(device, L"SkyboxPS.hlsl", skyboxPS);
	D3D11Utils::CreatePixelShader(device, L"CombinePS.hlsl", combinePS);
	D3D11Utils::CreatePixelShader(device, L"BloomDownPS.hlsl", bloomDownPS);
	D3D11Utils::CreatePixelShader(device, L"BloomUpPS.hlsl", bloomUpPS);
	D3D11Utils::CreatePixelShader(device, L"DepthOnlyPS.hlsl", depthOnlyPS);
	D3D11Utils::CreatePixelShader(device, L"PostEffectsPS.hlsl", postEffectsyPS);
}

void Graphics::InitBlendStates(ComPtr<ID3D11Device>& device)
{
	D3D11_BLEND_DESC mirrorBlendDesc;
	ZeroMemory(&mirrorBlendDesc, sizeof(mirrorBlendDesc));
	mirrorBlendDesc.AlphaToCoverageEnable = true; // MSAA
	mirrorBlendDesc.IndependentBlendEnable = false;
	// ���� RenderTaget�� ���ؼ� ���� (�ִ� 8��)
	mirrorBlendDesc.RenderTarget[0].BlendEnable = true;
	mirrorBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_BLEND_FACTOR;
	mirrorBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_BLEND_FACTOR;
	mirrorBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	mirrorBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	mirrorBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	mirrorBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	mirrorBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	ThrowIfFailed(device->CreateBlendState(&mirrorBlendDesc, mirrorBS.GetAddressOf()));
}

void Graphics::InitPipelineStates(ComPtr<ID3D11Device>& device)
{
	// defaultPSO
	defaultSolidPSO.m_inputLayout = basicIL;
	defaultSolidPSO.m_vertexShader = basicVS;
	defaultSolidPSO.m_rasterizerState = solidRS;
	defaultSolidPSO.m_pixelShader = basicPS;
	defaultSolidPSO.m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// defaultWirePSO
	defaultWirePSO = defaultSolidPSO;
	defaultWirePSO.m_rasterizerState = wireRS;

	// stencilMaskPSO
	stencilMaskPSO = defaultSolidPSO;
	stencilMaskPSO.m_depthStencilState = maskDSS;
	stencilMaskPSO.m_stencilRef = 1;
	stencilMaskPSO.m_vertexShader = depthOnlyVS;
	stencilMaskPSO.m_pixelShader = depthOnlyPS;

	// reflectSolidPSO
	reflectSolidPSO = defaultSolidPSO;
	reflectSolidPSO.m_depthStencilState = drawMaskedDSS;
	reflectSolidPSO.m_rasterizerState = solidCCWRS; // Counter-ClockWise
	reflectSolidPSO.m_stencilRef = 1;

	// reflectWirePSO
	reflectWirePSO = reflectSolidPSO;
	reflectWirePSO.m_rasterizerState = wireCCWRS;
	reflectWirePSO.m_stencilRef = 1;

	// mirrorBlendSolidPSO
	mirrorBlendSolidPSO = defaultSolidPSO;
	mirrorBlendSolidPSO.m_blendState = mirrorBS;
	mirrorBlendSolidPSO.m_depthStencilState = drawMaskedDSS;
	mirrorBlendSolidPSO.m_stencilRef = 1;

	// mirrorBlendWirePSO
	mirrorBlendWirePSO = defaultWirePSO;
	mirrorBlendWirePSO.m_blendState = mirrorBS;
	mirrorBlendWirePSO.m_depthStencilState = drawMaskedDSS;
	mirrorBlendWirePSO.m_stencilRef = 1;

	// skyboxSolidPSO
	skyboxSolidPSO = defaultSolidPSO;
	skyboxSolidPSO.m_inputLayout = skyboxIL;
	skyboxSolidPSO.m_vertexShader = skyboxVS;
	skyboxSolidPSO.m_pixelShader = skyboxPS;

	// skyboxWirePS
	skyboxWirePSO = skyboxSolidPSO;
	skyboxWirePSO.m_rasterizerState = wireRS;

	// reflectSkyboxSolidPSO
	reflectSkyboxSolidPSO = skyboxSolidPSO;
	reflectSkyboxSolidPSO.m_depthStencilState = drawMaskedDSS;
	reflectSkyboxSolidPSO.m_rasterizerState = solidCCWRS;
	reflectSkyboxSolidPSO.m_stencilRef = 1;

	// reflectSkyboxWirePSO
	reflectSkyboxWirePSO = reflectSkyboxSolidPSO;
	reflectSkyboxWirePSO.m_rasterizerState = wireCCWRS;
	reflectSkyboxWirePSO.m_stencilRef = 1;

	// normalsPSO
	normalsPSO = defaultSolidPSO;
	normalsPSO.m_vertexShader = normalVS;
	normalsPSO.m_geometryShader = normalGS;
	normalsPSO.m_pixelShader = normalPS;
	normalsPSO.m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

	// depthOnlyPSO
	depthOnlyPSO = defaultSolidPSO;
	depthOnlyPSO.m_vertexShader = depthOnlyVS;
	depthOnlyPSO.m_pixelShader = depthOnlyPS;

	// postEffectsPSO
	postEffectsPSO.m_inputLayout = samplingIL;
	postEffectsPSO.m_vertexShader = samplingVS;
	postEffectsPSO.m_rasterizerState = postProcessingRS;
	postEffectsPSO.m_pixelShader = postEffectsyPS;

	// postProcessingPSO
	postProcessingPSO.m_inputLayout = samplingIL;
	postProcessingPSO.m_vertexShader = samplingVS;
	postProcessingPSO.m_rasterizerState = postProcessingRS;
	postProcessingPSO.m_pixelShader = depthOnlyPS; // dummy;
}
