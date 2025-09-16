#pragma once

#include "D3D11Utils.h"
#include "GraphicsPSO.h"

namespace Graphics {
	void InitCommonStates(Microsoft::WRL::ComPtr<ID3D11Device>& device);
	void InitSamplers(Microsoft::WRL::ComPtr<ID3D11Device>& device);
	void InitRasterizerStates(Microsoft::WRL::ComPtr<ID3D11Device>& device);
	void InitDepthStencilStates(Microsoft::WRL::ComPtr<ID3D11Device>& device);
	void InitShaders(Microsoft::WRL::ComPtr<ID3D11Device>& device);
	void InitBlendStates(Microsoft::WRL::ComPtr<ID3D11Device>& device);
	void InitPipelineStates(Microsoft::WRL::ComPtr<ID3D11Device>& device);

	// Samplers
	extern Microsoft::WRL::ComPtr<ID3D11SamplerState> linearWrapSS;
	extern Microsoft::WRL::ComPtr<ID3D11SamplerState> linearClampSS;
	extern Microsoft::WRL::ComPtr<ID3D11SamplerState> shadowPointSS;
	extern Microsoft::WRL::ComPtr<ID3D11SamplerState> shadowCompareSS;
	extern std::vector<ID3D11SamplerState*> sampleStates;

	// Rasterizer States
	extern Microsoft::WRL::ComPtr<ID3D11RasterizerState> solidRS;
	extern Microsoft::WRL::ComPtr<ID3D11RasterizerState> solidCCWRS; // Counter-ClockWise
	extern Microsoft::WRL::ComPtr<ID3D11RasterizerState> wireRS;
	extern Microsoft::WRL::ComPtr<ID3D11RasterizerState> wireCCWRS;
	extern Microsoft::WRL::ComPtr<ID3D11RasterizerState> postProcessingRS;

	// Depth Stencil States
	extern Microsoft::WRL::ComPtr<ID3D11DepthStencilState> drawDSS; // 일반적으로 그리기
	extern Microsoft::WRL::ComPtr<ID3D11DepthStencilState> maskDSS; // Stencil Buffer에 1 표시
	extern Microsoft::WRL::ComPtr<ID3D11DepthStencilState> drawMaskedDSS; // Stencil Buffer에 표시된 곳만 그리기

	// Shaders
	extern Microsoft::WRL::ComPtr<ID3D11VertexShader> basicVS;
	extern Microsoft::WRL::ComPtr<ID3D11VertexShader> skyboxVS;
	extern Microsoft::WRL::ComPtr<ID3D11VertexShader> samplingVS;
	extern Microsoft::WRL::ComPtr<ID3D11VertexShader> normalVS;
	extern Microsoft::WRL::ComPtr<ID3D11VertexShader> depthOnlyVS;

	extern Microsoft::WRL::ComPtr<ID3D11GeometryShader> normalGS;

	extern Microsoft::WRL::ComPtr<ID3D11PixelShader> basicPS;
	extern Microsoft::WRL::ComPtr<ID3D11PixelShader> skyboxPS;
	extern Microsoft::WRL::ComPtr<ID3D11PixelShader> combinePS;
	extern Microsoft::WRL::ComPtr<ID3D11PixelShader> bloomDownPS;
	extern Microsoft::WRL::ComPtr<ID3D11PixelShader> bloomUpPS;
	extern Microsoft::WRL::ComPtr<ID3D11PixelShader> normalPS;
	extern Microsoft::WRL::ComPtr<ID3D11PixelShader> depthOnlyPS;
	extern Microsoft::WRL::ComPtr<ID3D11PixelShader> postEffectsPS;

	// Input Layouts
	extern Microsoft::WRL::ComPtr<ID3D11InputLayout> basicIL;
	extern Microsoft::WRL::ComPtr<ID3D11InputLayout> samplingIL;
	extern Microsoft::WRL::ComPtr<ID3D11InputLayout> skyboxIL;
	extern Microsoft::WRL::ComPtr<ID3D11InputLayout> postProcessingIL;

	// Blend States
	extern Microsoft::WRL::ComPtr<ID3D11BlendState> mirrorBS;

	// Grphics Pipeline States
	extern GraphicsPSO defaultSolidPSO;
	extern GraphicsPSO defaultWirePSO;
	extern GraphicsPSO stencilMaskPSO;
	extern GraphicsPSO reflectSolidPSO;
	extern GraphicsPSO reflectWirePSO;
	extern GraphicsPSO mirrorBlendSolidPSO;
	extern GraphicsPSO mirrorBlendWirePSO;
	extern GraphicsPSO skyboxSolidPSO;
	extern GraphicsPSO skyboxWirePSO;
	extern GraphicsPSO reflectSkyboxSolidPSO;
	extern GraphicsPSO reflectSkyboxWirePSO;
	extern GraphicsPSO normalsPSO;
	extern GraphicsPSO depthOnlyPSO;
	extern GraphicsPSO postEffectsPSO;
	extern GraphicsPSO postProcessingPSO;
}