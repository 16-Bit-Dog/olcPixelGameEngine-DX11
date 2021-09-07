/*TODO
	olcPGEX_DX11_3D.h

	+-------------------------------------------------------------+
	|         OneLoneCoder Pixel Game Engine Extension            |
	|                DX11 3d extension v0.22   	                  |
	+-------------------------------------------------------------+

	What is this?
	~~~~~~~~~~~~~
	This is an extension to the olcPixelGameEngine, which provides
	basic 3d with DX11... YAY - more DX for all! - maybe I'll even add FSR :smile:

	Use of this extension requires the olcPixelGameEngine-DX11-Fork's olcPixelGameEngine header

	It also requires tinyobjloader.h

	License (OLC-3)
	~~~~~~~~~~~~~~~

	Copyright 2018 - 2019 OneLoneCoder.com

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:

	1. Redistributions or derivations of source code must retain the above
	copyright notice, this list of conditions and the following disclaimer.

	2. Redistributions or derivative works in binary form must reproduce
	the above copyright notice. This list of conditions and the following
	disclaimer must be reproduced in the documentation and/or other
	materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its
	contributors may be used to endorse or promote products derived
	from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Links
	~~~~~
	https://github.com/16-Bit-Dog

	Author
	~~~~~~
	16_Bit_Dog

*/
// z axis is x axis in size for pge math
//everything is radians, no angles - convert to them if you want!
//
// TODO: cam lerp rotation, and model lerp (more generic than cam lerp)
//  
//draw types for now: reg '2d' 3d and reg 3d 
//TODO: Move cam as if 0 (or set) rotation option [move as in perspective]
//TODO: billboard plane for 2d but acts like 3d
//TODO: particle systems
//TODO: lerp function for cam rot && pos, model rot && pos

#pragma once

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#if defined(OLC_PGEX_DIRECTX11_3D)

namespace DOLC11 {
	
	
	float ToNotPGESpace(float* InvScreenSize, float* val) {
		return (*val) * (*InvScreenSize) * 2.0f; //returns to regular not PGE space
	}
	float ToPGESpace(float* InvScreenSize, float* val) {
		return (*val) / (*InvScreenSize) / 2.0f; //returns pixel space
	}


	struct DataDrawOrderAndFunc {
		//place holder struct incase things get more complex
		std::function<void()> func;

	};

	struct DataLerpFunc { //end position
		//place holder struct incase things get more complex
		std::function<void()> func;
		float X = 0.0f;
		bool useX = false;

		float Y = 0.0f;
		bool useY = false;

		float Z = 0.0f;
		bool useZ = false; 
		
		float MaxTime = 0.0f;
		float CurTime = 0.0f; 

		DataLerpFunc* me() {
			return this;
		}
	};


	std::vector< DataDrawOrderAndFunc > DrawOrder;
	std::vector< DataDrawOrderAndFunc > DrawOrderBefore;

	std::vector< DataLerpFunc > LerpCamPosFunc;
	std::vector< DataLerpFunc > LerpCamRotFunc;

	std::vector< DataLerpFunc > LerpModelCamPosFunc;
	std::vector< DataLerpFunc > LerpModelCamRotFunc;
	
	class ProgramLink_3D_DX : public olc::PGEX {
	public:

		void Initialize3DShadersPL(int LayersToUse = 0);

		ProgramLink_3D_DX()
		{

		}

		int32_t ScreenWidth() { return pge->ScreenWidth(); };
		int32_t ScreenHeight() { return pge->ScreenHeight(); };

		void DrawFuncMain();

		bool IniSAndB = false;

		int currentLayer = 0;

		std::function<void()> DrawerHandle = [&] { DrawFuncMain(); };

		float LastSec() { return pge->GetElapsedTime(); };

	}PL;

	struct VNT
	{
		XMFLOAT3 Position;
		XMFLOAT3 Normal;
		XMFLOAT2 Tex;
	};

	struct ObjTuneStatReg { //TODO: make scale and translate work in shader - make obj visible
		std::array<float, 3> Translate = { 0.0f,0.0f,0.0f }; //not using xm float, so sad... :(
		float pad1 = 0.0f;
		std::array<float, 3> Scale = { 1.0f,1.0f,1.0f };
		float pad2 = 0.0f;
		XMFLOAT4 Quat = { 0.0f,0.0f,0.0f,0.0f };
		
		//XMFLOAT4 pad2 = {0.0f,0.0f,0.0f,0.0f};
	};

	struct M3DR { //3d model with all data - I need seperate obj loader - regular model format

		ObjTuneStatReg ObjTune;

		ID3D11Buffer* CBuf;

		std::array<float, 3> Translate() {
			olc::vf2d vInvScreenSize = {
		(1.0f / float(PL.ScreenWidth())),
		(1.0f / float(PL.ScreenHeight()))
			};
			return std::array<float, 3> { ToPGESpace(&vInvScreenSize.x, &ObjTune.Translate[0]), ToPGESpace(&vInvScreenSize.y, &ObjTune.Translate[1]), ToPGESpace(&vInvScreenSize.x, &ObjTune.Translate[2]) };
		}

		std::array<float, 3> Scale() {
			return ObjTune.Scale;
		}
		std::array<float, 4> Quaternion() {
			return std::array<float, 4> {ObjTune.Quat.x, ObjTune.Quat.y, ObjTune.Quat.z, ObjTune.Quat.w};
		}
		std::array<float, 3> Radians() { //ObjTune.Quat
			/*
			XMVECTOR V = {};
			
			XMVECTOR V2 = {0,0,0};
			
			XMStoreFloat4(&ObjTune.Quat, V);

			float x; XMQuaternionToAxisAngle(&V2, &x, V);
			float y; XMQuaternionToAxisAngle(&V2, &y, V);
			float z; XMQuaternionToAxisAngle(&V2, &z, V);
			
			return std::array<float, 3> {x, y, z};
			*/

			float quatDiv = sqrt(1 - ObjTune.Quat.w * ObjTune.Quat.w);
			return std::array<float, 3> {(ObjTune.Quat.x / quatDiv), (ObjTune.Quat.y / quatDiv), (ObjTune.Quat.z / quatDiv)};
		}


		void DefaultCBuf() {
			D3D11_BUFFER_DESC bufDesc;
			ZeroMemory(&bufDesc, sizeof(bufDesc));
			bufDesc.Usage = D3D11_USAGE_DEFAULT;
			bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufDesc.CPUAccessFlags = 0;
			bufDesc.ByteWidth = sizeof(ObjTuneStatReg);

			dxDevice->CreateBuffer(&bufDesc, nullptr, &CBuf);

			dxDeviceContext->UpdateSubresource(CBuf, 0, nullptr, &ObjTune, 0, 0);
		}

		std::vector<VNT> modelDat;

		std::vector<UINT> Indice;

		ID3D11Buffer* VBuf = NULL;
		ID3D11UnorderedAccessView* VBufUAV = NULL;

		ID3D11Buffer* IBuf = NULL;

		//reflectivity? for light?
		//Vertex buffer, indice buffer,

		ID3D11ShaderResourceView* Tex1SRV = NULL; //SRV
		ID3D11Texture2D* Tex1R = NULL; // SRV data - kinda redundant due to ->GetResource()
		
		ID3D11SamplerState* Sampler = NULL;
		
		ID3D11BlendState* BlendState = NULL;
		//ID3D11UnorderedAccessView* Tex1UAV;

		void SetupTexLinkResource() {

			D3D11_SHADER_RESOURCE_VIEW_DESC SRVdesc;
			SRVdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			SRVdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			SRVdesc.Buffer.FirstElement = 0;
			SRVdesc.Buffer.NumElements = 1;
			SRVdesc.Texture2D.MostDetailedMip = 0;
			SRVdesc.Texture2D.MipLevels = 1;

			dxDevice->CreateShaderResourceView(Tex1R, &SRVdesc, &Tex1SRV); //seperate

		//	dxDevice->CreateUnorderedAccessView(gpuTex, &UAVdesc, &UAV);

		}

		void SetupBlendStateDefault() {
			D3D11_BLEND_DESC blendVal;

			blendVal.AlphaToCoverageEnable = false;
			blendVal.IndependentBlendEnable = false;
			blendVal.RenderTarget[0].BlendEnable = true;
			blendVal.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blendVal.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blendVal.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendVal.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendVal.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blendVal.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendVal.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			
			SafeRelease(BlendState);
			dxDevice->CreateBlendState(&blendVal, &BlendState);
		}

		//TODO: options in blend state

		void CreateSampler(bool LinearTOrPoint = true, bool ClampTOrMirror = true){
		
			if(Sampler != nullptr)
			SafeRelease(Sampler);

			D3D11_SAMPLER_DESC tmpSampleDesc;

			tmpSampleDesc.Filter = D3D11_FILTER{ D3D11_FILTER_ANISOTROPIC };
			tmpSampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE{ D3D11_TEXTURE_ADDRESS_WRAP };
			tmpSampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE{ D3D11_TEXTURE_ADDRESS_WRAP };
			tmpSampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE{ D3D11_TEXTURE_ADDRESS_WRAP };
			tmpSampleDesc.MipLODBias = 0;
			tmpSampleDesc.MaxAnisotropy = 8;
			tmpSampleDesc.ComparisonFunc = D3D11_COMPARISON_FUNC{ D3D11_COMPARISON_NEVER }; //never remove model for now - always pass render pass
			tmpSampleDesc.MinLOD = 1;
			tmpSampleDesc.MaxLOD = D3D11_FLOAT32_MAX;

			if (LinearTOrPoint) //don't give an option for mip linear filter for now - testing 
			{
				tmpSampleDesc.Filter = D3D11_FILTER{ D3D11_FILTER_MIN_MAG_MIP_LINEAR };
			}
			else
			{
				tmpSampleDesc.Filter = D3D11_FILTER{ D3D11_FILTER_MIN_MAG_MIP_POINT };
			}

			if (ClampTOrMirror) //no option for mirror tex as well
			{
				tmpSampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE{ D3D11_TEXTURE_ADDRESS_CLAMP };
				tmpSampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE{ D3D11_TEXTURE_ADDRESS_CLAMP };
				tmpSampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE{ D3D11_TEXTURE_ADDRESS_CLAMP };
			}
			else
			{
				tmpSampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE{ D3D11_TEXTURE_ADDRESS_MIRROR };
				tmpSampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE{ D3D11_TEXTURE_ADDRESS_MIRROR };
				tmpSampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE{ D3D11_TEXTURE_ADDRESS_MIRROR };
			}

			dxDevice->CreateSamplerState(&tmpSampleDesc, &Sampler);

		}

		void SetTexCopy(olc::Decal* Tex, bool LinearTOrPoint = true, bool ClampTOrMirror = true) {
			CreateSampler(LinearTOrPoint, ClampTOrMirror);
			
			if(Tex1R != NULL)
			SafeRelease(Tex1R);

			dxDeviceContext->CopyResource(Tex1R, olc::DecalTSR[Tex->id]);

			SetupTexLinkResource();
		}
		void SetTexCopy(olc::Sprite* Tex, bool LinearTOrPoint = true, bool ClampTOrMirror = true) {
			CreateSampler(LinearTOrPoint, ClampTOrMirror);

			if (Tex1R != NULL)
			SafeRelease(Tex1R);

			D3D11_TEXTURE2D_DESC gpuTexDescS;
			ZeroMemory(&gpuTexDescS, sizeof(gpuTexDescS));
			gpuTexDescS.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			gpuTexDescS.Width = Tex->width;
			gpuTexDescS.Height = Tex->height;
			gpuTexDescS.MipLevels = 1;
			gpuTexDescS.ArraySize = 1;
			gpuTexDescS.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			gpuTexDescS.SampleDesc.Count = 1;
			gpuTexDescS.SampleDesc.Quality = 0;
			gpuTexDescS.MiscFlags = 0;
			gpuTexDescS.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			gpuTexDescS.Usage = D3D11_USAGE_DYNAMIC;

			D3D11_SHADER_RESOURCE_VIEW_DESC SRVdesc;
			SRVdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			SRVdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			SRVdesc.Buffer.FirstElement = 0;
			SRVdesc.Buffer.NumElements = 1;
			SRVdesc.Texture2D.MostDetailedMip = 0;
			SRVdesc.Texture2D.MipLevels = 1;

			dxDevice->CreateTexture2D(&gpuTexDescS, NULL, &Tex1R);

			dxDevice->CreateShaderResourceView(Tex1R, &SRVdesc, &Tex1SRV);

			D3D11_MAPPED_SUBRESOURCE resource;

			dxDeviceContext->Map(Tex1R, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
			BYTE* mappedData = reinterpret_cast<BYTE*>(resource.pData);
			BYTE* buffer = reinterpret_cast<BYTE*>(Tex->pColData.data());

			for (int i = 0; i < Tex->height; i++) {
				memcpy(mappedData, buffer, Tex->width * sizeof(olc::Pixel));
				mappedData += resource.RowPitch;

				buffer += Tex->width * sizeof(olc::Pixel);
			}

			dxDeviceContext->Unmap(Tex1R, 0);
		}

		void LoadVertexIndiceData() //also reloads vertex data
		{
		//	if (VBuf != nullptr) {
				SafeRelease(VBuf);
		//	}
		//	if (IBuf != nullptr){
				SafeRelease(IBuf);
		//	}

			D3D11_BUFFER_DESC vertexBufferDesc; //describe buffer we will make
			ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

			vertexBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_VERTEX_BUFFER; //how to bind buffer 

			vertexBufferDesc.ByteWidth = sizeof(VNT) * (modelDat.size()); //size of buffer --> make it the size of verticies*vertexPosColor [since vertex will have pos and color
			vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // 0 means no CPU acsess

			vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC; //resource flag - 0 means none
			vertexBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;


			D3D11_SUBRESOURCE_DATA resourceData; //data for buffer
			ZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));
			resourceData.pSysMem = &modelDat[0]; //Vertex data for sub source

			dxDevice->CreateBuffer(&vertexBufferDesc, &resourceData, &VBuf); //create buffer, using data settings struct, struct of data, and vertex buffer output - this is also used to create other buffer styles

			D3D11_BUFFER_DESC a;
			VBuf->GetDesc(&a);

			ID3D11Buffer* tmpVertex;

			D3D11_BUFFER_DESC vertexBufferDescU; //describe buffer we will make
			ZeroMemory(&vertexBufferDescU, sizeof(D3D11_BUFFER_DESC));

			vertexBufferDescU.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE; //how to bind buffer 

			vertexBufferDescU.ByteWidth = sizeof(VNT) * (modelDat.size()); //size of buffer --> make it the size of verticies*vertexPosColor [since vertex will have pos and color
			vertexBufferDescU.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ; // 0 means no CPU acsess

			vertexBufferDescU.Usage = D3D11_USAGE_DEFAULT; //resource flag - 0 means none

			vertexBufferDescU.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			vertexBufferDescU.StructureByteStride = sizeof(VNT);

			resourceData.pSysMem = &modelDat[0]; //Vertex data pos for sub source - use Position?

			dxDevice->CreateBuffer(&vertexBufferDescU, &resourceData, &tmpVertex);

			D3D11_UNORDERED_ACCESS_VIEW_DESC UAVdesc;
			//DXGI_FORMAT_R32_TYPELESS
			UAVdesc.Format = DXGI_FORMAT_UNKNOWN;
			UAVdesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			UAVdesc.Buffer.FirstElement = 0;
			UAVdesc.Buffer.NumElements = modelDat.size();
			UAVdesc.Buffer.Flags = 0;

			dxDevice->CreateUnorderedAccessView(tmpVertex, &UAVdesc, &VBufUAV);

			D3D11_BUFFER_DESC indexBufferDesc; //buffer obj
			ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC)); //alloc

			indexBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_INDEX_BUFFER; //type of buffer m8 - same logic as vertex
			indexBufferDesc.ByteWidth = sizeof(UINT) * (Indice.size());
			indexBufferDesc.CPUAccessFlags = 0;
			indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			indexBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

			resourceData.pSysMem = &Indice[0]; //indice data for sub source

			dxDevice->CreateBuffer(&indexBufferDesc, &resourceData, &IBuf); //make buffer
		}

		void LoadOBJFile(std::string path)
		{
			tinyobj::attrib_t attrib;
			std::vector<tinyobj::shape_t> shapes;
			std::vector<tinyobj::material_t> materials;
			std::string warn, err;

			if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) {
				throw std::runtime_error(warn + err);
			}

			modelDat.empty();
			Indice.empty();

			int i = 0;

			VNT tmpV;

			std::map<std::tuple<float, float, float>, int> b;

			for (const auto& shape : shapes) { // combine all faces into 1 model --> they are normally "seperate"
				i = 0;
				for (const auto& index : shape.mesh.indices) { //iterate through each indice in mesh


					tmpV.Position = {
						attrib.vertices[3 * index.vertex_index + 0], //since these are floats I multiply by 3?
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2] // move obj pos
					};

					tmpV.Tex = { // flip image vertically to fix model texture
						attrib.texcoords[2 * index.texcoord_index + 0], // this vector does not work if I do not set texcoods when making the obj
						attrib.texcoords[2 * index.texcoord_index + 1]
					};

					tmpV.Normal = { 0,0,0};



					if (b.count((std::make_tuple(tmpV.Position.x, tmpV.Position.y, tmpV.Position.z))) == 0) { //filter out duplicate verticies
						b[std::make_tuple(tmpV.Position.x, tmpV.Position.y, tmpV.Position.z)] = modelDat.size();
						modelDat.push_back(tmpV);
						//i++;
					}
					Indice.push_back(b[std::make_tuple(tmpV.Position.x, tmpV.Position.y, tmpV.Position.z)]);
				}
			}
		}

		void LoadOBJFileWithVertex(std::string path) {
			LoadOBJFile(path);
			LoadVertexIndiceData();
		}
		//TODO: , make translate in pixels for x, y - and z is x depth for pixels

		void MSRObject(std::array<float, 3> XYZTranslate = { 0.0f,0.0f,0.0f }, std::array<float, 3> scale = { 1.0f,1.0f,1.0f }, std::array<float, 3> rotateXYZaxis = { 0.0f,0.0f,0.0f }) { // pass in OG values to not change the ones not wanted to modify
			
			olc::vf2d vInvScreenSize = {
		(1.0f / float(PL.ScreenWidth())),
		(1.0f / float(PL.ScreenHeight()))
			};

			ObjTune.Translate = std::array<float, 3>{ToNotPGESpace(&vInvScreenSize.x, &XYZTranslate[0]), ToNotPGESpace(&vInvScreenSize.y, &XYZTranslate[1]), ToNotPGESpace(&vInvScreenSize.x, &XYZTranslate[2]) };

			ObjTune.Scale = scale;

			XMStoreFloat4(&ObjTune.Quat, XMQuaternionRotationRollPitchYaw(rotateXYZaxis[0], rotateXYZaxis[1], rotateXYZaxis[2]) );

			dxDeviceContext->UpdateSubresource(CBuf, 0, nullptr, &ObjTune, 0, 0);
		}

		M3DR(olc::Decal* Tex, std::string path = "", bool LinearTOrPoint = true, bool ClampTOrMirror = true) {
			//SetupTexLinkResource();
			SetTexCopy(Tex, LinearTOrPoint, ClampTOrMirror);
			SetupBlendStateDefault();
			if (path != "") {
				LoadOBJFileWithVertex(path);
			}
			DefaultCBuf();
		}
		M3DR(olc::Sprite* Tex, std::string path = "", bool LinearTOrPoint = true, bool ClampTOrMirror = true) {
			//SetupTexLinkResource();
			SetTexCopy(Tex, LinearTOrPoint, ClampTOrMirror);
			SetupBlendStateDefault();
			if (path != "") {
				LoadOBJFileWithVertex(path);
			}
			DefaultCBuf();
		}



	};


	

	struct ShaderCollection { // I got lazy typing public: to a class... why not a class - those programming books and their public: classes... just use structs next time...

		ID3D11VertexShader* BMSVs;
		ID3D11PixelShader* BMSPs;
		ID3D11InputLayout* BMSIl;

		ID3D11VertexShader* CreateShaderStaticM(ID3DBlob* pShaderBlob, ID3D11ClassLinkage* pClassLinkage) //vertex shader - shader type
		{
			ID3D11VertexShader* pVertexShader = nullptr;
			dxDevice->CreateVertexShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), pClassLinkage, &pVertexShader); //make a shader based on buffer, buffer size, classtype, and return to pshader object

			D3D11_INPUT_ELEMENT_DESC dxVertexLayoutDesc[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

			};

			HRESULT hr = dxDevice->CreateInputLayout( //make input layout - global change to input Layout
				dxVertexLayoutDesc, //vertex shader - input assembler data
				_countof(dxVertexLayoutDesc), //number of elements
				pShaderBlob->GetBufferPointer(),  //vertex shader buffer
				pShaderBlob->GetBufferSize(), //vetex shader blob size 
				&BMSIl); //input layout to output to

			if (FAILED(hr))
			{
				OutputDebugStringW(L"failed input layout setup");
			}
			return pVertexShader;
		}

		ID3D11VertexShader* LoadShaderStaticM(const std::string* shaderInfo, const std::string& entryPoint, const std::string& _profile) {

			ID3DBlob* pShaderBlob = nullptr;
			ID3DBlob* pErrorBlob = nullptr;
			ID3D11VertexShader* pShader = nullptr;

			std::string profile = _profile;
			if (profile == "latest")
			{
				profile = GetLatestProfile<ID3D11VertexShader>(); //get shader profiles/settings
			}

			UINT flags = D3DCOMPILE_OPTIMIZATION_LEVEL3;

#if _DEBUG
			flags |= D3DCOMPILE_DEBUG;
#endif
			HRESULT hr = D3DCompile2(shaderInfo->c_str(), shaderInfo->length(), nullptr,
				nullptr, nullptr, entryPoint.c_str(),
				profile.c_str(), flags, 0, 0, 0, 0, &pShaderBlob, &pErrorBlob);
			OutputDebugStringA("\n");
			if (pErrorBlob != nullptr) {
				OutputDebugStringA((const char*)pErrorBlob->GetBufferPointer());
			}

			pShader = CreateShaderStaticM(pShaderBlob, nullptr);

			SafeRelease(pShaderBlob); // no longer need shader mem
			SafeRelease(pErrorBlob); // no longer need shader mem <-- I frogot to safe release to delete and do other stuff... so I need to look back at that

			return pShader;

		}

		void CreateStaticModelShader() {


			const std::string TestPS = std::string(
				"Texture2D shaderTexture : register(t0);\n"
				"SamplerState SampleType : register(s0);\n"
				"struct PixelShaderInput{\n"
				"float4 position : SV_POSITION;\n"
				"float3 normal: NORMAL;\n"
				"float4 color: COLOR;\n"
				"float2 tex : TEXCOORD0;\n"
				"float4 PositionWS : TEXCOORD1;"
				"};\n"
				"float4 SimplePS(PixelShaderInput IN) : SV_TARGET{\n"
				"float4 textureColor = shaderTexture.Sample(SampleType, IN.tex);\n"
				//"textureColor.r *= IN.color.r/255;\n"
				//"textureColor.g *= IN.color.g/255;\n"
				//"textureColor.b *= IN.color.b/255;\n"
				//"textureColor.w *= IN.color.w/255;\n"
				"return textureColor;}");


			const std::string TestVS = std::string(
				"cbuffer MyObjD : register(b6){\n"
				"float3 Translate;\n"
				"float pad1 = 0.0f;\n"
				"float3 Scale;\n"
				"float pad2;\n"
				"float4 Quat;\n"
				"}\n"

				"cbuffer PerApplication : register(b0){\n"
				"matrix projectionMatrix;}\n"

				"cbuffer PerFrame : register(b1){\n"
				"matrix viewMatrix;}\n"
				
				"cbuffer PerObject : register(b2){\n"
				"matrix worldMatrix;}\n"
				
				"struct AppData{\n"
				"float3 position : POSITION;\n"
				"float3 normal : NORMAL;\n"
				"float4 color: COLOR;\n"
				"float2 tex : TEXCOORD;\n"
				"};\n"
				
				"struct VertexShaderOutput{\n"
				"float4 position : SV_POSITION;\n"
				"float3 normal: NORMAL;\n"
				"float4 color: COLOR;\n"
				"float2 tex : TEXCOORD0;\n"
				"float4 PositionWS : TEXCOORD1;};\n"
				
				"float3 QuatRotate(float3 pos, float4 quat){\n"
				"return pos + 2.0 * cross(quat.xyz, cross(quat.xyz, pos) + quat.w * pos);\n"
				"}\n"

				"VertexShaderOutput SimpleVS(AppData IN){\n"
				"VertexShaderOutput OUT;\n"
				"float3 posTMP = ( QuatRotate(IN.position, Quat)*Scale)+Translate;"

				"matrix mvp = mul(projectionMatrix, mul(viewMatrix, worldMatrix));\n"
				"OUT.position = mul(mvp,float4(posTMP,1));\n"
				"OUT.normal = mul(mvp, IN.normal);\n" //mul(mvp, float4(IN.normal, 1.0f));
				"OUT.normal = normalize(OUT.normal);\n"
				"OUT.PositionWS = mul(worldMatrix, float4(posTMP, 1.0f));\n"
				"OUT.tex = IN.tex;\n"
				"OUT.color = IN.color;\n"
				"return OUT;}");

			BMSVs = LoadShaderStaticM(&TestVS, "SimpleVS", "latest");

			BMSPs = LoadShader<ID3D11PixelShader>(&TestPS, "SimplePS", "latest");
			
		}

		ID3D11VertexShader* BMS2dVs;
		ID3D11PixelShader* BMS2dPs;
		ID3D11InputLayout* BMS2dIl;

		ID3D11VertexShader* CreateShaderStaticM2d(ID3DBlob* pShaderBlob, ID3D11ClassLinkage* pClassLinkage) //vertex shader - shader type
		{
			ID3D11VertexShader* pVertexShader = nullptr;
			dxDevice->CreateVertexShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), pClassLinkage, &pVertexShader); //make a shader based on buffer, buffer size, classtype, and return to pshader object

			D3D11_INPUT_ELEMENT_DESC dxVertexLayoutDesc[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

			};

			HRESULT hr = dxDevice->CreateInputLayout( //make input layout - global change to input Layout
				dxVertexLayoutDesc, //vertex shader - input assembler data
				_countof(dxVertexLayoutDesc), //number of elements
				pShaderBlob->GetBufferPointer(),  //vertex shader buffer
				pShaderBlob->GetBufferSize(), //vetex shader blob size 
				&BMSIl); //input layout to output to

			if (FAILED(hr))
			{
				OutputDebugStringW(L"failed input layout setup");
			}
			return pVertexShader;
		}

		ID3D11VertexShader* LoadShaderStaticM2d(const std::string* shaderInfo, const std::string& entryPoint, const std::string& _profile) {

			ID3DBlob* pShaderBlob = nullptr;
			ID3DBlob* pErrorBlob = nullptr;
			ID3D11VertexShader* pShader = nullptr;

			std::string profile = _profile;
			if (profile == "latest")
			{
				profile = GetLatestProfile<ID3D11VertexShader>(); //get shader profiles/settings
			}

			UINT flags = D3DCOMPILE_OPTIMIZATION_LEVEL3;

#if _DEBUG
			flags |= D3DCOMPILE_DEBUG;
#endif
			HRESULT hr = D3DCompile2(shaderInfo->c_str(), shaderInfo->length(), nullptr,
				nullptr, nullptr, entryPoint.c_str(),
				profile.c_str(), flags, 0, 0, 0, 0, &pShaderBlob, &pErrorBlob);
			OutputDebugStringA("\n");
			if (pErrorBlob != nullptr) {
				OutputDebugStringA((const char*)pErrorBlob->GetBufferPointer());
			}

			pShader = CreateShaderStaticM(pShaderBlob, nullptr);

			SafeRelease(pShaderBlob); // no longer need shader mem
			SafeRelease(pErrorBlob); // no longer need shader mem <-- I frogot to safe release to delete and do other stuff... so I need to look back at that

			return pShader;

		}

		void CreateStaticModel2dShader() {


			const std::string TestPS = std::string(
				"Texture2D shaderTexture : register(t0);\n"
				"SamplerState SampleType : register(s0);\n"
				"struct PixelShaderInput{\n"
				"float4 position : SV_POSITION;\n"
				"float3 normal: NORMAL;\n"
				"float4 color: COLOR;\n"
				"float2 tex : TEXCOORD0;\n"
				"float4 PositionWS : TEXCOORD1;"
				"};\n"
				"float4 SimplePS(PixelShaderInput IN) : SV_TARGET{\n"
				"float4 textureColor = shaderTexture.Sample(SampleType, IN.tex);\n"
				//"textureColor.r *= IN.color.r/255;\n"
				//"textureColor.g *= IN.color.g/255;\n"
				//"textureColor.b *= IN.color.b/255;\n"
				//"textureColor.w *= IN.color.w/255;\n"
				"return textureColor;}");


			const std::string TestVS = std::string(
				"cbuffer MyObjD : register(b6){\n"
				"float3 Translate;\n"
				"float pad1 = 0.0f;\n"
				"float3 Scale;\n"
				"float pad2;\n"
				"float4 Quat;\n"
				"}\n"

				"cbuffer PerApplication : register(b0){\n"
				"matrix projectionMatrix;}\n"

				"cbuffer PerFrame : register(b1){\n"
				"matrix viewMatrix;}\n"

				"cbuffer PerObject : register(b2){\n"
				"matrix worldMatrix;}\n"

				"struct AppData{\n"
				"float3 position : POSITION;\n"
				"float3 normal : NORMAL;\n"
				"float4 color: COLOR;\n"
				"float2 tex : TEXCOORD;\n"
				"};\n"

				"struct VertexShaderOutput{\n"
				"float4 position : SV_POSITION;\n"
				"float3 normal: NORMAL;\n"
				"float4 color: COLOR;\n"
				"float2 tex : TEXCOORD0;\n"
				"float4 PositionWS : TEXCOORD1;};\n"

				"float3 QuatRotate(float3 pos, float4 quat){\n"
				"return pos + 2.0 * cross(quat.xyz, cross(quat.xyz, pos) + quat.w * pos);\n"
				"}\n"

				"VertexShaderOutput SimpleVS(AppData IN){\n"
				"VertexShaderOutput OUT;\n"
				"float3 posTMP = ( QuatRotate(IN.position, Quat)*Scale)+Translate;"

				"matrix mvp = mul(projectionMatrix, mul(viewMatrix, worldMatrix));\n"
				"OUT.position = float4(posTMP,1);\n"
				"OUT.normal = mul(mvp, IN.normal);\n" //mul(mvp, float4(IN.normal, 1.0f));
				"OUT.normal = normalize(OUT.normal);\n"
				"OUT.PositionWS = mul(worldMatrix, float4(posTMP, 1.0f));\n"
				"OUT.tex = IN.tex;\n"
				"OUT.color = IN.color;\n"
				"return OUT;}");

			BMS2dVs = LoadShaderStaticM(&TestVS, "SimpleVS", "latest");

			BMS2dPs = LoadShader<ID3D11PixelShader>(&TestPS, "SimplePS", "latest");

		}

		void PostCreate() {

			CreateStaticModelShader();
			CreateStaticModel2dShader();
		}

	}ShaderData;
	
	void Initialize3DMode(olc::PixelGameEngine* pge)
	{
		pge->pgex_Register(&PL);

	}

	void Initialize3DShaders(int LayersToUse = 0) {
		PL.Initialize3DShadersPL(LayersToUse);
	}

//#include <iostream>

	void ProgramLink_3D_DX::Initialize3DShadersPL(int LayersToUse) {
		if (IniSAndB == false) {
			IniSAndB = true;

			currentLayer = LayersToUse;

			if (LayersToUse < pge->GetLayers().size() && pge->GetLayers()[LayersToUse].bShow == true) {

			}
			else {
				currentLayer = 0;
			}
			
			pge->SetLayerCustomRenderFunction(currentLayer, DrawerHandle);

			ShaderData.PostCreate();

		}
	}
	struct ModelDrawFuncs {

		std::vector<ID3D11Buffer**> CBufTmp;

		void DrawM(M3DR* Model, bool usingTmps = false, std::array<float,3> XYZtmpTranslate = {0.0f,0.0f,0.0f}, std::array<float, 3> tmpScale = { 1.0f,1.0f,1.0f }, std::array<float, 3> TmpRotateXYZaxis = { 0.0f,0.0f,0.0f }) {
			
			
			ID3D11Buffer* CBufTmpOne;

			ObjTuneStatReg ObjTuneTmp;

			if (usingTmps == true) {
				
				
				ObjTuneTmp.Scale = tmpScale;
				ObjTuneTmp.Translate = XYZtmpTranslate;
				XMStoreFloat4(&ObjTuneTmp.Quat, XMQuaternionRotationRollPitchYaw(TmpRotateXYZaxis[0], TmpRotateXYZaxis[1], TmpRotateXYZaxis[2]));

				D3D11_BUFFER_DESC bufDesc;
				ZeroMemory(&bufDesc, sizeof(bufDesc));
				bufDesc.Usage = D3D11_USAGE_DEFAULT;
				bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				bufDesc.CPUAccessFlags = 0;
				bufDesc.ByteWidth = sizeof(ObjTuneStatReg);
				dxDevice->CreateBuffer(&bufDesc, nullptr, &CBufTmpOne);

				dxDeviceContext->UpdateSubresource(CBufTmpOne, 0, nullptr, &ObjTuneTmp, 0, 0);

				CBufTmp.push_back(&CBufTmpOne);
			}

			const UINT vertexStride = sizeof(VNT);
			const UINT offset = 0;

			dxDeviceContext->IASetVertexBuffers(0, 1, &Model->VBuf, &vertexStride, &offset);
			dxDeviceContext->IASetIndexBuffer(
				Model->IBuf,
				DXGI_FORMAT_R32_UINT,
				0);

			dxDeviceContext->VSSetConstantBuffers( //in case no decals were drawn I need to fill const buf with the proper matrix's
				0,
				3,
				dxConstantBuffers
			);

			if (usingTmps == false) {
				dxDeviceContext->VSSetConstantBuffers(6, 1, &Model->CBuf);
			}
			else {
				dxDeviceContext->VSSetConstantBuffers(6, 1, &CBufTmpOne);
			}
			
			dxDeviceContext->IASetInputLayout(
				ShaderData.BMSIl);

			dxDeviceContext->IASetPrimitiveTopology(
				D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ


			dxDeviceContext->VSSetShader(
				ShaderData.BMSVs,
				nullptr,
				0);


			dxDeviceContext->RSSetState(dxRasterizerStateF);

			float bState[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			dxDeviceContext->OMSetBlendState(Model->BlendState, bState, 0xffffffff);

			dxDeviceContext->OMSetDepthStencilState(dxDepthStencilStateDefault, 1);

			dxDeviceContext->PSSetShader(
				ShaderData.BMSPs,
				nullptr,
				0);


			dxDeviceContext->PSSetShaderResources(0, 1, &Model->Tex1SRV);

			dxDeviceContext->PSSetSamplers(0, 1, &Model->Sampler);

			dxDeviceContext->DrawIndexed(
				Model->Indice.size(),
				0,
				0);

		}
		void DrawM2D(M3DR* Model, bool usingTmps = false, std::array<float, 3> XYZtmpTranslate = { 0.0f,0.0f,0.0f }, std::array<float, 3> tmpScale = { 1.0f,1.0f,1.0f }, std::array<float, 3> TmpRotateXYZaxis = { 0.0f,0.0f,0.0f }) {


			ID3D11Buffer* CBufTmpOne;

			ObjTuneStatReg ObjTuneTmp;

			if (usingTmps == true) {


				ObjTuneTmp.Scale = tmpScale;
				ObjTuneTmp.Translate = XYZtmpTranslate;
				XMStoreFloat4(&ObjTuneTmp.Quat, XMQuaternionRotationRollPitchYaw(TmpRotateXYZaxis[0], TmpRotateXYZaxis[1], TmpRotateXYZaxis[2]));

				D3D11_BUFFER_DESC bufDesc;
				ZeroMemory(&bufDesc, sizeof(bufDesc));
				bufDesc.Usage = D3D11_USAGE_DEFAULT;
				bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				bufDesc.CPUAccessFlags = 0;
				bufDesc.ByteWidth = sizeof(ObjTuneStatReg);
				dxDevice->CreateBuffer(&bufDesc, nullptr, &CBufTmpOne);

				dxDeviceContext->UpdateSubresource(CBufTmpOne, 0, nullptr, &ObjTuneTmp, 0, 0);

				CBufTmp.push_back(&CBufTmpOne);
			}

			const UINT vertexStride = sizeof(VNT);
			const UINT offset = 0;

			dxDeviceContext->IASetVertexBuffers(0, 1, &Model->VBuf, &vertexStride, &offset);
			dxDeviceContext->IASetIndexBuffer(
				Model->IBuf,
				DXGI_FORMAT_R32_UINT,
				0);

			dxDeviceContext->VSSetConstantBuffers( //in case no decals were drawn I need to fill const buf with the proper matrix's
				0,
				3,
				dxConstantBuffers
			);

			if (usingTmps == false) {
				dxDeviceContext->VSSetConstantBuffers(6, 1, &Model->CBuf);
			}
			else {
				dxDeviceContext->VSSetConstantBuffers(6, 1, &CBufTmpOne);
			}

			dxDeviceContext->IASetInputLayout(
				ShaderData.BMSIl);

			dxDeviceContext->IASetPrimitiveTopology(
				D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ


			dxDeviceContext->VSSetShader(
				ShaderData.BMSVs,
				nullptr,
				0);


			dxDeviceContext->RSSetState(dxRasterizerStateF);

			float bState[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			dxDeviceContext->OMSetBlendState(Model->BlendState, bState, 0xffffffff);

			dxDeviceContext->OMSetDepthStencilState(dxDepthStencilStateDefault, 1);

			dxDeviceContext->PSSetShader(
				ShaderData.BMSPs,
				nullptr,
				0);


			dxDeviceContext->PSSetShaderResources(0, 1, &Model->Tex1SRV);

			dxDeviceContext->PSSetSamplers(0, 1, &Model->Sampler);

			dxDeviceContext->DrawIndexed(
				Model->Indice.size(),
				0,
				0);

		}

	}MDFs;

	void DrawM(M3DR* Model, bool before = false, bool usingTmps = false, std::array<float, 3> XYZtmpTranslate = { 0.0f,0.0f,0.0f }, std::array<float, 3> tmpScale = { 1.0f,1.0f,1.0f }, std::array<float, 3> rotateXYZaxis = { 0.0f,0.0f,0.0f }) {
		DataDrawOrderAndFunc tmp;

		tmp.func = [=]() {MDFs.DrawM(Model, usingTmps, XYZtmpTranslate, tmpScale, rotateXYZaxis); };

		if (before == false) {
			DrawOrder.push_back(tmp);
		}
		else {
			DrawOrderBefore.push_back(tmp);
		}

	}

	void DrawM2D(M3DR* Model, bool before = false, bool usingTmps = false, std::array<float, 3> XYZtmpTranslate = { 0.0f,0.0f,0.0f }, std::array<float, 3> tmpScale = { 1.0f,1.0f,1.0f }, std::array<float, 3> rotateXYZaxis = { 0.0f,0.0f,0.0f }) {
		DataDrawOrderAndFunc tmp;

		tmp.func = [=]() {MDFs.DrawM2D(Model, usingTmps, XYZtmpTranslate, tmpScale, rotateXYZaxis); };

		if (before == false) {
			DrawOrder.push_back(tmp);
		}
		else {
			DrawOrderBefore.push_back(tmp);
		}

	}

	/*
	
	FUN CAM STUFF
	
	*/
	
	//everything moves at the end of 1 loop, so set and add are independent - faster this way
	void SetEndFrameMoveCam(float StrafeLeftRight, float UpDown, float ForwardBackward) { //all in pge pixel size this cam movement
		//StrafeLeftRight --> + is move right, - is move left
		//UpDown --> + is up, - is down
		//ForwardBackward --> + is forward towards screen, - is backward
		olc::vf2d vInvScreenSize = {
(1.0f / float(PL.ScreenWidth())),
(1.0f / float(PL.ScreenHeight()))
		};

		moveLeftRight = ToNotPGESpace(&vInvScreenSize.x, &StrafeLeftRight);
		moveBackForward = ToNotPGESpace(&vInvScreenSize.x, &ForwardBackward);;
		moveUpDown = ToNotPGESpace(&vInvScreenSize.y, &UpDown);
	}
	void AddToEndFrameMoveCam(float StrafeLeftRight, float UpDown, float ForwardBackward) { //all in pge pixel size this cam movement
	//StrafeLeftRight --> + is move right, - is move left
	//UpDown --> + is up, - is down
	//ForwardBackward --> + is forward towards screen, - is backward
		olc::vf2d vInvScreenSize = {
(1.0f / float(PL.ScreenWidth())),
(1.0f / float(PL.ScreenHeight()))
		};

		moveLeftRight += ToNotPGESpace(&vInvScreenSize.x, &StrafeLeftRight);
		moveBackForward += ToNotPGESpace(&vInvScreenSize.x, &ForwardBackward);;
		moveUpDown += ToNotPGESpace(&vInvScreenSize.y, &UpDown);
	}
	void SetCamPos(float X, float Y, float Z) {
		UpdateCamForce = true;

		olc::vf2d vInvScreenSize = {
(1.0f / float(PL.ScreenWidth())),
(1.0f / float(PL.ScreenHeight()))
		};

		camRotationMatrix = XMMatrixRotationRollPitchYaw(0, 0, 0);
		camRight = XMVector3TransformCoord(DefaultRight, camRotationMatrix);
		camForward = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
		camVertical = XMVector3TransformCoord(DefaultUp, camRotationMatrix);

		camPosition += XMVectorScale(camRight, ToNotPGESpace(&vInvScreenSize.x, &X));
		camPosition += XMVectorScale(camForward, ToNotPGESpace(&vInvScreenSize.x, &Z));
		camPosition += XMVectorScale(camVertical, ToNotPGESpace(&vInvScreenSize.y, &Y));
	}
	void MoveCamNowAsIfRotationIs(float PretendXRotateRad, float PretendYRotateRad, float PretendZRollRad, float StrafeLeftRight, float UpDown, float ForwardBackward) { //keeps normal rotation, just pretends when applying cam position mod that the rotation is already something (or 0,0,0 - so this can be used to move as if rest position if used - allowing normal strafing while looking up for example)
		UpdateCamForce = true;

		olc::vf2d vInvScreenSize = {
(1.0f / float(PL.ScreenWidth())),
(1.0f / float(PL.ScreenHeight()))
		};

		camRotationMatrix = XMMatrixRotationRollPitchYaw(PretendYRotateRad, PretendXRotateRad, PretendZRollRad);
		camRight = XMVector3TransformCoord(DefaultRight, camRotationMatrix);
		camForward = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
		camVertical = XMVector3TransformCoord(DefaultUp, camRotationMatrix);

		camPosition += XMVectorScale(camForward, ToNotPGESpace(&vInvScreenSize.x, &ForwardBackward));
		camPosition += XMVectorScale(camRight, ToNotPGESpace(&vInvScreenSize.x, &StrafeLeftRight));
		camPosition += XMVectorScale(camVertical, ToNotPGESpace(&vInvScreenSize.y, &UpDown));
	}
	void MoveCamNowWithCurrentRotation(float StrafeLeftRight, float UpDown, float ForwardBackward) { //keeps normal rotation, just pretends when applying cam position mod that the rotation is already something (or 0,0,0 - so this can be used to move as if rest position if used - allowing normal strafing while looking up for example)
		UpdateCamForce = true;

		olc::vf2d vInvScreenSize = {
(1.0f / float(PL.ScreenWidth())),
(1.0f / float(PL.ScreenHeight()))
		};

		camRotationMatrix = XMMatrixRotationRollPitchYaw(camYRot, camXRot, camZRot);
		camRight = XMVector3TransformCoord(DefaultRight, camRotationMatrix);
		camForward = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
		camVertical = XMVector3TransformCoord(DefaultUp, camRotationMatrix);

		camPosition += XMVectorScale(camForward, ToNotPGESpace(&vInvScreenSize.x, &ForwardBackward));
		camPosition += XMVectorScale(camRight, ToNotPGESpace(&vInvScreenSize.x, &StrafeLeftRight));
		camPosition += XMVectorScale(camVertical, ToNotPGESpace(&vInvScreenSize.y, &UpDown));
	}

	void SetCamAngle(float X, float Y, float Z) { //SwivelSideWays, tilt up down, or roll with z - easy names for the unaffiliated with cam terms of pitch, yaw, roll
		camXRot = X;
		camYRot = Y;
		camZRot = Z;
	}
	void AddToCamAngle(float X, float Y, float Z) { //add also allows subtract
		camXRot += X;
		camYRot += Y;
		camZRot += Z;
	}

	std::array<float,3> ReturnCamAngle() {
		return std::array<float, 3> {camXRot, camYRot, camZRot};
	}

	std::array<float, 3> ReturnCamPosition() {
		olc::vf2d Inv = {
(1.0f / float(PL.ScreenWidth())),
(1.0f / float(PL.ScreenHeight()))
		};

		float xP = XMVectorGetByIndex(camPosition, 0);
		float yP = XMVectorGetByIndex(camPosition, 1);
		float zP = XMVectorGetByIndex(camPosition, 2);

		return std::array<float, 3>{
			ToPGESpace(&Inv.x, &xP),
				ToPGESpace(&Inv.y, &yP),
				ToPGESpace(&Inv.x, &zP)};
		//TODO: test
	}

	struct camFuncLogic {

		void LerpCamPosLogic(DataLerpFunc* tmp) {
			//use fLastElapsed to get steps
			
			float LaS = PL.LastSec();
			float TimeLeft = tmp->MaxTime - tmp->CurTime;
			float ratio = TimeLeft / LaS;

			//std::cout <<"ratio:"<< ratio << "\n";
			//std::cout << "time left:" << TimeLeft << "\n";


			if (TimeLeft > 0) { //should always pass through - for now for debug I keep - but I can trash this later - TODO:

				camRotationMatrix = XMMatrixRotationRollPitchYaw(0, 0, 0);

				if (tmp->useX) {

					camRight = XMVector3TransformCoord(DefaultRight, camRotationMatrix);

					float xP = XMVectorGetByIndex(camPosition, 0);

					float DXLeftRatio = (tmp->X - xP) / ratio;

					camPosition += XMVectorScale(camRight, DXLeftRatio);
				}
				if (tmp->useY) {

					camVertical = XMVector3TransformCoord(DefaultUp, camRotationMatrix);

					float yP = XMVectorGetByIndex(camPosition, 1);

					float DYLeftRatio = (tmp->Y - yP) / ratio;

					camPosition += XMVectorScale(camVertical, DYLeftRatio);
				}
				if (tmp->useZ) {

					camForward = XMVector3TransformCoord(DefaultForward, camRotationMatrix);

					float zP = XMVectorGetByIndex(camPosition, 2);

					float DZLeftRatio = (tmp->Z - zP) / ratio;

					camPosition += XMVectorScale(camForward, DZLeftRatio);

				}
				
				UpdateCamForce = true;

				tmp->CurTime += LaS;

			}
		}
	}CFL;

	void LerpCamPos(bool useX = false, bool useY = false, bool useZ = false, float X2 = 0, float Y2 = 0, float Z2 = 0, float MaxTime = 3.0f) {
		DataLerpFunc tmp;
		LerpCamPosFunc.push_back(tmp);

		olc::vf2d vInvScreenSize = {
	(1.0f / float(PL.ScreenWidth())),
	(1.0f / float(PL.ScreenHeight()))
		};

		LerpCamPosFunc[LerpCamPosFunc.size()-1].X = ToNotPGESpace(&vInvScreenSize.x, &X2);
		LerpCamPosFunc[LerpCamPosFunc.size() - 1].useX = useX;
		
		LerpCamPosFunc[LerpCamPosFunc.size() - 1].Y = ToNotPGESpace(&vInvScreenSize.y, &Y2);
		LerpCamPosFunc[LerpCamPosFunc.size() - 1].useY = useY;

		LerpCamPosFunc[LerpCamPosFunc.size() - 1].Z = ToNotPGESpace(&vInvScreenSize.x, &Z2);
		LerpCamPosFunc[LerpCamPosFunc.size() - 1].useZ = useZ;

		LerpCamPosFunc[LerpCamPosFunc.size() - 1].CurTime = 0.0f;
		LerpCamPosFunc[LerpCamPosFunc.size() - 1].MaxTime = MaxTime;
		
		LerpCamPosFunc[LerpCamPosFunc.size() - 1].func = [&]() {CFL.LerpCamPosLogic(LerpCamPosFunc[LerpCamPosFunc.size() - 1].me() ); };

		//TODO: runs in my draw func as lambda like coroutine 
	}
	void LerpCamRotation() {
		//TODO: runs in my draw func as lambda like coroutine 
	}
	void ChainedLerpCamPos() {
		//TODO: when done lerp it starts next in chain like linked list - which can start another, ect- and can loop the chain as option when done until stopped
	}
	void ChainedLerpCamRotation() {
		//TODO: when done lerp it starts next in chain like linked list - which can start another, ect- and can loop the chain as option when done until stopped
	}

	/*
	
	END OF FUN CAM STUFF
	
	*/
	void ProgramLink_3D_DX::DrawFuncMain() {

		for (int i = 0; i < LerpCamPosFunc.size(); i++) {
			if (LerpCamPosFunc[i].CurTime < LerpCamPosFunc[i].MaxTime) {
				LerpCamPosFunc[i].func();
			}
			else {
				LerpCamPosFunc.erase(LerpCamPosFunc.begin() + i);
				i -= 1;
			}
		}

		if (pge->GetLayers()[currentLayer].bUpdate)
		{
			pge->GetLayers()[currentLayer].pDrawTarget.Decal()->Update();
			pge->GetLayers()[currentLayer].bUpdate = false;
		}

		olc::renderer->DrawLayerQuad(pge->GetLayers()[currentLayer].vOffset, pge->GetLayers()[currentLayer].vScale, pge->GetLayers()[currentLayer].tint);

		for (int i = 0; i < DrawOrderBefore.size(); i++) {
			DrawOrderBefore[i].func();
		}

		//dxDeviceContext->IASetInputLayout(dxInputLayout);

		for (auto& decal : pge->GetLayers()[currentLayer].vecDecalInstance)
			olc::renderer->DrawDecal(decal);
		pge->GetLayers()[currentLayer].vecDecalInstance.clear();

		for (int i = 0; i < DrawOrder.size(); i++) {
			DrawOrder[i].func();
		}

		DrawOrder.clear();
		DrawOrderBefore.clear();

		//clear tmp buf : start
		for (int i = 0; i < MDFs.CBufTmp.size(); i++) {
			SafeRelease(*MDFs.CBufTmp[i]);
		}
		MDFs.CBufTmp.clear();
		//clear tmp buf : end
	}



}


#endif
