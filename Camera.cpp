#include "Camera.h"

#include <iostream>

using namespace std;
using namespace DirectX::SimpleMath;

// Rendering�� ����� View Matrix�� ������ִ� �κ�
DirectX::SimpleMath::Matrix Camera::GetViewRow()
{
	// �̵��� ��ġ���� ȸ���ؾ��� => ���� ����
	// ���� �ƴ϶� ������ �����δ� => ���� �����̴� �Ͱ� ���� ȿ��
	return Matrix::CreateTranslation(-this->m_position) *
		   Matrix::CreateRotationY(-this->m_yaw) *
		   Matrix::CreateRotationX(-this->m_pitch);
}

DirectX::SimpleMath::Matrix Camera::GetProjRow()
{
	return m_usePerspectiveProjection ? DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(m_projFovAngleY), m_aspect, m_nearZ, m_farZ)
									  : DirectX::XMMatrixOrthographicOffCenterLH(-m_aspect, m_aspect, -1.0f, 1.0f, m_nearZ, m_farZ);
}

DirectX::SimpleMath::Vector3 Camera::GetEyePos()
{
	return m_position;
}

void Camera::UpdateViewDir()
{
	m_viewDir = Vector3::Transform(Vector3(0.0f, 0.0f, 1.0f),
								   Matrix::CreateRotationY(this->m_yaw));
	m_viewDir.Normalize();

	m_rightDir = m_upDir.Cross(m_viewDir);
	m_rightDir.Normalize();
}

void Camera::UpdateKeyboard(const float dt, bool const keyPreesed[256])
{
	if (m_useFirstPersonView) {
		if (keyPreesed['W'])
			MoveForward(dt);
		if (keyPreesed['S'])
			MoveForward(-dt);
		if (keyPreesed['D'])
			MoveRight(dt);
		if (keyPreesed['A'])
			MoveRight(-dt);
		if (keyPreesed['E'])
			MoveUp(dt);
		if (keyPreesed['Q'])
			MoveUp(-dt);
	}
}

void Camera::UpdateMouse(float mouseNdcX, float mouseNdcY)
{
	// NDC [-1, 1] x [-1, 1] x [0, 1]
	if (m_useFirstPersonView) {
		m_yaw = mouseNdcX * DirectX::XM_2PI;	  // �¿� 360��
		m_pitch = -mouseNdcY * DirectX::XM_PIDIV2; // �� �Ʒ� 90��

		UpdateViewDir();
	}
}

void Camera::MoveForward(float dt)
{
	m_position += m_viewDir * m_speed * dt;
}

void Camera::MoveRight(float dt)
{
	m_position += m_rightDir * m_speed * dt;
}

void Camera::MoveUp(float dt)
{
	m_position += m_upDir * m_speed * dt;
}

void Camera::SetAspectRatio(float aspect)
{
	m_aspect = aspect;
}
