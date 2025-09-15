#pragma once

#include <directxtk/SimpleMath.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include <iostream>
#include <vector>
#include <algorithm>

#include "Camera.h"
#include "ConstantBuffers.h"
#include "D3D11Utils.h"
#include "GraphicsPSO.h"
#include "PostProcess.h"
#include "GraphicsCommon.h"

class AppBase {
public:
	AppBase();
	virtual ~AppBase();

	int Run();
	float GetAspectRatio() const;

	virtual bool Initialize();
	virtual void UpdateGUI() = 0;
	virtual void Update(float dt) = 0;
	virtual void Render() = 0;
	virtual void OnMouseMove(int mouseX, int mouseY);
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void InitCupemaps(std::wstring basePath, std::wstring envFileName,
					  std::wstring specularFileName, std::wstring irradianceFileName,
					  std::wstring brdfFileName);
	void UpdateGlobalConstants(const DirectX::SimpleMath::Vector3 &eyeWorld,
							   const DirectX::SimpleMath::Matrix &viewRow,
							   const DirectX::SimpleMath::Matrix &projRow,
							   const DirectX::SimpleMath::Matrix &refl);
	void SetGlobalConsts(Microsoft::WRL::ComPtr<ID3D11Buffer> &globalConstsGPU);

	void CreateDepthBuffers();
	void SetPipelineState(const GraphicsPSO &pso);
	bool UpdateMouseControl(const DirectX::BoundingSphere& bs,
							DirectX::SimpleMath::Quaternion& q,
							DirectX::SimpleMath::Vector3& dragTranslation,
							DirectX::SimpleMath::Vector3& pickPoint);

protected:
	bool InitMainWindow();
	bool InitDirect3D();
	bool InitGUI();
	void CreateBuffers();
	void SetMainViewport();
	void SetShadowViewport();

public:
	int m_screenWidth; // 렌더링할 최종 화면 해상도
	int m_screenHeight;
	HWND m_mainWindow;
	bool m_useMSAA = true;
	UINT m_numQualityLevels = 0;
	bool m_drawAsWire = false;

	Microsoft::WRL::ComPtr<ID3D11Device> m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_backBufferRTV;

	// float(MSAA) -> resolved(Not MSAA) -> PostProcess -> backBuffer
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_floatBuffer;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_resolvedBuffer;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_postEffectsBuffer;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_floatRTV;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_resolvedRTV;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_postEffectsRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_resolvedSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_postEffectsSRV;

	// Depth Buffer 관련
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthOnlyBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthOnlyDSV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_depthOnlySRV;

	// Shadow maps
	int m_shadowWidth = 1280;
	int m_shadowHeight = 1280;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_shadowBuffers[MAX_LIGHTS];
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_shadowDSVs[MAX_LIGHTS];
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shadowSRVs[MAX_LIGHTS];

	D3D11_VIEWPORT m_screenViewport;

	// 시점 결정하는 Camera 추가
	Camera m_camera;
	bool m_keyPressed[256] = { false };
	bool m_leftButton = false;
	bool m_rightButton = false;
	bool m_dragStartFlag = false;

	// 커서 위치 (Picking)
	float m_cursorNdcX = 0.0f;
	float m_cursorNdcY = 0.0;

	// Rendering -> PostEffects -> PostProcess
	PostEffectsConstants m_postEffectsConstsCPU;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_postEffectsConstsGPU;

	PostProcess m_postProcess;

	// Const Buffer
	GlobalConstants m_globalConstsCPU;
	GlobalConstants m_reflectGlobalConstsCPU;
	GlobalConstants m_shadowGlobalConstsCPU[MAX_LIGHTS];
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_globalConstsGPU;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_reflectGlobalConstsGPU;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_shadowGlobalConstsGPU[MAX_LIGHTS];

	// Shader Resource View
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_envSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_irradianceSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_specularSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_brdfSRV;

	bool m_lightRotate = false;
};