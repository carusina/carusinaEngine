#pragma once

#include <directxtk/SimpleMath.h>

// Common.hlsli와 동일하게 맞춰주기
#define MAX_LIGHTS 3
#define LIGHT_OFF 0x00
#define LIGHT_DIRECTOINAL 0x01
#define LIGHT_POINT 0X02
#define LIGHT_SPOT 0x04
#define LIGHT_SHADOW 0x10

// for Vertex / Geometry Shader
__declspec(align(256)) struct MeshConstants {
	DirectX::SimpleMath::Matrix world; // World Matrix
	DirectX::SimpleMath::Matrix worldIT; // World Inverse Transpose for normal
	int useHeightMap = 0;
	float heightScale = 0.0f;
	DirectX::SimpleMath::Vector2 dummy;
};

// for Pixel Shader
__declspec(align(256)) struct MaterialConstants {
	DirectX::SimpleMath::Vector3 albedoFactor = DirectX::SimpleMath::Vector3(1.0f);
	float roughnessFactor = 1.0f;
	float metallicFactor = 1.0f;
	DirectX::SimpleMath::Vector3 emissionFactor = DirectX::SimpleMath::Vector3(0.0f);
	
	// Option
	int useAlbedoMap = 0;
	int useNormalMap = 0;
	int useAoMap = 0;
	int invertNoramlMapY = 0;
	int useMetallicMap = 0;
	int useRoughnessMap = 0;
	int useEmissiveMap = 0;
	float dummy = 0.0f;
};

// about Light, Use in GlobalConstants
struct Light {
	DirectX::SimpleMath::Vector3 position = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -2.0f);
	DirectX::SimpleMath::Vector3 direction = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f);
	float fallOfStart = 0.0f;
	float fallOfEnd = 20.0f;
	DirectX::SimpleMath::Vector3 radiance = DirectX::SimpleMath::Vector3(5.0f); 
	float spotPower = 6.0f;
	
	// Light type bitMasking
	uint32_t type = LIGHT_OFF;
	float radius = 0.0f; // Light radius

	float haloRadius = 0.0f;
	float haloStrength = 0.0f;

	DirectX::SimpleMath::Matrix viewProj; // for Shadow Rendering
	DirectX::SimpleMath::Matrix invProj;  // for Shadow Rendering Debug
};

__declspec(align(256)) struct  GlobalConstants {
	DirectX::SimpleMath::Matrix view;
	DirectX::SimpleMath::Matrix proj;
	DirectX::SimpleMath::Matrix invProj; // Inverse Projection Martix
	DirectX::SimpleMath::Matrix viewProj;
	DirectX::SimpleMath::Matrix invViewProj; // Projection Space -> World Space

	DirectX::SimpleMath::Vector3 eyeWorld;
	
	float IblStrength = 0.0f;
	int textureToDraw = 0; // 0: Env, 1: Specular, 2: Irradiance, else: Black
	float envLodBias = 0.0f;
	float lodBias = 2.0f;
	float dummy = 0.0f;

	Light lights[MAX_LIGHTS];
};

// for PostEffectsPS
__declspec(align(256)) struct PostEffectsConstants {
	int mode = 1; // 1: Rendered Image, 2: Depth Only
	float depthScale = 1.0f;
	float fogStrength = 0.0f;
	float dummy = 0.0f;
};