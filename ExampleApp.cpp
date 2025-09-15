#include "ExampleApp.h"

#include <DirectXCollision.h>
#include <directxtk/DDSTextureLoader.h>

#include <tuple>
#include <vector>

#include "GraphicsCommon.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

ExampleApp::ExampleApp() : AppBase() {}

bool ExampleApp::Initialize()
{
	if (!AppBase::Initialize())
	{
		return false;
	}
	
	AppBase::InitCupemaps(L"Assets/Textures/Cubemaps/HDRI/",
					   	  L"SampleEnvHDR.dds", L"SampleSpecularHDR.dds",
						  L"SampleDiffuseHDR.dds", L"SampleBrdf.dds");

	// ��ó���� ȭ�� �簢��
	{
		MeshData meshData = GeometryGenerator::MakeSquare();
		m_screenSquare = make_shared<Model>(m_device, m_context, vector{ meshData });
	}

	// Skybox
	{
		MeshData skyboxMesh = GeometryGenerator::MakeBox(40.0f);
		std::reverse(skyboxMesh.indices.begin(), skyboxMesh.indices.end());
		m_skybox = make_shared<Model>(m_device, m_context, vector{ skyboxMesh });
	}

	// �ٴ�(�ſ�)
	{
		MeshData groundMesh = GeometryGenerator::MakeSquare(5.0f);
		m_ground = make_shared<Model>(m_device, m_context, vector{ groundMesh });
		m_ground->m_materialConstsCPU.albedoFactor = Vector3(0.1f);
		m_ground->m_materialConstsCPU.emissionFactor = Vector3(0.0f);
		m_ground->m_materialConstsCPU.metallicFactor = 0.5f;
		m_ground->m_materialConstsCPU.roughnessFactor = 0.3f;

		Vector3 position = Vector3(0.0f, -0.5f, 2.0f);
		m_ground->UpdateWorldRow(Matrix::CreateRotationX(3.141592f * 0.5f) *
								 Matrix::CreateTranslation(position));

		m_mirrorPlane = Plane(position, Vector3(0.0f, 1.0f, 0.0f));
		m_mirror = m_ground;
	}

	// Main Object
	{
		vector<MeshData> mainMeshes = GeometryGenerator::ReadFromFile("Assets/Models/medieval_vagrant_knights/",
																	  "scene.gltf", true);
		//vector<MeshData> mainMeshes = { GeometryGenerator::MakeSphere(0.4f, 50, 50) };

		Vector3 center(0.0f, 0.0f, 2.0f);
		m_mainObj = make_shared<Model>(m_device, m_context, mainMeshes);
		m_mainObj->m_materialConstsCPU.invertNoramlMapY = true; // GLTF�� true
		m_mainObj->m_materialConstsCPU.albedoFactor - Vector3(1.0f);
		m_mainObj->m_materialConstsCPU.roughnessFactor = 0.3f;
		m_mainObj->m_materialConstsCPU.metallicFactor = 0.8f;
		m_mainObj->UpdateWorldRow(Matrix::CreateTranslation(center));

		m_basicList.push_back(m_mainObj);
		
		m_mainBoundingSphere = BoundingSphere(center, 0.4f);
	}

	// Lights
	{
		// ���� 0�� ����
		m_globalConstsCPU.lights[0].radiance = Vector3(5.0f);
		m_globalConstsCPU.lights[0].position = Vector3(0.0f, 1.5f, 1.1f);
		m_globalConstsCPU.lights[0].direction = Vector3(0.0f, -1.0f, 0.0f);
		m_globalConstsCPU.lights[0].spotPower = 3.0f;
		m_globalConstsCPU.lights[0].radius = 0.02f;
		m_globalConstsCPU.lights[0].type = LIGHT_SPOT | LIGHT_SHADOW;

		// ���� 1�� Update
		m_globalConstsCPU.lights[1].radiance = Vector3(5.0f);
		m_globalConstsCPU.lights[1].spotPower = 3.0f;
		m_globalConstsCPU.lights[1].fallOfEnd = 20.0f;
		m_globalConstsCPU.lights[1].radius = 0.02f;
		m_globalConstsCPU.lights[1].type = LIGHT_SPOT | LIGHT_SHADOW;

		// ���� 2�� ��
		m_globalConstsCPU.lights[2].type = LIGHT_OFF;
	}

	// Lights ��ġ ǥ��
	{
		for (int i = 0; i < MAX_LIGHTS; i++)
		{
			MeshData sphere = GeometryGenerator::MakeSphere(1.0f, 20, 20);
			m_lightSphere[i] = make_shared<Model>(m_device, m_context, vector{ sphere });
			m_lightSphere[i]->UpdateWorldRow(Matrix::CreateTranslation(m_globalConstsCPU.lights[i].position));
			m_lightSphere[i]->m_materialConstsCPU.albedoFactor = Vector3(0.0f);
			m_lightSphere[i]->m_materialConstsCPU.emissionFactor = Vector3(1.0f, 1.0f, 0.0f);
			m_lightSphere[i]->m_castShadow = false; // �׸��� X

			if (m_globalConstsCPU.lights[i].type == 0)
			{
				m_lightSphere[i]->m_isVisible = false;
			}

			m_basicList.push_back(m_lightSphere[i]);
		}
	}

	// ���콺 Ŀ�� ǥ��
	{
		MeshData sphere = GeometryGenerator::MakeSphere(0.01f, 10, 10);
		m_cursorSphere = make_shared<Model>(m_device, m_context, vector{ sphere });
		m_cursorSphere->m_isVisible = false; // ���콺 Ŭ������ ���� ����
		m_cursorSphere->m_castShadow = false; // �׸��� X
		m_cursorSphere->m_materialConstsCPU.albedoFactor = Vector3(0.0f);
		m_cursorSphere->m_materialConstsCPU.emissionFactor = Vector3(0.0f, 1.0f, 0.0f);

		m_basicList.push_back(m_cursorSphere);
	}

	return true;
}

void ExampleApp::UpdateGUI()
{
	ImGui::SetNextItemOpen(false, ImGuiCond_Once);
	if (ImGui::TreeNode("General"))
	{
		ImGui::Checkbox("Use FPV", &m_camera.m_useFirstPersonView);
		ImGui::Checkbox("Wireframe", &m_drawAsWire);
		if (ImGui::Checkbox("MSAA ON", &m_useMSAA))
		{
			CreateBuffers();
		}
		ImGui::Checkbox("Perspective Projection", &m_camera.m_usePerspectiveProjection);
		ImGui::TreePop();
	}

	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Skybox"))
	{
		ImGui::SliderFloat("Strength", &m_globalConstsCPU.IblStrength, 0.0f, 5.0f);
		ImGui::RadioButton("Env", &m_globalConstsCPU.textureToDraw, 0);
		ImGui::SameLine();
		ImGui::RadioButton("Specular", &m_globalConstsCPU.textureToDraw, 1);
		ImGui::SameLine();
		ImGui::RadioButton("Irradiance", &m_globalConstsCPU.textureToDraw, 2);
		ImGui::SliderFloat("EnvLodBias", &m_globalConstsCPU.envLodBias, 0.0f, 10.0f);
		ImGui::TreePop();
	}

	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Post Effects"))
	{
		int flag = 0;
		flag += ImGui::RadioButton("Render", &m_postEffectsConstsCPU.mode, 1);
		ImGui::SameLine();
		flag += ImGui::RadioButton("Depth", &m_postEffectsConstsCPU.mode, 2);
		flag += ImGui::SliderFloat("DepthScale", &m_postEffectsConstsCPU.depthScale, 0.0, 1.0);
		flag += ImGui::SliderFloat("Fog", &m_postEffectsConstsCPU.fogStrength, 0.0, 10.0);

		if (flag)
		{
			D3D11Utils::UpdateBuffer(m_device, m_context,
									 m_postEffectsConstsCPU,
									 m_postEffectsConstsGPU);
		}
			
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Post Processing"))
	{
		int flag = 0;
		flag += ImGui::SliderFloat("Bloom Strength",
								   &m_postProcess.m_combineFilter.m_constData.strength,
								   0.0f, 1.0f);
		flag += ImGui::SliderFloat("Exposure",
								   &m_postProcess.m_combineFilter.m_constData.option1,
								   0.0f, 10.0f);
		flag += ImGui::SliderFloat("Gamma",
								   &m_postProcess.m_combineFilter.m_constData.option2,
								   0.1f, 5.0f);
		// ���ǻ� ����� �Է��� �νĵǸ� �ٷ� GPU ���۸� ������Ʈ
		if (flag)
		{
			m_postProcess.m_combineFilter.UpdateConstantBuffers(m_device,m_context);
		}
		ImGui::TreePop();
	}

	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Mirror"))
	{

		ImGui::SliderFloat("Alpha", &m_mirrorAlpha, 0.0f, 1.0f);
		const float blendColor[4] = { m_mirrorAlpha, m_mirrorAlpha,
									  m_mirrorAlpha, 1.0f };
		if (m_drawAsWire)
		{
			Graphics::mirrorBlendWirePSO.SetBlendFactor(blendColor);
		}
		else
		{
			Graphics::mirrorBlendSolidPSO.SetBlendFactor(blendColor);
		}
		ImGui::SliderFloat("Metallic",
						   &m_mirror->m_materialConstsCPU.metallicFactor,
						   0.0f, 1.0f);
		ImGui::SliderFloat("Roughness",
						   &m_mirror->m_materialConstsCPU.roughnessFactor,
						   0.0f, 1.0f);
		ImGui::TreePop();
	}

	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Light"))
	{
		ImGui::SliderFloat("Halo Radius",
						   &m_globalConstsCPU.lights[1].haloRadius,
						   0.0f, 2.0f);
		ImGui::SliderFloat("Halo Strength",
						   &m_globalConstsCPU.lights[1].haloStrength,
						   0.0f, 1.0f);
		ImGui::SliderFloat("Radius",
						   &m_globalConstsCPU.lights[1].radius,
						   0.0f, 0.5f);
		ImGui::TreePop();
	}

	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Material"))
	{
		ImGui::SliderFloat("LodBias", &m_globalConstsCPU.lodBias, 0.0f, 10.0f);

		int flag = 0;

		flag += ImGui::SliderFloat("Metallic",
								   &m_mainObj->m_materialConstsCPU.metallicFactor,
								   0.0f, 1.0f);
		flag += ImGui::SliderFloat("Roughness",
								   &m_mainObj->m_materialConstsCPU.roughnessFactor,
								   0.0f, 1.0f);
		flag += ImGui::CheckboxFlags("AlbedoTexture",
								     &m_mainObj->m_materialConstsCPU.useAlbedoMap, 1);
		flag += ImGui::CheckboxFlags("EmissiveTexture",
									 &m_mainObj->m_materialConstsCPU.useEmissiveMap, 1);
		flag += ImGui::CheckboxFlags("Use NormalMapping",
									 &m_mainObj->m_materialConstsCPU.useNormalMap, 1);
		flag += ImGui::CheckboxFlags("Use AO",
									 &m_mainObj->m_materialConstsCPU.useAoMap, 1);
		flag += ImGui::CheckboxFlags("Use HeightMapping",
									 &m_mainObj->m_meshConstsCPU.useHeightMap, 1);
		flag += ImGui::SliderFloat("HeightScale",
								   &m_mainObj->m_meshConstsCPU.heightScale,
								   0.0f, 0.1f);
		flag += ImGui::CheckboxFlags("Use MetallicMap",
									 &m_mainObj->m_materialConstsCPU.useMetallicMap, 1);
		flag += ImGui::CheckboxFlags("Use RoughnessMap",
									 &m_mainObj->m_materialConstsCPU.useRoughnessMap, 1);

		if (flag) {
			m_mainObj->UpdateConstantBuffers(m_device, m_context);
		}

		ImGui::Checkbox("Draw Normals", &m_mainObj->m_drawNormals);

		ImGui::TreePop();
	}
}

void ExampleApp::Update(float dt)
{
	// Camera �̵�
	m_camera.UpdateKeyboard(dt, m_keyPressed);

	const Vector3 eyeWorld = m_camera.GetEyePos();
	const Matrix reflectRow = Matrix::CreateReflection(m_mirrorPlane);
	const Matrix viewRow = m_camera.GetViewRow();
	const Matrix projRow = m_camera.GetProjRow();

	UpdateLights(dt);

	// ���� ConstantBuffer ������Ʈ
	AppBase::UpdateGlobalConstants(eyeWorld, viewRow, projRow, reflectRow);

	// �ſ��� ���� ó��
	m_mirror->UpdateConstantBuffers(m_device, m_context);

	// ������ ��ġ �ݿ�
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		m_lightSphere[i]->UpdateWorldRow(Matrix::CreateScale(max(0.01f, m_globalConstsCPU.lights[i].radius)) *
										 Matrix::CreateTranslation(m_globalConstsCPU.lights[i].position));
	}
		

	// ���콺 �̵�/ȸ�� �ݿ�
	if (m_leftButton || m_rightButton)
	{
		Quaternion q;
		Vector3 dragTranslation;
		Vector3 pickPoint;

		if (UpdateMouseControl(m_mainBoundingSphere, q, dragTranslation, pickPoint))
		{
			Vector3 translation = m_mainObj->m_worldRow.Translation();
			m_mainObj->m_worldRow.Translation(Vector3(0.0f));
			m_mainObj->UpdateWorldRow(m_mainObj->m_worldRow *
									  Matrix::CreateFromQuaternion(q) *
									  Matrix::CreateTranslation(dragTranslation + translation));
			m_mainBoundingSphere.Center = m_mainObj->m_worldRow.Translation();

			// �浹 ������ ���� �� �׸���
			m_cursorSphere->m_isVisible = true;
			m_cursorSphere->UpdateWorldRow(Matrix::CreateTranslation(pickPoint));
		}
		else
		{
			m_cursorSphere->m_isVisible = false;
		}
	}
	else
	{
		m_cursorSphere->m_isVisible = false;
	}

	for (shared_ptr<Model>& i : m_basicList)
	{
		i->UpdateConstantBuffers(m_device, m_context);
	}
}

void ExampleApp::Render()
{
	AppBase::SetMainViewport();

	// ��� ���÷����� �������� ���
	m_context->VSSetSamplers(0, UINT(Graphics::sampleStates.size()),
		Graphics::sampleStates.data());
	m_context->PSSetSamplers(0, UINT(Graphics::sampleStates.size()),
		Graphics::sampleStates.data());

	// ���� �ؽ����: "Common.hlsli"���� register(t10)���� ����
	vector<ID3D11ShaderResourceView*> commonSRVs = { m_envSRV.Get(), m_specularSRV.Get(),
													 m_irradianceSRV.Get(), m_brdfSRV.Get() };
	m_context->PSSetShaderResources(10, UINT(commonSRVs.size()), commonSRVs.data());

	const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	vector<ID3D11RenderTargetView*> rtvs = { m_floatRTV.Get() };

	// Depth Only Pass (RTS ���� ����)
	m_context->OMSetRenderTargets(0, NULL, m_depthOnlyDSV.Get());
	m_context->ClearDepthStencilView(m_depthOnlyDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	AppBase::SetPipelineState(Graphics::depthOnlyPSO);
	AppBase::SetGlobalConsts(m_globalConstsGPU);

	for (auto& i : m_basicList)
	{
		i->Render(m_context);
	}
	m_skybox->Render(m_context);
	m_mirror->Render(m_context);

	// �׸��ڸ� �����
	AppBase::SetShadowViewport(); // �׸��ڸ� �ػ�
	AppBase::SetPipelineState(Graphics::depthOnlyPSO);
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		if (m_globalConstsCPU.lights[i].type & LIGHT_SHADOW)
		{
			// RTS ���� ����
			m_context->OMSetRenderTargets(0, NULL, m_shadowDSVs[i].Get());
			m_context->ClearDepthStencilView(m_shadowDSVs[i].Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
			AppBase::SetGlobalConsts(m_shadowGlobalConstsGPU[i]);
			for (auto& i : m_basicList)
			{
				if (i->m_castShadow && i->m_isVisible)
				{
					i->Render(m_context);
				}
			}
				
			m_skybox->Render(m_context);
			m_mirror->Render(m_context);
		}
	}

	// �ٽ� ������ �ػ󵵷� �ǵ�����
	AppBase::SetMainViewport();

	// �ſ� 1. �ſ��� ���� ���� ��� �׸���
	for (size_t i = 0; i < rtvs.size(); i++)
	{
		m_context->ClearRenderTargetView(rtvs[i], clearColor);
	}
	m_context->OMSetRenderTargets(UINT(rtvs.size()), rtvs.data(), m_depthStencilView.Get());

	// �׸��ڸʵ鵵 ���� �ؽ���� ���Ŀ� �߰�
	// ����: ������ shadowDSV�� RenderTarget���� ������ �� ����
	vector<ID3D11ShaderResourceView*> shadowSRVs;
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		shadowSRVs.push_back(m_shadowSRVs[i].Get());
	}
	m_context->PSSetShaderResources(15, UINT(shadowSRVs.size()), shadowSRVs.data());

	m_context->ClearDepthStencilView(m_depthStencilView.Get(),
									 D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
									 1.0f, 0);
	AppBase::SetPipelineState(m_drawAsWire ? Graphics::defaultWirePSO
										   : Graphics::defaultSolidPSO);
	AppBase::SetGlobalConsts(m_globalConstsGPU);

	for (auto& i : m_basicList)
	{
		i->Render(m_context);
	}

	// �ſ� �ݻ縦 �׸� �ʿ䰡 ������ ������ �ſ︸ �׸���
	if (m_mirrorAlpha == 1.0f)
	{
		m_mirror->Render(m_context);
	}

	AppBase::SetPipelineState(Graphics::normalsPSO);
	for (auto& i : m_basicList)
	{
		if (i->m_drawNormals)
		{
			i->RenderNormals(m_context);
		}
	}

	AppBase::SetPipelineState(m_drawAsWire ? Graphics::skyboxWirePSO
										   : Graphics::skyboxSolidPSO);

	m_skybox->Render(m_context);

	if (m_mirrorAlpha < 1.0f)
	{ // �ſ��� �׷��� �ϴ� ��Ȳ

		// �ſ� 2. �ſ� ��ġ�� StencilBuffer�� 1�� ǥ��
		AppBase::SetPipelineState(Graphics::stencilMaskPSO);

		m_mirror->Render(m_context);

		// �ſ� 3. �ſ� ��ġ�� �ݻ�� ��ü���� ������
		AppBase::SetPipelineState(m_drawAsWire ? Graphics::reflectWirePSO
											   : Graphics::reflectSolidPSO);
		AppBase::SetGlobalConsts(m_reflectGlobalConstsGPU);

		m_context->ClearDepthStencilView(m_depthStencilView.Get(),
										 D3D11_CLEAR_DEPTH, 1.0f, 0);

		for (auto& i : m_basicList)
		{
			i->Render(m_context);
		}

		AppBase::SetPipelineState(m_drawAsWire ? Graphics::reflectSkyboxWirePSO
											   : Graphics::reflectSkyboxSolidPSO);
		m_skybox->Render(m_context);

		// �ſ� 4. �ſ� ��ü�� ������ "Blend"�� �׸�
		AppBase::SetPipelineState(m_drawAsWire ? Graphics::mirrorBlendWirePSO
											   : Graphics::mirrorBlendSolidPSO);
		AppBase::SetGlobalConsts(m_globalConstsGPU);

		m_mirror->Render(m_context);

	} // end of if (m_mirrorAlpha < 1.0f)

	m_context->ResolveSubresource(m_resolvedBuffer.Get(), 0,
								  m_floatBuffer.Get(), 0,
								  DXGI_FORMAT_R16G16B16A16_FLOAT);

	// PostEffects
	AppBase::SetPipelineState(Graphics::postEffectsPSO);

	vector<ID3D11ShaderResourceView*> postEffectsSRVs = { m_resolvedSRV.Get(),
														  m_depthOnlySRV.Get() };

	AppBase::SetGlobalConsts(m_globalConstsGPU);

	// 20���� �־���
	m_context->PSSetShaderResources(20, UINT(postEffectsSRVs.size()), postEffectsSRVs.data());
	m_context->OMSetRenderTargets(1, m_postEffectsRTV.GetAddressOf(), NULL);

	m_context->PSSetConstantBuffers(3, 1, m_postEffectsConstsGPU.GetAddressOf());
	m_screenSquare->Render(m_context);

	// �ܼ� �̹��� ó���� ���
	AppBase::SetPipelineState(Graphics::postProcessingPSO);
	m_postProcess.Render(m_context);
}

void ExampleApp::UpdateLights(float dt)
{
	// ȸ���ϴ� lights[1] Update
	static Vector3 lightDev = Vector3(1.0f, 0.0f, 0.0f);
	if (m_lightRotate)
	{
		lightDev = Vector3::Transform(lightDev, Matrix::CreateRotationY(dt * 3.141592f * 0.5f));
	}
	m_globalConstsCPU.lights[1].position = Vector3(0.0f, 1.1f, 2.0f) + lightDev;
	Vector3 focusPosition = Vector3(0.0f, -0.5f, 1.7f);
	m_globalConstsCPU.lights[1].direction = focusPosition - m_globalConstsCPU.lights[1].position;
	m_globalConstsCPU.lights[1].direction.Normalize();

	// Shadow Map�� ���� ����
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		const Light& light = m_globalConstsCPU.lights[i];
		if (light.type & LIGHT_SHADOW)
		{
			Vector3 upDir = Vector3(0.0f, 1.0f, 0.0f);
			if (abs(upDir.Dot(light.direction) + 1.0f) < 1e-5f)
			{
				upDir = Vector3(1.0f, 0.0f, 0.0f);
			}

			Matrix lightViewRow = XMMatrixLookAtLH(light.position, light.position + light.direction, upDir);
			Matrix lightProjRow = XMMatrixPerspectiveFovLH(XMConvertToRadians(120.0f), 1.0f, 0.1f, 10.0f);
			
			m_shadowGlobalConstsCPU[i].eyeWorld = light.position;
			m_shadowGlobalConstsCPU[i].view = lightViewRow.Transpose();
			m_shadowGlobalConstsCPU[i].proj = lightProjRow.Transpose();
			m_shadowGlobalConstsCPU[i].invProj = lightProjRow.Invert().Transpose();
			m_shadowGlobalConstsCPU[i].viewProj = (lightViewRow * lightProjRow).Transpose();
			D3D11Utils::UpdateBuffer(m_device, m_context, m_shadowGlobalConstsCPU[i], m_shadowGlobalConstsGPU[i]);

			m_globalConstsCPU.lights[i].viewProj = m_shadowGlobalConstsCPU[i].viewProj;
			m_globalConstsCPU.lights[i].invProj = m_shadowGlobalConstsCPU[i].invProj;
		}
	}
}
