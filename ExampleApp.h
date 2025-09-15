#pragma once

#include <directxtk/SimpleMath.h>

#include <iostream>
#include <algorithm>
#include <memory>

#include "AppBase.h"
#include "GeometryGenerator.h"
#include "ImageFilter.h"
#include "Model.h"

class ExampleApp : public AppBase {
public:
	ExampleApp();

	virtual bool Initialize() override;
	virtual void UpdateGUI() override;
	virtual void Update(float dt) override;
	virtual void Render() override;

	void UpdateLights(float dt);

protected:
	std::shared_ptr<Model> m_ground;
	std::shared_ptr<Model> m_mainObj;
	std::shared_ptr<Model> m_lightSphere[MAX_LIGHTS];
	std::shared_ptr<Model> m_skybox;
	std::shared_ptr<Model> m_cursorSphere;
	std::shared_ptr<Model> m_screenSquare;

	DirectX::BoundingSphere m_mainBoundingSphere;

	// 거울
	std::shared_ptr<Model> m_mirror;
	DirectX::SimpleMath::Plane m_mirrorPlane;
	float m_mirrorAlpha = 1.0f;

	// 거울 제외 Object 리스트
	std::vector<std::shared_ptr<Model>> m_basicList;
};