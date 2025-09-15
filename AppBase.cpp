#include "AppBase.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace Microsoft::WRL;


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg,
                                                             WPARAM wParam, LPARAM lParam);

AppBase* g_appBase = nullptr;

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return g_appBase->MsgProc(hwnd, msg, wParam, lParam);
}

AppBase::AppBase() 
	: m_screenWidth(1280), m_screenHeight(720),
	  m_mainWindow(0), m_screenViewport(D3D11_VIEWPORT())
{
	g_appBase = this;

	m_camera.SetAspectRatio(this->GetAspectRatio());
}

AppBase::~AppBase()
{
	g_appBase = nullptr;

	// CleanUp
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	DestroyWindow(m_mainWindow);
}

int AppBase::Run()
{
	// Main message loop
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();

			ImGui::NewFrame();
			ImGui::Begin("Scene Control");

			// ImGui�� �������ִ� Framerate ���
			ImGui::Text("Average %.3f ms/frame (%.1f FPS)",
				1000.0f / ImGui::GetIO().Framerate,
				ImGui::GetIO().Framerate);

			UpdateGUI(); // �߰������� ����� GUI

			ImGui::End();
			ImGui::Render();

			Update(ImGui::GetIO().DeltaTime);

			Render(); // �츮�� ������ ������

			// GUI ������
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			// GUI ������ �Ŀ� Present() ȣ��
			m_swapChain->Present(1, 0);
		}
	}

	return 0;
}

float AppBase::GetAspectRatio() const
{
	return float(m_screenWidth) / m_screenHeight;
}

bool AppBase::Initialize()
{
	if (!InitMainWindow())
	{
		return false;
	}
	if (!InitDirect3D())
	{
		return false;
	}
	if (!InitGUI())
	{
		return false;
	}

	// �ܼ�â�� ������â�� ���� ���� ����
	SetForegroundWindow(m_mainWindow);

	return true;
}

void AppBase::OnMouseMove(int mouseX, int mouseY)
{
	// ���콺 Ŀ���� ��ġ�� NDC ��ǥ�� ��ȯ
	// Screen(���� �»��) [0, width - 1] x [0, height - 1]
	// => NDC(���� �����) [-1, 1] x [-1, 1]
	m_cursorNdcX = mouseX * 2.0f / m_screenWidth - 1.0f;
	m_cursorNdcY = -mouseY * 2.0f / m_screenHeight + 1.0f;

	// Ŀ���� ȭ�� ������ ������ ��� ó��
	m_cursorNdcX = clamp(m_cursorNdcX, -1.0f, 1.0f);
	m_cursorNdcY = clamp(m_cursorNdcY, -1.0f, 1.0f);

	// ī�޶� ���� ��ȯ
	m_camera.UpdateMouse(m_cursorNdcX, m_cursorNdcY);
}

LRESULT AppBase::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
	{
		return true;
	}

	switch (msg)
	{
	case WM_SIZE:
		// ȭ�� �ػ󵵰� �ٲ��, SwapChain�� �����
		if (m_swapChain)
		{
			m_screenWidth = int(LOWORD(lParam));
			m_screenHeight = int(HIWORD(lParam));

			m_backBufferRTV.Reset();
			m_swapChain->ResizeBuffers(0, // ���� ���� ����
									   UINT(LOWORD(lParam)), UINT(HIWORD(lParam)), // �ػ� ����
									   DXGI_FORMAT_UNKNOWN, // ���� ���� ����
									   0);

			CreateBuffers();
			SetMainViewport();
			m_camera.SetAspectRatio(this->GetAspectRatio());
		}
		break;

	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
		{
			return 0;
		}
		break;

	case WM_MOUSEMOVE:
		OnMouseMove(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_LBUTTONDOWN:
		if (!m_leftButton)
		{
			m_dragStartFlag = true; // �巡�׸� ���� �����ϴ��� Ȯ��
		}
		m_leftButton = true;
		break;

	case WM_LBUTTONUP:
		m_leftButton = false;
		break;

	case WM_RBUTTONDOWN:
		if (!m_rightButton)
		{
			m_dragStartFlag = true; // �巡�׸� ���� �����ϴ��� Ȯ��
		}
		m_rightButton = true;
		break;

	case WM_RBUTTONUP:
		m_rightButton = false;
		break;

	case WM_KEYDOWN:
		m_keyPressed[wParam] = true;
		if (wParam == VK_ESCAPE) // ESCŰ ����
		{
			DestroyWindow(hwnd);
		}
		if (wParam == VK_SPACE) // SpaceŰ ���� ������
		{
			m_lightRotate = !m_lightRotate;
		}
		break;

	case WM_KEYUP:
		if (wParam == 'F') // FŰ 1��Ī ���� ������
		{
			m_camera.m_useFirstPersonView = !m_camera.m_useFirstPersonView;
		}
		if (wParam == 'C') // CŰ ȭ�� ĸó
		{
			ComPtr<ID3D11Texture2D> backBuffer;
			m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
			D3D11Utils::WriteToFile(m_device, m_context, backBuffer, "captured.png");
		}
		m_keyPressed[wParam] = false;
		break;

	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}

	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

void AppBase::InitCupemaps(std::wstring basePath, std::wstring envFileName,
						   std::wstring specularFileName, std::wstring irradianceFileName,
						   std::wstring brdfFileName)
{
	// BRDF LUT�� 2D Texture
	D3D11Utils::CreateDDSTexture(m_device, (basePath + envFileName).c_str(), true, m_envSRV);
	D3D11Utils::CreateDDSTexture(m_device, (basePath + specularFileName).c_str(), true, m_specularSRV);
	D3D11Utils::CreateDDSTexture(m_device, (basePath + irradianceFileName).c_str(), true, m_irradianceSRV);
	D3D11Utils::CreateDDSTexture(m_device, (basePath + brdfFileName).c_str(), false, m_brdfSRV);
}

void AppBase::UpdateGlobalConstants(const DirectX::SimpleMath::Vector3& eyeWorld,
									const DirectX::SimpleMath::Matrix& viewRow,
									const DirectX::SimpleMath::Matrix& projRow,
									const DirectX::SimpleMath::Matrix& refl)
{
	m_globalConstsCPU.eyeWorld = eyeWorld;
	m_globalConstsCPU.view = viewRow.Transpose();
	m_globalConstsCPU.proj = projRow.Transpose();
	m_globalConstsCPU.invProj = projRow.Invert().Transpose();
	m_globalConstsCPU.viewProj = (viewRow * projRow).Transpose();
	m_globalConstsCPU.invViewProj = m_globalConstsCPU.viewProj.Invert(); // �׸��� �������� ���

	m_reflectGlobalConstsCPU = m_globalConstsCPU;
	memcpy(&m_reflectGlobalConstsCPU, &m_globalConstsCPU, sizeof(m_globalConstsCPU));
	m_reflectGlobalConstsCPU.view = (refl * viewRow).Transpose();
	m_reflectGlobalConstsCPU.viewProj = (refl * viewRow * projRow).Transpose();
	m_reflectGlobalConstsCPU.invViewProj = m_reflectGlobalConstsCPU.viewProj.Invert();

	D3D11Utils::UpdateBuffer(m_device, m_context, m_globalConstsCPU, m_globalConstsGPU);
	D3D11Utils::UpdateBuffer(m_device, m_context, m_reflectGlobalConstsCPU, m_reflectGlobalConstsGPU);
}

void AppBase::SetGlobalConsts(Microsoft::WRL::ComPtr<ID3D11Buffer>& globalConstsGPU)
{
	// ���̴��� �ϰ��� ���� register(b1)
	m_context->VSSetConstantBuffers(1, 1, globalConstsGPU.GetAddressOf());
	m_context->GSSetConstantBuffers(1, 1, globalConstsGPU.GetAddressOf());
	m_context->PSSetConstantBuffers(1, 1, globalConstsGPU.GetAddressOf());
}

void AppBase::CreateDepthBuffers()
{
	// DepthStencilView ����
	D3D11_TEXTURE2D_DESC texdesc;
	ZeroMemory(&texdesc, sizeof(texdesc));
	texdesc.Width = m_screenWidth;
	texdesc.Height = m_screenHeight;
	texdesc.MipLevels = 1;
	texdesc.ArraySize = 1;
	texdesc.Usage = D3D11_USAGE_DEFAULT;
	texdesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texdesc.CPUAccessFlags = 0;
	texdesc.MiscFlags = 0;
	if (m_useMSAA && m_numQualityLevels > 0)
	{
		texdesc.SampleDesc.Count = 4;
		texdesc.SampleDesc.Quality = 0;
	}
	else
	{
		texdesc.SampleDesc.Count = 1;
		texdesc.SampleDesc.Quality = 0;
	}
	texdesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // Depth 24Bit, Stencil 8Bit
	ComPtr<ID3D11Texture2D> depthStencilBuffer;
	ThrowIfFailed(m_device->CreateTexture2D(&texdesc, 0,
											depthStencilBuffer.GetAddressOf()));
	ThrowIfFailed(m_device->CreateDepthStencilView(depthStencilBuffer.Get(), NULL,
												   m_depthStencilView.GetAddressOf()));

	// DepthOnly
	texdesc.Format = DXGI_FORMAT_R32_TYPELESS;
	texdesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texdesc.SampleDesc.Count = 1;
	texdesc.SampleDesc.Quality = 0;
	ThrowIfFailed(m_device->CreateTexture2D(&texdesc, NULL,
										    m_depthOnlyBuffer.GetAddressOf()));

	// Shadow Buffer
	texdesc.Width = m_shadowWidth;
	texdesc.Height = m_shadowHeight;
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		ThrowIfFailed(m_device->CreateTexture2D(&texdesc, NULL,
												m_shadowBuffers[i].GetAddressOf()));
	}
	
	// DepthStencilView
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	ThrowIfFailed(m_device->CreateDepthStencilView(m_depthOnlyBuffer.Get(), &dsvDesc,
												   m_depthOnlyDSV.GetAddressOf()));

	// Shadow DSV
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		ThrowIfFailed(m_device->CreateDepthStencilView(m_shadowBuffers[i].Get(), &dsvDesc,
													   m_shadowDSVs[i].GetAddressOf()));
	}

	// ShaderResourceView
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	ThrowIfFailed(m_device->CreateShaderResourceView(m_depthOnlyBuffer.Get(), &srvDesc,
													 m_depthOnlySRV.GetAddressOf()));

	// Shadow SRV
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		ThrowIfFailed(m_device->CreateShaderResourceView(m_shadowBuffers[i].Get(), &srvDesc,
														 m_shadowSRVs[i].GetAddressOf()));
	}
}

void AppBase::SetPipelineState(const GraphicsPSO& pso)
{
	m_context->IASetInputLayout(pso.m_inputLayout.Get());
	m_context->IASetPrimitiveTopology(pso.m_primitiveTopology);

	m_context->VSSetShader(pso.m_vertexShader.Get(), 0, 0);

	m_context->HSSetShader(pso.m_hullShader.Get(), 0, 0);

	m_context->DSSetShader(pso.m_domainShader.Get(), 0, 0);

	m_context->GSSetShader(pso.m_geometryShader.Get(), 0, 0);

	m_context->RSSetState(pso.m_rasterizerState.Get());

	m_context->PSSetShader(pso.m_pixelShader.Get(), 0, 0);

	m_context->OMSetBlendState(pso.m_blendState.Get(), pso.m_blendFactor, 0xffffffff);
	m_context->OMSetDepthStencilState(pso.m_depthStencilState.Get(), pso.m_stencilRef);
}

bool AppBase::UpdateMouseControl(const DirectX::BoundingSphere& bs,
								 DirectX::SimpleMath::Quaternion& q,
								 DirectX::SimpleMath::Vector3& dragTranslation,
								 DirectX::SimpleMath::Vector3& pickPoint)
{
	const Matrix viewRow = m_camera.GetViewRow();
	const Matrix projRow = m_camera.GetProjRow();

	// mainSphere ȸ�� ����
	static float prevRatio = 0.0f;
	static Vector3 prevPos(0.0f);
	static Vector3 prevVector(0.0f);

	// ȸ���� �̵� �ʱ�ȭ
	q = Quaternion::CreateFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), 0.0f);
	dragTranslation = Vector3(0.0f);

	// ���콺 ��Ŭ���� ȸ��
	if (m_leftButton)
	{
		Vector3 cursorNdcNear = Vector3(m_cursorNdcX, m_cursorNdcY, 0.0f);
		Vector3 cursorNdcFar = Vector3(m_cursorNdcX, m_cursorNdcY, 1.0f);

		// NDC ��ǥ���� Ŀ���� World ��ǥ��� ����ȯ ���ִ� Matrix
		Matrix inverseProjView = (viewRow * projRow).Invert();

		// Picking Ray ����
		Vector3 cursorWorldNear = Vector3::Transform(cursorNdcNear, inverseProjView);
		Vector3 cursorWorldFar = Vector3::Transform(cursorNdcFar, inverseProjView);
		Vector3 dir = cursorWorldFar - cursorWorldNear;
		dir.Normalize();

		const Ray pickingRay = Ray(cursorWorldNear, dir);
		float dist = 0.0f;
		if (pickingRay.Intersects(bs, dist))
		{
			pickPoint = cursorWorldNear + dist * dir;

			if (m_dragStartFlag)
			{
				// �巡�� ����
				prevVector = pickPoint - bs.Center;
				prevVector.Normalize();
				m_dragStartFlag = false;
			}
			else
			{
				Vector3 currentVector = pickPoint - bs.Center;
				currentVector.Normalize();
				float theta = acos(prevVector.Dot(currentVector));
				if (theta > 3.141592f / 180.0f * 3.0f)
				{
					Vector3 axis = prevVector.Cross(currentVector);
					axis.Normalize();
					q = Quaternion::CreateFromAxisAngle(axis, theta);

					prevVector = currentVector;
				}
			}

			return true; // selected
		}
	}

	// ���콺 ��Ŭ���� �̵�
	if (m_rightButton)
	{
		Vector3 cursorNdcNear = Vector3(m_cursorNdcX, m_cursorNdcY, 0.0f);
		Vector3 cursorNdcFar = Vector3(m_cursorNdcX, m_cursorNdcY, 1.0f);

		// NDC ��ǥ���� Ŀ���� World ��ǥ��� ����ȯ ���ִ� Matrix
		Matrix inverseProjView = (viewRow * projRow).Invert();

		// Picking Ray�� ����
		Vector3 cursorWorldNear = Vector3::Transform(cursorNdcNear, inverseProjView);
		Vector3 cursorWorldFar = Vector3::Transform(cursorNdcFar, inverseProjView);
		Vector3 dir = cursorWorldFar - cursorWorldNear;
		dir.Normalize();

		const Ray pickingRay = Ray(cursorWorldNear, dir);
		float dist = 0.0f;
		if (pickingRay.Intersects(bs, dist)) {
			pickPoint = cursorWorldNear + dist * dir;

			if (m_dragStartFlag)
			{
				prevRatio = dist / (cursorWorldFar - cursorWorldNear).Length();
				prevPos = pickPoint;
				m_dragStartFlag = false;
			}
			else
			{
				Vector3 newPos = cursorWorldNear + prevRatio * (cursorWorldFar - cursorWorldNear);
				if ((newPos - prevPos).Length() > 1e-3) {
					dragTranslation = newPos - prevPos;
					prevPos = newPos;
				}
			}

			return true; // selected
		}
	}

	return false;
}

bool AppBase::InitMainWindow()
{
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc,
					  0L, 0L, GetModuleHandle(NULL),
					  NULL, NULL, NULL, NULL,
					  L"HongLabGraphics", // lpszClassName, L-string
					  NULL };

	if (!RegisterClassEx(&wc))
	{
		cout << "RegisterClassEx() failed.\n";
		return false;
	}

	RECT wr = { 0, 0, m_screenWidth, m_screenHeight };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, false);
	m_mainWindow = CreateWindow(wc.lpszClassName, L"carusina Engine",
								WS_OVERLAPPEDWINDOW,
								100, // ������ ���� ����� x ��ǥ
								100, // ������ ���� ����� y ��ǥ
								wr.right - wr.left, // ������ ���� ���� �ػ�
								wr.bottom - wr.top, // ������ ���� ���� �ػ�
								NULL, NULL, wc.hInstance, NULL);

	if (!m_mainWindow)
	{
		cout << "CreateWindow() failed.\n";
		return false;
	}

	ShowWindow(m_mainWindow, SW_SHOWDEFAULT);
	UpdateWindow(m_mainWindow);

	return true;
}

bool AppBase::InitDirect3D()
{
	const D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE;

	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	const D3D_FEATURE_LEVEL featureLevels[2] = { D3D_FEATURE_LEVEL_11_0,
												 D3D_FEATURE_LEVEL_9_3 };
	D3D_FEATURE_LEVEL featureLevel;

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferDesc.Width = m_screenWidth;
	sd.BufferDesc.Height = m_screenHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferCount = 2; // Double
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = m_mainWindow;
	sd.Windowed = TRUE;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	ThrowIfFailed(D3D11CreateDeviceAndSwapChain(0, driverType, 0, createDeviceFlags, featureLevels, 1,
												D3D11_SDK_VERSION, &sd, m_swapChain.GetAddressOf(), m_device.GetAddressOf(),
												&featureLevel, m_context.GetAddressOf()));

	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		cout << "D3D Feature Level 11 unsupported.\n";
		return false;
	}

	Graphics::InitCommonStates(m_device);
	CreateBuffers();
	SetMainViewport();

	// �������� ���� ConstsBuffer
	D3D11Utils::CreateConstBuffer(m_device, m_globalConstsCPU, m_globalConstsGPU);
	D3D11Utils::CreateConstBuffer(m_device, m_reflectGlobalConstsCPU, m_reflectGlobalConstsGPU);

	// Shadow Map �������� �� ����� GlobalConsts
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		D3D11Utils::CreateConstBuffer(m_device, m_shadowGlobalConstsCPU[i], m_shadowGlobalConstsGPU[i]);
	}

	// PostEffects�� ����� ConstsBuffer
	D3D11Utils::CreateConstBuffer(m_device, m_postEffectsConstsCPU, m_postEffectsConstsGPU);

	return true;
}

bool AppBase::InitGUI()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.DisplaySize = ImVec2(float(m_screenWidth), float(m_screenHeight));
	ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	if (!ImGui_ImplDX11_Init(m_device.Get(), m_context.Get()))
	{
		return false;
	}

	if (!ImGui_ImplWin32_Init(m_mainWindow))
	{
		return false;
	}

	return true;
}

void AppBase::CreateBuffers()
{
	// Rasterize -> float/DepthBuffer(MSAA) -> Resolved(Not MSAA) -> backBuffer

	ComPtr<ID3D11Texture2D> backBuffer;
	ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())));
	ThrowIfFailed(m_device->CreateRenderTargetView(backBuffer.Get(), NULL, m_backBufferRTV.GetAddressOf()));

	// Float MSAA RenderTargetView/ShaderResourceView
	ThrowIfFailed(m_device->CheckMultisampleQualityLevels(DXGI_FORMAT_R16G16B16A16_FLOAT, 4, &m_numQualityLevels));

	D3D11_TEXTURE2D_DESC texDesc;
	backBuffer->GetDesc(&texDesc);
	texDesc.MipLevels = texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.MiscFlags = 0;
	texDesc.CPUAccessFlags = 0;
	if (m_useMSAA && m_numQualityLevels)
	{
		texDesc.SampleDesc.Count = 4;
		texDesc.SampleDesc.Quality = m_numQualityLevels - 1;
	}
	else
	{
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
	}

	ThrowIfFailed(m_device->CreateTexture2D(&texDesc, NULL, m_floatBuffer.GetAddressOf()));
	ThrowIfFailed(m_device->CreateRenderTargetView(m_floatBuffer.Get(), NULL, m_floatRTV.GetAddressOf()));

	// Float MSAA�� Resovle�ؼ� ������ SRV/RTV
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	ThrowIfFailed(m_device->CreateTexture2D(&texDesc, NULL, m_resolvedBuffer.GetAddressOf()));
	ThrowIfFailed(m_device->CreateTexture2D(&texDesc, NULL, m_postEffectsBuffer.GetAddressOf()));
	ThrowIfFailed(m_device->CreateShaderResourceView(m_resolvedBuffer.Get(), NULL, m_resolvedSRV.GetAddressOf()));
	ThrowIfFailed(m_device->CreateShaderResourceView(m_postEffectsBuffer.Get(), NULL, m_postEffectsSRV.GetAddressOf()));
	ThrowIfFailed(m_device->CreateRenderTargetView(m_resolvedBuffer.Get(), NULL, m_resolvedRTV.GetAddressOf()));
	ThrowIfFailed(m_device->CreateRenderTargetView(m_postEffectsBuffer.Get(), NULL, m_postEffectsRTV.GetAddressOf()));

	CreateDepthBuffers();

	m_postProcess.Initialize(m_device, m_context,
							{ m_postEffectsSRV },
							{ m_backBufferRTV },
							m_screenWidth, m_screenHeight, 4);
}

void AppBase::SetMainViewport()
{
	// Set the viewport
	ZeroMemory(&m_screenViewport, sizeof(D3D11_VIEWPORT));
	m_screenViewport.TopLeftX = 0;
	m_screenViewport.TopLeftY = 0;
	m_screenViewport.Width = float(m_screenWidth);
	m_screenViewport.Height = float(m_screenHeight);
	m_screenViewport.MinDepth = 0.0f;
	m_screenViewport.MaxDepth = 1.0f;

	m_context->RSSetViewports(1, &m_screenViewport);
}

void AppBase::SetShadowViewport()
{
	// Set the Shadow viewport
	D3D11_VIEWPORT shadowViewport;
	ZeroMemory(&shadowViewport, sizeof(D3D11_VIEWPORT));
	shadowViewport.TopLeftX = 0;
	shadowViewport.TopLeftY = 0;
	shadowViewport.Width = float(m_shadowWidth);
	shadowViewport.Height = float(m_shadowHeight);
	shadowViewport.MinDepth = 0.0f;
	shadowViewport.MaxDepth = 1.0f;

	m_context->RSSetViewports(1, &shadowViewport);
}
