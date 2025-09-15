#pragma once

#include <directxtk/SimpleMath.h>

class Camera {
public:
	Camera() { UpdateViewDir(); }

	DirectX::SimpleMath::Matrix GetViewRow();
	DirectX::SimpleMath::Matrix GetProjRow();
	DirectX::SimpleMath::Vector3 GetEyePos();

	void UpdateViewDir();
	void UpdateKeyboard(const float dt, bool const keyPreesed[256]);
	void UpdateMouse(float mouseNdcX, float mouseNdcY);
	
	void MoveForward(float dt);
	void MoveRight(float dt);
	void MoveUp(float dt);
	
	void SetAspectRatio(float aspect);

public:
	bool m_useFirstPersonView = false;
	bool m_usePerspectiveProjection = true;

private:
	DirectX::SimpleMath::Vector3 m_position = DirectX::SimpleMath::Vector3(0.275514f, 0.461257f, 0.0855238f);
	DirectX::SimpleMath::Vector3 m_viewDir = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f);
	DirectX::SimpleMath::Vector3 m_upDir = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
	DirectX::SimpleMath::Vector3 m_rightDir = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);

	float m_yaw = -0.019635f;
	float m_pitch = -0.120477f;

	float m_speed = 3.0f;

	// Projection Option
	float m_projFovAngleY = 90.0f;
	float m_nearZ = 0.01f;
	float m_farZ = 100.0f;
	float m_aspect = 16.0f / 9.0f;
};