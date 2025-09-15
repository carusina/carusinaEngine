#define _CRT_SECURE_NO_WARNINGS // stb_image_write compile error fix

#include "D3D11Utils.h"

#include <DirectXTexEXR.h> // Read .exr
#include <directxtk/DDSTextureLoader.h> // Read CupeMap
#include <dxgi.h>
#include <dxgi1_4.h>
#include <fp16.h>

#include <iostream>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

void CheckResult(HRESULT hr, ID3DBlob *errorBlob) {
	if (FAILED(hr)) {
		// 파일이 없을 경우
		if ((hr & D3D11_ERROR_FILE_NOT_FOUND) != 0) {
			cout << "File not found.\n";
		}

		// 에러 메시지가 있을 경우
		if (errorBlob) {
			cout << "Shader compile error\n" << (char*)errorBlob->GetBufferPointer() << endl;
		}
	}
}

void D3D11Utils::CreateVertexShaderAndInputLayout(Microsoft::WRL::ComPtr<ID3D11Device>& device,
												  const std::wstring& fileName,
												  const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElements,
												  Microsoft::WRL::ComPtr<ID3D11VertexShader>& vertexShader,
												  Microsoft::WRL::ComPtr<ID3D11InputLayout>& inputLayout)
{
	ComPtr<ID3DBlob> shaderBlob;
	ComPtr<ID3DBlob> errorBlob;

	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	HRESULT hr = D3DCompileFromFile(fileName.c_str(), 0,
									D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0",
									compileFlags, 0, &shaderBlob, &errorBlob);

	CheckResult(hr, errorBlob.Get());

	device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(),
							   NULL, &vertexShader);

	device->CreateInputLayout(inputElements.data(), UINT(inputElements.size()),
							  shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(),
							  &inputLayout);
}

void D3D11Utils::CreateHullShader(Microsoft::WRL::ComPtr<ID3D11Device>& device,
								  const std::wstring& fileName,
								  Microsoft::WRL::ComPtr<ID3D11HullShader>& hullShader)
{
	ComPtr<ID3DBlob> shaderBlob;
	ComPtr<ID3DBlob> errorBlob;

	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	HRESULT hr = D3DCompileFromFile(fileName.c_str(), 0,
									D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "hs_5_0",
									compileFlags, 0, &shaderBlob, &errorBlob);

	CheckResult(hr, errorBlob.Get());

	device->CreateHullShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(),
							 NULL, &hullShader);
}

void D3D11Utils::CreateDomainShader(Microsoft::WRL::ComPtr<ID3D11Device>& device,
									const std::wstring& fileName,
									Microsoft::WRL::ComPtr<ID3D11DomainShader>& domainShader)
{
	ComPtr<ID3DBlob> shaderBlob;
	ComPtr<ID3DBlob> errorBlob;

	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	HRESULT hr = D3DCompileFromFile(fileName.c_str(), 0,
									D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ds_5_0",
									compileFlags, 0, &shaderBlob, &errorBlob);

	CheckResult(hr, errorBlob.Get());

	device->CreateDomainShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(),
							   NULL, &domainShader);
}

void D3D11Utils::CreateGeometryShader(Microsoft::WRL::ComPtr<ID3D11Device>& device,
									  const std::wstring& fileName,
									  Microsoft::WRL::ComPtr<ID3D11GeometryShader>& geometryShader)
{
	ComPtr<ID3DBlob> shaderBlob;
	ComPtr<ID3DBlob> errorBlob;

	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	HRESULT hr = D3DCompileFromFile(fileName.c_str(), 0,
									D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "gs_5_0",
									compileFlags, 0, &shaderBlob, &errorBlob);

	CheckResult(hr, errorBlob.Get());

	device->CreateGeometryShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(),
							   NULL, &geometryShader);
}

void D3D11Utils::CreatePixelShader(Microsoft::WRL::ComPtr<ID3D11Device>& device,
								   const std::wstring& fileName,
								   Microsoft::WRL::ComPtr<ID3D11PixelShader>& pixelShader)
{
	ComPtr<ID3DBlob> shaderBlob;
	ComPtr<ID3DBlob> errorBlob;

	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	HRESULT hr = D3DCompileFromFile(fileName.c_str(), 0,
									D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0",
									compileFlags, 0, &shaderBlob, &errorBlob);

	CheckResult(hr, errorBlob.Get());

	device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(),
							   NULL, &pixelShader);
}

void D3D11Utils::CreateIndexBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& device,
								   const std::vector<uint32_t>& indices,
								   Microsoft::WRL::ComPtr<ID3D11Buffer>& indexBuffer)
{
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth = UINT(sizeof(uint32_t) * indices.size());
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.StructureByteStride = sizeof(uint32_t);

	D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
	indexBufferData.pSysMem = indices.data();
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;

	device->CreateBuffer(&bufferDesc, &indexBufferData,
						 indexBuffer.GetAddressOf());
}

void ReadEXRImage(const std::string fileName, std::vector<uint8_t>& image,
				  int& width, int& height, DXGI_FORMAT& pixelFormat)
{
	const std::wstring wFilename(fileName.begin(), fileName.end());

	TexMetadata metadata;
	ThrowIfFailed(GetMetadataFromEXRFile(wFilename.c_str(), metadata));

	ScratchImage scratchImage;
	ThrowIfFailed(LoadFromEXRFile(wFilename.c_str(), NULL, scratchImage));

	width = static_cast<int>(metadata.width);
	height = static_cast<int>(metadata.height);
	pixelFormat = metadata.format;

	cout << fileName << " " << metadata.width << " " << metadata.height
		<< metadata.format << endl;

	image.resize(scratchImage.GetPixelsSize());
	memcpy(image.data(), scratchImage.GetPixels(), image.size());

	// 데이터 범위 확인해보기
	vector<float> f32(image.size() / 2);
	uint16_t* f16 = (uint16_t*)image.data();
	for (int i = 0; i < image.size() / 2; i++) {
		f32[i] = fp16_ieee_to_fp32_value(f16[i]);
	}

	const float minValue = *std::min_element(f32.begin(), f32.end());
	const float maxValue = *std::max_element(f32.begin(), f32.end());

	cout << minValue << " " << maxValue << endl;
}

void ReadImage(const std::string filename, std::vector<uint8_t>& image,
			   int& width, int& height)
{
	int channels;

	unsigned char* img =
		stbi_load(filename.c_str(), &width, &height, &channels, 0);

	cout << filename << " " << width << " " << height << " " << channels
		<< endl;

	// 4채널로 만들어서 복사
	image.resize(width * height * 4);

	if (channels == 1) {
		for (size_t i = 0; i < width * height; i++) {
			uint8_t g = img[i * channels + 0];
			for (size_t c = 0; c < 4; c++) {
				image[4 * i + c] = g;
			}
		}
	}
	else if (channels == 2) {
		for (size_t i = 0; i < width * height; i++) {
			for (size_t c = 0; c < 2; c++) {
				image[4 * i + c] = img[i * channels + c];
			}
			image[4 * i + 2] = 255;
			image[4 * i + 3] = 255;
		}
	}
	else if (channels == 3) {
		for (size_t i = 0; i < width * height; i++) {
			for (size_t c = 0; c < 3; c++) {
				image[4 * i + c] = img[i * channels + c];
			}
			image[4 * i + 3] = 255;
		}
	}
	else if (channels == 4) {
		for (size_t i = 0; i < width * height; i++) {
			for (size_t c = 0; c < 4; c++) {
				image[4 * i + c] = img[i * channels + c];
			}
		}
	}
	else {
		std::cout << "Cannot read " << channels << " channels" << endl;
	}

	delete[] img;
}

ComPtr<ID3D11Texture2D> CreateStagingTexture(ComPtr<ID3D11Device>& device,
											 ComPtr<ID3D11DeviceContext>& context,
											 const int width, const int height,
											 const vector<uint8_t>& image,
											 const DXGI_FORMAT pixelFormat = DXGI_FORMAT_R8G8B8A8_UNORM,
											 const int mipLevels = 1, const int arraySize = 1)
{
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = mipLevels;
	texDesc.ArraySize = arraySize;
	texDesc.Format = pixelFormat;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_STAGING;
	texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;

	ComPtr<ID3D11Texture2D> stagingTexture;
	if (FAILED(device->CreateTexture2D(&texDesc, NULL, stagingTexture.GetAddressOf())))
	{
		cout << "Failed()\n";
	}

	// CPU에서 이미지 데이터 복사
	size_t pixelSize = sizeof(uint8_t) * 4;
	if (pixelFormat == DXGI_FORMAT_R16G16B16A16_FLOAT)
	{
		pixelSize = sizeof(uint16_t) * 4;
	}

	D3D11_MAPPED_SUBRESOURCE ms;
	context->Map(stagingTexture.Get(), NULL, D3D11_MAP_WRITE, NULL, &ms);
	uint8_t *pData = (uint8_t *)ms.pData;
	for (UINT h = 0; h < UINT(height); h++) // 가로로 한 줄씩 복사
	{
		memcpy(&pData[h * ms.RowPitch],
			   &image[h * width * pixelSize],
			   width * pixelSize);
	}
	context->Unmap(stagingTexture.Get(), NULL);

	return stagingTexture;
}

void CreateTextureHelper(ComPtr<ID3D11Device>& device,
						 ComPtr<ID3D11DeviceContext>& context,
						 const int width, const int height,
						 const vector<uint8_t>& image,
						 const DXGI_FORMAT pixelFormat,
						 ComPtr<ID3D11Texture2D>& texture,
						 ComPtr<ID3D11ShaderResourceView>& srv)
{
	// Staging Texture를 만들고 CPU에서 이미지를 복사
	ComPtr<ID3D11Texture2D> stagingTexture = CreateStagingTexture(device, context, width, height, image, pixelFormat);

	// 실제 사용할 texture 설정
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 0; // Mipmap 최대 레벨 사용
	texDesc.ArraySize = 1;
	texDesc.Format = pixelFormat;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT; // Staging texture로부터 복사 가능
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS; // Mipmap 사용
	texDesc.CPUAccessFlags = 0;

	// 초기 데이터 없이 texture 생성
	device->CreateTexture2D(&texDesc, NULL, texture.GetAddressOf());
	
	// Staging texture로부터 가장 높은 해상도의 texture 복사
	context->CopySubresourceRegion(texture.Get(), 0, 0, 0, 0,
								   stagingTexture.Get(), 0, NULL);

	// SRV 생성
	device->CreateShaderResourceView(texture.Get(), 0, srv.GetAddressOf());

	// 해상도를 낮춰가며 Mipmap 생성
	context->GenerateMips(srv.Get());
}

void D3D11Utils::CreateTexture(Microsoft::WRL::ComPtr<ID3D11Device>& device,
							   Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
							   const std::string fileName,
							   const bool useSRGB,
							   Microsoft::WRL::ComPtr<ID3D11Texture2D>& texture,
							   Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv)
{

	int width = 0, height = 0;
	vector<uint8_t> image;
	DXGI_FORMAT pixelFormat = useSRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB
									  : DXGI_FORMAT_R8G8B8A8_UNORM;

	string ext(fileName.end() - 3, fileName.end());
	std::transform(ext.begin(), ext.end(), ext.begin(), std::tolower); // 확장자 소문자로 변환
	if (ext == "exr")
	{
		ReadEXRImage(fileName, image, width, height, pixelFormat);
	}
	else
	{
		ReadImage(fileName, image, width, height);
	}

	CreateTextureHelper(device, context, width, height, image, pixelFormat, texture, srv);
}

void D3D11Utils::CreateMetallicRoughnessTexture(Microsoft::WRL::ComPtr<ID3D11Device>& device,
												Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
												const std::string metallicFimeName,
												const std::string roughnessFileName,
												Microsoft::WRL::ComPtr<ID3D11Texture2D>& texture,
												Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv)
{
	if (!metallicFimeName.empty() && (metallicFimeName == roughnessFileName))
	{
		CreateTexture(device, context, metallicFimeName, false, texture, srv);
	}
	else
	{
		int mWidth = 0, mHeight = 0;
		int rWidth = 0, rHeight = 0;
		vector<uint8_t> mImage;
		vector<uint8_t> rImage;

		if (!metallicFimeName.empty())
		{
			ReadImage(metallicFimeName, mImage, mWidth, mHeight);
		}
		if (!roughnessFileName.empty())
		{
			ReadImage(roughnessFileName, rImage, rWidth, rHeight);
		}

		// MetallicTexture와 RoughnessTexture의 해상도가 같다고 가정
		if (!metallicFimeName.empty() && !roughnessFileName.empty())
		{
			assert(mWidth == rWidth);
			assert(mHeight == rHeight);
		}

		vector<uint8_t> combinedImage(size_t(mWidth * mHeight) * 4);
		fill(combinedImage.begin(), combinedImage.end(), 0);

		for (size_t i = 0; i < size_t(mWidth * mHeight); i++)
		{
			if (rImage.size())
			{
				combinedImage[4 * i + 1] = rImage[4 * i]; // Green == Roughness
			}
			if (mImage.size())
			{
				combinedImage[4 * i + 2] = mImage[4 * i]; // Blue == Metallic
			}
		}

		CreateTextureHelper(device, context, mWidth, mHeight, combinedImage, DXGI_FORMAT_R8G8B8A8_UNORM, texture, srv);
	}
}

void D3D11Utils::CreateTextureArray(Microsoft::WRL::ComPtr<ID3D11Device>& device,
									Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
									const std::vector<std::string> fileNames,
									Microsoft::WRL::ComPtr<ID3D11Texture2D>& texture,
									Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv)
{
	if (fileNames.empty())
		return;

	int width = 0, height = 0;
	vector<vector<uint8_t>> imageArray;
	for (const auto& f : fileNames)
	{	
		vector<uint8_t> image;
		ReadImage(f, image, width, height);

		imageArray.push_back(image);
	}

	UINT size = UINT(fileNames.size());

	// Create Texture2DArray
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = UINT(width);
	texDesc.Height = UINT(height);
	texDesc.MipLevels = 0;
	texDesc.ArraySize = size; // ArraySize
	texDesc.Format = DXGI_FORMAT_R8G8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	device->CreateTexture2D(&texDesc, NULL, texture.GetAddressOf());

	//texture->GetDesc(&texDesc);

	// Staging texture를 만들어서 하나씩 복사
	for (size_t i = 0; i < imageArray.size(); i++)
	{
		vector<uint8_t>& image = imageArray[i];
		
		ComPtr<ID3D11Texture2D> stagingTexture = CreateStagingTexture(device, context, width, height, image, texDesc.Format, 1, 1);

		UINT subresourceIndex = D3D11CalcSubresource(0, UINT(i), texDesc.MipLevels);

		context->CopySubresourceRegion(texture.Get(), subresourceIndex, 0, 0, 0, 
									   stagingTexture.Get(), 0, NULL);
	}

	device->CreateShaderResourceView(texture.Get(), NULL, srv.GetAddressOf());
	context->GenerateMips(srv.Get());
}

void D3D11Utils::CreateDDSTexture(Microsoft::WRL::ComPtr<ID3D11Device>& device,
								  const wchar_t* fileName,
								  const bool isCubeMap,
								  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv)
{
	ComPtr<ID3D11Texture2D> texture;

	UINT miscFlags = 0;
	if (isCubeMap)
	{
		miscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
	}

	ThrowIfFailed(CreateDDSTextureFromFileEx(device.Get(), fileName, 0,
										     D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE,
											 0, miscFlags, DDS_LOADER_FLAGS(false),
											 (ID3D11Resource**)texture.GetAddressOf(), srv.GetAddressOf(), NULL));
}

void D3D11Utils::WriteToFile(Microsoft::WRL::ComPtr<ID3D11Device>& device,
							 Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
							 Microsoft::WRL::ComPtr<ID3D11Texture2D>& textureToWrite,
							 const std::string fileName)
{
	D3D11_TEXTURE2D_DESC texDesc;
	textureToWrite->GetDesc(&texDesc);
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.BindFlags = 0;
	texDesc.MiscFlags = 0;
	texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	texDesc.Usage = D3D11_USAGE_STAGING; // GPU에서 CPU로 보낼 데이터 임시 보관

	ComPtr<ID3D11Texture2D> stagingTexture;
	if (FAILED(device->CreateTexture2D(&texDesc, NULL, stagingTexture.GetAddressOf())))
	{
		cout << "Failed()\n";
	}

	D3D11_BOX box;
	box.left = 0;
	box.right = texDesc.Width;
	box.top = 0;
	box.bottom = texDesc.Height;
	box.front = 0;
	box.back = 1;
	context->CopySubresourceRegion(stagingTexture.Get(), 0, 0, 0, 0,
								   textureToWrite.Get(), 0, &box);

	// R8G8B8A8이라고 가정
	vector<uint8_t> pixels(texDesc.Width * texDesc.Height * 4);

	D3D11_MAPPED_SUBRESOURCE ms;
	context->Map(stagingTexture.Get(), NULL, D3D11_MAP_READ, NULL, &ms);

	uint8_t* pData = (uint8_t*)ms.pData;
	for (unsigned int h = 0; h < texDesc.Height; h++)
	{
		memcpy(&pixels[h * texDesc.Width * 4], &pData[h * ms.RowPitch],
			   texDesc.Width * sizeof(uint8_t) * 4);
	}

	context->Unmap(stagingTexture.Get(), NULL);

	stbi_write_png(fileName.c_str(), texDesc.Width, texDesc.Height,
				   4, pixels.data(), texDesc.Width * 4);

	cout << fileName << endl;
}
