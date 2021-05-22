/*TODO
	olcPGEX_DX11ShadersPlus.h

	+-------------------------------------------------------------+
	|         OneLoneCoder Pixel Game Engine Extension            |
	|                DX11 shaders Macro v0.1		      |
	+-------------------------------------------------------------+

	What is this?
	~~~~~~~~~~~~~
	This is an extension to the olcPixelGameEngine, which provides
	macros of shaders for easy use without any knowledge of graphics API's
	**Get the advantages of using DX11 shaders without need to know any shaders**
	This includes Compute shader compute utilizing dx11 and output

	Use of this extension requires the olcPixelGameEngine-DX11-Fork's olcPixelGameEngine header

	It also requires

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

#pragma once



//examine code utilizing pragma regions - else reading will hurt
//also - inheritance would have been messy... sorry - not gonna do that
#if defined(OLC_PGEX_DIRECTX11_SHADERS_PLUS)


class ProgramLink : public olc::PGEX {
public:
	
	void InitializeParticlesWorker();

	ProgramLink()
	{

	}

	int32_t ScreenWidth() { return pge->ScreenWidth(); };
	int32_t ScreenHeight() { return pge->ScreenHeight(); };

	void MoveParticleLayer(int i);

	void DrawFuncMain();

	std::function<void()> DrawerHandle = [&] { DrawFuncMain(); };
	
	int currentLayer = 0;

	void OnAfterUserCreate() {

		pge->SetLayerCustomRenderFunction(currentLayer, DrawerHandle);
	
	}

	void OnAfterUserUpdate(float fElapsedTime) {

	}
	
}PL;

#pragma region TestParticleClass
//not inheriting since i'd assume many edge cases...
struct TestParticleClass {
	bool ToDraw = false;
	locVertexF pVertexMem[4];

	olc::Sprite* sprite;

	ID3D11VertexShader* TestVSs;
	ID3D11PixelShader* TestPSs;

	ID3D11ShaderResourceView* SRV;
	ID3D11Resource* SR;
	ID3D11UnorderedAccessView* UAV;
	ID3D11SamplerState* SS;
	ID3D11Texture2D* gpuTex;
	ID3D11Texture2D* gpuTexS;

	ID3D11BlendState* BlendState;

	float tint[4];

	olc::vf2d pos[4]; //may change to have depth - and you input that depth - may need inherit draw after update thing

	olc::vf2d uv[4];

	float w[4]; //w is depth

	ID3D11Buffer* vb_quad;

	void updateVBuff() {
		for (int i = 0; i < 4; i++) {
			this->pVertexMem[i] = { {this->pos[i].x,this->pos[i].y, this->w[i]}, {this->uv[i].x, this->uv[i].y},{this->tint[0],this->tint[1],this->tint[2],this->tint[3]} };
		}
		D3D11_MAPPED_SUBRESOURCE resource;
		dxDeviceContext->Map(vb_quad, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
		memcpy(resource.pData, &pVertexMem, sizeof(locVertexF) * 4);
		dxDeviceContext->Unmap(vb_quad, 0);
	}
	
	void Adjust(const olc::vf2d& pos, const olc::vf2d& scale = { 1.0f,1.0f }, olc::Pixel tint = olc::WHITE, std::array<float, 4> depth = { 0.0f, 0.0f, 0.0f, 0.0f }) {
		//no texture change because this is not a reasonable particle system to use... it is not even a system...
		this->tint[0] = tint.r;
		this->tint[1] = tint.g;
		this->tint[2] = tint.b;
		this->tint[3] = tint.a;
		olc::vf2d vInvScreenSize = {
	(1.0f / float(PL.ScreenWidth())),
	(1.0f / float(PL.ScreenHeight()))
		};

		olc::vf2d vScreenSpacePos =
		{
			(std::floor(pos.x) * vInvScreenSize.x) * 2.0f - 1.0f,
			((std::floor(pos.y) * vInvScreenSize.y) * 2.0f - 1.0f) * -1.0f
		};

		olc::vf2d vScreenSpaceDim =
		{
			vScreenSpacePos.x + (2.0f * (float(sprite->width) * vInvScreenSize.x)) * scale.x,
			vScreenSpacePos.y - (2.0f * (float(sprite->height) * vInvScreenSize.y)) * scale.y
		};


		this->w[0] = depth[0];
		this->w[1] = depth[1];
		this->w[2] = depth[2];
		this->w[3] = depth[3];

		this->pos[0] = { vScreenSpacePos.x, vScreenSpacePos.y };
		this->pos[1] = { vScreenSpacePos.x, vScreenSpaceDim.y };
		this->pos[2] = { vScreenSpaceDim.x, vScreenSpaceDim.y };
		this->pos[3] = { vScreenSpaceDim.x, vScreenSpacePos.y };


		this->uv[0] = { 0.0f, 0.0f };
		this->uv[1] = { 0.0f, 1.0f };
		this->uv[2] = { 1.0f, 1.0f };
		this->uv[3] = { 1.0f, 0.0f };

		updateVBuff();

	}

	void ChangeBlend(const olc::DecalMode& mode) {
		D3D11_BLEND_DESC blendVal;

		switch (mode)
		{
		case olc::DecalMode::NORMAL:
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
			break;
		case olc::DecalMode::ADDITIVE:
			blendVal.AlphaToCoverageEnable = false;
			blendVal.IndependentBlendEnable = false;
			blendVal.RenderTarget[0].BlendEnable = true;
			blendVal.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
			blendVal.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			blendVal.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendVal.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendVal.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			blendVal.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendVal.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			break;
		case olc::DecalMode::MULTIPLICATIVE:
			blendVal.AlphaToCoverageEnable = true;
			blendVal.IndependentBlendEnable = false;
			blendVal.RenderTarget[0].BlendEnable = true;
			blendVal.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
			blendVal.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR;
			blendVal.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendVal.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendVal.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blendVal.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendVal.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			break;
		case olc::DecalMode::STENCIL:
			blendVal.AlphaToCoverageEnable = false;
			blendVal.IndependentBlendEnable = false;
			blendVal.RenderTarget[0].BlendEnable = true;
			blendVal.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
			blendVal.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_ALPHA;
			blendVal.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendVal.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendVal.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blendVal.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendVal.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			break;
		case olc::DecalMode::ILLUMINATE:
			blendVal.AlphaToCoverageEnable = false;
			blendVal.IndependentBlendEnable = false;
			blendVal.RenderTarget[0].BlendEnable = true;
			blendVal.RenderTarget[0].SrcBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blendVal.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_ALPHA;
			blendVal.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendVal.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendVal.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blendVal.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendVal.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			break;
		case olc::DecalMode::WIREFRAME:
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
			break;
		}
		SafeRelease(BlendState);
		dxDevice->CreateBlendState(&blendVal, &BlendState);

	}

	void Draw() {

		const UINT vertexStride = sizeof(locVertexF);
		const UINT offset = 0;

		dxDeviceContext->IASetVertexBuffers(0, 1, &vb_quad, &vertexStride, &offset);
		dxDeviceContext->IASetInputLayout(
			dxInputLayout);
		dxDeviceContext->IASetPrimitiveTopology(
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		dxDeviceContext->IASetIndexBuffer(
			m_viQuad,
			DXGI_FORMAT_R32_UINT,
			0);

		dxDeviceContext->VSSetShader(
			TestVSs,
			nullptr,
			0);


		dxDeviceContext->RSSetState(dxRasterizerStateF);

		float bState[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		dxDeviceContext->OMSetBlendState(BlendState, bState, 0xffffffff);

		dxDeviceContext->OMSetDepthStencilState(dxDepthStencilStateDefault, 1);

		dxDeviceContext->PSSetShader(
			TestPSs,
			nullptr,
			0);


		dxDeviceContext->PSSetShaderResources(0, 1, &SRV);

		dxDeviceContext->PSSetSamplers(0, 1, &SS);

		dxDeviceContext->DrawIndexed( //TODO: make the layer 4 indices... but its really not important - it could be less than a ns of time saved
			(4), //TODO: make 4 
			0,
			0);

	}

	void CreateTexture(/*olc::Sprite* sprite, ID3D11ShaderResourceView** SRV, ID3D11Resource** SR, ID3D11UnorderedAccessView** UAV, ID3D11SamplerState** SS, ID3D11Texture2D** gpuTex, ID3D11Texture2D** gpuTexS*/) {

		D3D11_TEXTURE2D_DESC gpuTexDesc;
		ZeroMemory(&gpuTexDesc, sizeof(gpuTexDesc));
		gpuTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		gpuTexDesc.Width = sprite->width;
		gpuTexDesc.Height = sprite->height;
		gpuTexDesc.MipLevels = 1;
		gpuTexDesc.ArraySize = 1;
		gpuTexDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS |
			D3D11_BIND_SHADER_RESOURCE;
		gpuTexDesc.SampleDesc.Count = 1;
		gpuTexDesc.SampleDesc.Quality = 0;
		gpuTexDesc.MiscFlags = 0;
		gpuTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
		gpuTexDesc.Usage = D3D11_USAGE_DEFAULT;

		D3D11_TEXTURE2D_DESC gpuTexDescS;
		ZeroMemory(&gpuTexDescS, sizeof(gpuTexDescS));
		gpuTexDescS.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		gpuTexDescS.Width = sprite->width;
		gpuTexDescS.Height = sprite->height;
		gpuTexDescS.MipLevels = 1;
		gpuTexDescS.ArraySize = 1;
		gpuTexDescS.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		gpuTexDescS.SampleDesc.Count = 1;
		gpuTexDescS.SampleDesc.Quality = 0;
		gpuTexDescS.MiscFlags = 0;
		gpuTexDescS.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		gpuTexDescS.Usage = D3D11_USAGE_DYNAMIC;


		D3D11_UNORDERED_ACCESS_VIEW_DESC UAVdesc;
		UAVdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		UAVdesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		UAVdesc.Buffer.FirstElement = 0;
		UAVdesc.Buffer.NumElements = 1;
		UAVdesc.Texture2D.MipSlice = 0;

		D3D11_SHADER_RESOURCE_VIEW_DESC SRVdesc;
		SRVdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SRVdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVdesc.Buffer.FirstElement = 0;
		SRVdesc.Buffer.NumElements = 1;
		SRVdesc.Texture2D.MostDetailedMip = 0;
		SRVdesc.Texture2D.MipLevels = 1;

		D3D11_SAMPLER_DESC tmpSampleDesc;

		tmpSampleDesc.Filter = D3D11_FILTER{ D3D11_FILTER_ANISOTROPIC };
		tmpSampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE{ D3D11_TEXTURE_ADDRESS_WRAP };
		tmpSampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE{ D3D11_TEXTURE_ADDRESS_WRAP };
		tmpSampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE{ D3D11_TEXTURE_ADDRESS_WRAP };
		tmpSampleDesc.MipLODBias = 0;
		tmpSampleDesc.MaxAnisotropy = 8;
		tmpSampleDesc.ComparisonFunc = D3D11_COMPARISON_FUNC{ D3D11_COMPARISON_LESS };
		tmpSampleDesc.MinLOD = 1;
		tmpSampleDesc.MaxLOD = D3D11_FLOAT32_MAX;

		if (false) //don't give an option for mip linear filter for now - testing 
		{
			tmpSampleDesc.Filter = D3D11_FILTER{ D3D11_FILTER_MIN_MAG_MIP_LINEAR };
		}
		else
		{
			tmpSampleDesc.Filter = D3D11_FILTER{ D3D11_FILTER_MIN_MAG_MIP_POINT };
		}

		if (true) //no option for mirror tex as well
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

		dxDevice->CreateTexture2D(&gpuTexDesc, NULL, &gpuTex);
		dxDevice->CreateTexture2D(&gpuTexDescS, NULL, &gpuTexS);

		dxDevice->CreateShaderResourceView(gpuTexS, &SRVdesc, &SRV); //seperate

		dxDevice->CreateUnorderedAccessView(gpuTex, &UAVdesc, &UAV);

		dxDevice->CreateSamplerState(&tmpSampleDesc, &SS);

		D3D11_MAPPED_SUBRESOURCE resource;

		dxDeviceContext->Map(gpuTexS, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
		BYTE* mappedData = reinterpret_cast<BYTE*>(resource.pData);
		BYTE* buffer = reinterpret_cast<BYTE*>(sprite->pColData.data());

		for (int i = 0; i < sprite->height; i++) {
			memcpy(mappedData, buffer, sprite->width * sizeof(olc::Pixel));
			mappedData += resource.RowPitch;

			buffer += sprite->width * sizeof(olc::Pixel);
		}

		dxDeviceContext->Unmap(gpuTexS, 0);


	}


	const std::string TestPS = std::string(
		"Texture2D shaderTexture : register(t0);\n"
		"SamplerState SampleType : register(s0);\n"
		"struct PixelShaderInput{\n"
		"float4 position : SV_POSITION;\n"
		"float4 color: COLOR;\n"
		"float2 tex : TEXCOORD0;\n"
		"float4 PositionWS : TEXCOORD1;};\n"
		"float4 SimplePS(PixelShaderInput IN) : SV_TARGET{\n"
		"float4 textureColor = shaderTexture.Sample(SampleType, IN.tex);\n"
		"textureColor.r *= IN.color.r/255;\n"
		"textureColor.g *= IN.color.g/255;\n"
		"textureColor.b *= IN.color.b/255;\n"
		"textureColor.w *= IN.color.w/255;\n"
		"return textureColor;}");


	const std::string TestVS = std::string(

		"cbuffer PerApplication : register(b0){\n"
		"matrix projectionMatrix;}\n"
		"cbuffer PerFrame : register(b1){\n"
		"matrix viewMatrix;}\n"
		"cbuffer PerObject : register(b2){\n"
		"matrix worldMatrix;}\n"
		"struct AppData{\n"
		"float3 position : POSITION;\n"
		"float4 color: COLOR;\n"
		"float2 tex : TEXCOORD;\n"
		"};\n"
		"struct VertexShaderOutput{\n"
		"float4 position : SV_POSITION;\n"
		"float4 color: COLOR;\n"
		"float2 tex : TEXCOORD0;\n"
		"float4 PositionWS : TEXCOORD1;};\n"
		"VertexShaderOutput SimpleVS(AppData IN){\n"
		"VertexShaderOutput OUT;\n"
		"matrix mvp = mul(projectionMatrix, mul(viewMatrix, worldMatrix));\n"
		"OUT.position = float4(IN.position,1);"
		"OUT.PositionWS = mul(worldMatrix, float4(IN.position, 1.0f));\n"
		"OUT.tex = IN.tex;\n"
		"OUT.color = IN.color;\n"
		"return OUT;}");

	TestParticleClass() {

		TestVSs = LoadShader<ID3D11VertexShader>(&TestVS, "SimpleVS", "latest");

		TestPSs = LoadShader<ID3D11PixelShader>(&TestPS, "SimplePS", "latest");


	}

};
#pragma endregion

#pragma region RandomParticleRange

struct RandomRangeParticleClass {
	bool ToDraw = false;
	locVertexF pVertexMem[4];

	ID3D11BlendState* BlendState;

	olc::vf2d DistMax;

	int elementCount = 0;

	olc::Sprite* sprite;

	ID3D11VertexShader* RandVSs;
	ID3D11PixelShader* RandPSs;

	ID3D11ShaderResourceView* SRV;
	ID3D11Resource* SR;
	ID3D11UnorderedAccessView* UAV;
	ID3D11SamplerState* SS;
	ID3D11Texture2D* gpuTex;
	ID3D11Texture2D* gpuTexS;

	float tint[4];

	olc::vf2d pos[4]; //may change to have depth - and you input that depth - may need inherit draw after update thing

	olc::vf2d uv[4];

	float w[4]; //w is depth

	ID3D11InputLayout* dxInputLayoutR;

	ID3D11Buffer* vb_quad; //[0]
	ID3D11Buffer* instB; //instance buffer is [1]
	//ID3D11Buffer* instVB[2] = { vb_quad , instB};

	struct InstData {

		XMFLOAT2 offset;

	};

	void ChangeBlend(const olc::DecalMode& mode) {
		D3D11_BLEND_DESC blendVal;

		switch (mode)
		{
		case olc::DecalMode::NORMAL:
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
			break;
		case olc::DecalMode::ADDITIVE:
			blendVal.AlphaToCoverageEnable = false;
			blendVal.IndependentBlendEnable = false;
			blendVal.RenderTarget[0].BlendEnable = true;
			blendVal.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
			blendVal.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			blendVal.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendVal.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendVal.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			blendVal.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendVal.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			break;
		case olc::DecalMode::MULTIPLICATIVE:
			blendVal.AlphaToCoverageEnable = true;
			blendVal.IndependentBlendEnable = false;
			blendVal.RenderTarget[0].BlendEnable = true;
			blendVal.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
			blendVal.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR;
			blendVal.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendVal.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendVal.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blendVal.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendVal.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			break;
		case olc::DecalMode::STENCIL:
			blendVal.AlphaToCoverageEnable = false;
			blendVal.IndependentBlendEnable = false;
			blendVal.RenderTarget[0].BlendEnable = true;
			blendVal.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
			blendVal.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_ALPHA;
			blendVal.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendVal.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendVal.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blendVal.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendVal.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			break;
		case olc::DecalMode::ILLUMINATE:
			blendVal.AlphaToCoverageEnable = false;
			blendVal.IndependentBlendEnable = false;
			blendVal.RenderTarget[0].BlendEnable = true;
			blendVal.RenderTarget[0].SrcBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blendVal.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_ALPHA;
			blendVal.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendVal.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendVal.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blendVal.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendVal.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			break;
		case olc::DecalMode::WIREFRAME:
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
			break;
		}
		SafeRelease(BlendState);
		dxDevice->CreateBlendState(&blendVal, &BlendState);

	}

	void GenerateRandomPosFirst() {

		std::vector<InstData> instance(elementCount);

		for (int i = 0; i < instance.size(); i++) {
			float x = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / DistMax.x)) * 2;
			float y = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / DistMax.y)) * 2;
			XMStoreFloat2(&instance[i].offset, { x, y });
			//TODOinclude velocity in here for the movment version of this 
		}

		D3D11_BUFFER_DESC instBuffDesc;
		ZeroMemory(&instBuffDesc, sizeof(instBuffDesc));

		instBuffDesc.Usage = D3D11_USAGE_DEFAULT;
		instBuffDesc.ByteWidth = sizeof(InstData) * elementCount;
		instBuffDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_VERTEX_BUFFER;
		instBuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		instBuffDesc.MiscFlags = 0;
		instBuffDesc.Usage = D3D11_USAGE_DYNAMIC;

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));

		data.pSysMem = &instance[0];
		dxDevice->CreateBuffer(&instBuffDesc, &data, &instB);

	}

	ID3D11VertexShader* CreateShaderVR(ID3DBlob* pShaderBlob, ID3D11ClassLinkage* pClassLinkage) //vertex shader - shader type
	{
		ID3D11VertexShader* pVertexShader = nullptr;
		dxDevice->CreateVertexShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), pClassLinkage, &pVertexShader); //make a shader based on buffer, buffer size, classtype, and return to pshader object

		D3D11_INPUT_ELEMENT_DESC dxVertexLayoutDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

			{ "OFFSET", 0, DXGI_FORMAT_R32G32_FLOAT, UINT(1), 0, D3D11_INPUT_PER_INSTANCE_DATA, UINT(1) }
		};

		HRESULT hr = dxDevice->CreateInputLayout( //make input layout - global change to input Layout
			dxVertexLayoutDesc, //vertex shader - input assembler data
			_countof(dxVertexLayoutDesc), //number of elements
			pShaderBlob->GetBufferPointer(),  //vertex shader buffer
			pShaderBlob->GetBufferSize(), //vetex shader blob size 
			&dxInputLayoutR); //input layout to output to

		if (FAILED(hr))
		{
			OutputDebugStringW(L"failed input layout setup");
		}
		return pVertexShader;
	}

	ID3D11VertexShader* LoadShaderVR(const std::string* shaderInfo, const std::string& entryPoint, const std::string& _profile) {

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

		pShader = CreateShaderVR(pShaderBlob, nullptr);

		SafeRelease(pShaderBlob); // no longer need shader mem
		SafeRelease(pErrorBlob); // no longer need shader mem <-- I frogot to safe release to delete and do other stuff... so I need to look back at that

		return pShader;

	}

	void updateVBuff() {
		for (int i = 0; i < 4; i++) {
			this->pVertexMem[i] = { {this->pos[i].x,this->pos[i].y, this->w[i]}, {this->uv[i].x, this->uv[i].y},{this->tint[0],this->tint[1],this->tint[2],this->tint[3]} };
		}
		D3D11_MAPPED_SUBRESOURCE resource;
		dxDeviceContext->Map(vb_quad, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
		memcpy(resource.pData, &pVertexMem, sizeof(locVertexF) * 4);
		dxDeviceContext->Unmap(vb_quad, 0);
	}

	void Adjust(int elementCount, const std::array<olc::vf2d, 2> InitialToEnd, olc::Sprite* sprite, const olc::vf2d& scale = { 1.0f,1.0f }, olc::Pixel tint = olc::WHITE, std::array<float, 4> depth = { 0.0f, 0.0f, 0.0f, 0.0f }) {
		
		SafeRelease(gpuTex);
		SafeRelease(gpuTexS);
		SafeRelease(UAV);
		SafeRelease(SRV);
		SafeRelease(vb_quad);
		SafeRelease(instB);

		this->elementCount = elementCount;
		this->sprite = sprite;
		this -> tint[0] = tint.r;
		this -> tint[1] = tint.g;
		this -> tint[2] = tint.b;
		this -> tint[3] = tint.a;

		olc::vf2d vInvScreenSize = {
			(1.0f / float(PL.ScreenWidth())),
			(1.0f / float(PL.ScreenHeight()))
		};

		this->DistMax = //max distance - make random points from around these points
		{
		(std::floor(InitialToEnd[1].x) / float(PL.ScreenWidth())),
		-1*((std::floor(InitialToEnd[1].y) / float(PL.ScreenHeight())))
		};


		olc::vf2d vScreenSpacePos =
		{
			(std::floor(InitialToEnd[0].x) * vInvScreenSize.x) * 2.0f - 1.0f,
			((std::floor(InitialToEnd[0].y) * vInvScreenSize.y) * 2.0f - 1.0f) * -1.0f
		};

		olc::vf2d vScreenSpaceDim =
		{
			vScreenSpacePos.x + (2.0f * (float(sprite->width) * vInvScreenSize.x)) * scale.x,
			vScreenSpacePos.y - (2.0f * (float(sprite->height) * vInvScreenSize.y)) * scale.y
		};



		w[0] = depth[0];
		w[1] = depth[1];
		w[2] = depth[2];
		w[3] = depth[3];

		pos[0] = { vScreenSpacePos.x, vScreenSpacePos.y };
		pos[1] = { vScreenSpacePos.x, vScreenSpaceDim.y };
		pos[2] = { vScreenSpaceDim.x, vScreenSpaceDim.y };
		pos[3] = { vScreenSpaceDim.x, vScreenSpacePos.y };


		uv[0] = { 0.0f, 0.0f };
		uv[1] = { 0.0f, 1.0f };
		uv[2] = { 1.0f, 1.0f };
		uv[3] = { 1.0f, 0.0f };

		for (int i = 0; i < 4; i++) {
			pVertexMem[i] = { {pos[i].x,pos[i].y, w[i]}, {uv[i].x, uv[i].y},{this->tint[0],this->tint[1],this->tint[2],this->tint[3]} };
		}

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

		vertexBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_VERTEX_BUFFER;

		vertexBufferDesc.ByteWidth = sizeof(pVertexMem);
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

		D3D11_SUBRESOURCE_DATA resourceData;
		ZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));
		resourceData.pSysMem = pVertexMem;

		dxDevice->CreateBuffer(&vertexBufferDesc, &resourceData, &vb_quad);

		CreateTexture();

		GenerateRandomPosFirst();


	}
	void RegenRRforRandomRange() {

		std::vector<InstData> instance(elementCount);

		for (int i = 0; i < instance.size(); i++) {
			float x = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / DistMax.x)) * 2;
			float y = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / DistMax.y)) * 2;
			XMStoreFloat2(&instance[i].offset, {x, y}); //*2
			//TODOinclude velocity in here for the movment version of this 
		}

		D3D11_MAPPED_SUBRESOURCE resource;
		dxDeviceContext->Map(instB, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
		memcpy(resource.pData, &instance[0], sizeof(InstData) * elementCount);
		dxDeviceContext->Unmap(instB, 0);

	}

	void Draw() {
		const UINT vertexStride[2] = { sizeof(locVertexF), sizeof(InstData) };
		const UINT offset[2] = { 0,0 };

		ID3D11Buffer* instVB[2] = { vb_quad , instB };

		dxDeviceContext->IASetInputLayout(
			dxInputLayoutR);

		dxDeviceContext->IASetVertexBuffers(0, 2, instVB, vertexStride, offset);

		dxDeviceContext->IASetPrimitiveTopology(
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		dxDeviceContext->IASetIndexBuffer(
			m_viQuad,
			DXGI_FORMAT_R32_UINT,
			0);

		dxDeviceContext->VSSetShader(
			RandVSs,
			nullptr,
			0);


		dxDeviceContext->RSSetState(dxRasterizerStateF);

		float bState[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		dxDeviceContext->OMSetBlendState(BlendState, bState, 0xffffffff);

		dxDeviceContext->OMSetDepthStencilState(dxDepthStencilStateDefault, 1);

		dxDeviceContext->PSSetShader(
			RandPSs,
			nullptr,
			0);


		dxDeviceContext->PSSetShaderResources(0, 1, &SRV);

		dxDeviceContext->PSSetSamplers(0, 1, &SS);

		dxDeviceContext->DrawIndexedInstanced( //TODO: make the layer 4 indices... but its really not important - it could be less than a ns of time saved
			(4), //TODO: make 4 
			elementCount,
			0,
			0,
			0);

		dxDeviceContext->IASetInputLayout(
			dxInputLayout); //default layout - else i'ma do jank rebinding every for every call with how I code... bad for perf
	}

	void CreateTexture(/*olc::Sprite* sprite, ID3D11ShaderResourceView** SRV, ID3D11Resource** SR, ID3D11UnorderedAccessView** UAV, ID3D11SamplerState** SS, ID3D11Texture2D** gpuTex, ID3D11Texture2D** gpuTexS*/) {

		D3D11_TEXTURE2D_DESC gpuTexDesc;
		ZeroMemory(&gpuTexDesc, sizeof(gpuTexDesc));
		gpuTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		gpuTexDesc.Width = sprite->width;
		gpuTexDesc.Height = sprite->height;
		gpuTexDesc.MipLevels = 1;
		gpuTexDesc.ArraySize = 1;
		gpuTexDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS |
			D3D11_BIND_SHADER_RESOURCE;
		gpuTexDesc.SampleDesc.Count = 1;
		gpuTexDesc.SampleDesc.Quality = 0;
		gpuTexDesc.MiscFlags = 0;
		gpuTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
		gpuTexDesc.Usage = D3D11_USAGE_DEFAULT;

		D3D11_TEXTURE2D_DESC gpuTexDescS;
		ZeroMemory(&gpuTexDescS, sizeof(gpuTexDescS));
		gpuTexDescS.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		gpuTexDescS.Width = sprite->width;
		gpuTexDescS.Height = sprite->height;
		gpuTexDescS.MipLevels = 1;
		gpuTexDescS.ArraySize = 1;
		gpuTexDescS.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		gpuTexDescS.SampleDesc.Count = 1;
		gpuTexDescS.SampleDesc.Quality = 0;
		gpuTexDescS.MiscFlags = 0;
		gpuTexDescS.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		gpuTexDescS.Usage = D3D11_USAGE_DYNAMIC;


		D3D11_UNORDERED_ACCESS_VIEW_DESC UAVdesc;
		UAVdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		UAVdesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		UAVdesc.Buffer.FirstElement = 0;
		UAVdesc.Buffer.NumElements = 1;
		UAVdesc.Texture2D.MipSlice = 0;

		D3D11_SHADER_RESOURCE_VIEW_DESC SRVdesc;
		SRVdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SRVdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVdesc.Buffer.FirstElement = 0;
		SRVdesc.Buffer.NumElements = 1;
		SRVdesc.Texture2D.MostDetailedMip = 0;
		SRVdesc.Texture2D.MipLevels = 1;

		D3D11_SAMPLER_DESC tmpSampleDesc;

		tmpSampleDesc.Filter = D3D11_FILTER{ D3D11_FILTER_ANISOTROPIC };
		tmpSampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE{ D3D11_TEXTURE_ADDRESS_WRAP };
		tmpSampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE{ D3D11_TEXTURE_ADDRESS_WRAP };
		tmpSampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE{ D3D11_TEXTURE_ADDRESS_WRAP };
		tmpSampleDesc.MipLODBias = 0;
		tmpSampleDesc.MaxAnisotropy = 8;
		tmpSampleDesc.ComparisonFunc = D3D11_COMPARISON_FUNC{ D3D11_COMPARISON_LESS };
		tmpSampleDesc.MinLOD = 1;
		tmpSampleDesc.MaxLOD = D3D11_FLOAT32_MAX;

		if (false) //don't give an option for mip linear filter for now - testing 
		{
			tmpSampleDesc.Filter = D3D11_FILTER{ D3D11_FILTER_MIN_MAG_MIP_LINEAR };
		}
		else
		{
			tmpSampleDesc.Filter = D3D11_FILTER{ D3D11_FILTER_MIN_MAG_MIP_POINT };
		}

		if (true) //no option for mirror tex as well
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

		dxDevice->CreateTexture2D(&gpuTexDesc, NULL, &gpuTex);
		dxDevice->CreateTexture2D(&gpuTexDescS, NULL, &gpuTexS);

		dxDevice->CreateShaderResourceView(gpuTexS, &SRVdesc, &SRV); //seperate

		dxDevice->CreateUnorderedAccessView(gpuTex, &UAVdesc, &UAV);

		dxDevice->CreateSamplerState(&tmpSampleDesc, &SS);

		D3D11_MAPPED_SUBRESOURCE resource;

		dxDeviceContext->Map(gpuTexS, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
		BYTE* mappedData = reinterpret_cast<BYTE*>(resource.pData);
		BYTE* buffer = reinterpret_cast<BYTE*>(sprite->pColData.data());

		for (int i = 0; i < sprite->height; i++) {
			memcpy(mappedData, buffer, sprite->width * sizeof(olc::Pixel));
			mappedData += resource.RowPitch;

			buffer += sprite->width * sizeof(olc::Pixel);
		}

		dxDeviceContext->Unmap(gpuTexS, 0);


	}


	const std::string RandPS = std::string(
		"Texture2D shaderTexture : register(t0);\n"
		"SamplerState SampleType : register(s0);\n"
		"struct PixelShaderInput{\n"
		"float4 position : SV_POSITION;\n"
		"float4 color: COLOR;\n"
		"float2 tex : TEXCOORD0;\n"
		"float4 PositionWS : TEXCOORD1;};\n"
		"float4 SimplePS(PixelShaderInput IN) : SV_TARGET{\n"
		"float4 textureColor = shaderTexture.Sample(SampleType, IN.tex);\n"
		"textureColor.r *= IN.color.r/255;\n"
		"textureColor.g *= IN.color.g/255;\n"
		"textureColor.b *= IN.color.b/255;\n"
		"textureColor.w *= IN.color.w/255;\n"
		"return textureColor;}");


	const std::string RandVS = std::string(

		"cbuffer PerApplication : register(b0){\n"
		"matrix projectionMatrix;}\n"
		"cbuffer PerFrame : register(b1){\n"
		"matrix viewMatrix;}\n"
		"cbuffer PerObject : register(b2){\n"
		"matrix worldMatrix;}\n"
		"struct AppData{\n"
		"float3 position : POSITION;\n"
		"float4 color: COLOR;\n"
		"float2 tex : TEXCOORD;\n"
		"float2 offset : OFFSET;\n"
		"};\n"
		"struct VertexShaderOutput{\n"
		"float4 position : SV_POSITION;\n"
		"float4 color: COLOR;\n"
		"float2 tex : TEXCOORD0;\n"
		"float4 PositionWS : TEXCOORD1;};\n"
		"VertexShaderOutput SimpleVS(AppData IN){\n"
		"VertexShaderOutput OUT;\n"
		"matrix mvp = mul(projectionMatrix, mul(viewMatrix, worldMatrix));\n"
		"OUT.position = float4(float3(IN.position[0]+IN.offset[0],IN.position[1]+IN.offset[1],IN.position[2]),1);"
		"OUT.PositionWS = mul(worldMatrix, float4(IN.position, 1.0f));\n"
		"OUT.tex = IN.tex;\n"
		"OUT.color = IN.color;\n"
		"return OUT;}");

	RandomRangeParticleClass() {

		RandVSs = LoadShaderVR(&RandVS, "SimpleVS", "latest");

		RandPSs = LoadShader<ID3D11PixelShader>(&RandPS, "SimplePS", "latest");


	}

};

#pragma endregion

struct SystemsCollection {

	std::vector<TestParticleClass> TestParticles;
	std::vector<RandomRangeParticleClass> RandomRangeParticles;
}SysC;

#pragma region RandomRangeParticlesFuncs

//InitialToEnd refers to start X and Y you are allowed to generate from --> and then the end is a Width and Height value of pixels
int DX11CreateRandomRangeParticleSystem(int elementCount, const std::array<olc::vf2d,2> InitialToEnd, olc::Sprite* sprite, const olc::vf2d& scale = { 1.0f,1.0f }, olc::Pixel tint = olc::WHITE, std::array<float, 4> depth = { 0.0f, 0.0f, 0.0f, 0.0f }) {
	RandomRangeParticleClass tmpClass;

	tmpClass.BlendState = dxBlendStateDefault;
	tmpClass.elementCount = elementCount;
	tmpClass.sprite = sprite;
	tmpClass.tint[0] = tint.r;
	tmpClass.tint[1] = tint.g;
	tmpClass.tint[2] = tint.b;
	tmpClass.tint[3] = tint.a;

	olc::vf2d vInvScreenSize = {
		(1.0f / float(PL.ScreenWidth())),
		(1.0f / float(PL.ScreenHeight()))
	};

	tmpClass.DistMax = //max distance - make random points from around these points
	{
	(std::floor(InitialToEnd[1].x)/ float(PL.ScreenWidth())),
	((std::floor(InitialToEnd[1].y) / float(PL.ScreenHeight())))
	};


	olc::vf2d vScreenSpacePos =
	{
		(std::floor(InitialToEnd[0].x) * vInvScreenSize.x) * 2.0f - 1.0f,
		((std::floor(InitialToEnd[0].y) * vInvScreenSize.y) * 2.0f - 1.0f) * -1.0f
	};

	olc::vf2d vScreenSpaceDim =
	{
		vScreenSpacePos.x + (2.0f * (float(sprite->width) * vInvScreenSize.x)) * scale.x,
		vScreenSpacePos.y - (2.0f * (float(sprite->height) * vInvScreenSize.y)) * scale.y
	};



	tmpClass.w[0] = depth[0];
	tmpClass.w[1] = depth[1];
	tmpClass.w[2] = depth[2];
	tmpClass.w[3] = depth[3];

	tmpClass.pos[0] = { vScreenSpacePos.x, vScreenSpacePos.y };
	tmpClass.pos[1] = { vScreenSpacePos.x, vScreenSpaceDim.y };
	tmpClass.pos[2] = { vScreenSpaceDim.x, vScreenSpaceDim.y };
	tmpClass.pos[3] = { vScreenSpaceDim.x, vScreenSpacePos.y };


	tmpClass.uv[0] = { 0.0f, 0.0f };
	tmpClass.uv[1] = { 0.0f, 1.0f };
	tmpClass.uv[2] = { 1.0f, 1.0f };
	tmpClass.uv[3] = { 1.0f, 0.0f };

	for (int i = 0; i < 4; i++) {
		tmpClass.pVertexMem[i] = { {tmpClass.pos[i].x,tmpClass.pos[i].y, tmpClass.w[i]}, {tmpClass.uv[i].x, tmpClass.uv[i].y},{tmpClass.tint[0],tmpClass.tint[1],tmpClass.tint[2],tmpClass.tint[3]} };
	}

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	vertexBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_VERTEX_BUFFER;

	vertexBufferDesc.ByteWidth = sizeof(tmpClass.pVertexMem);
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

	D3D11_SUBRESOURCE_DATA resourceData;
	ZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	resourceData.pSysMem = tmpClass.pVertexMem;

	dxDevice->CreateBuffer(&vertexBufferDesc, &resourceData, &tmpClass.vb_quad);

	tmpClass.CreateTexture();

	tmpClass.GenerateRandomPosFirst();


	SysC.RandomRangeParticles.push_back(tmpClass);


	return SysC.RandomRangeParticles.size() - 1;
}

void DrawRandomRangeParticleSystem(int i) {
	SysC.RandomRangeParticles[i].ToDraw = true;

}

void RegenRRforRandomRange(int System) {

	SysC.RandomRangeParticles[System].RegenRRforRandomRange();

}

void AdjustRandomRangeParticleClass(int System, int elementCount, const std::array<olc::vf2d, 2> InitialToEnd, olc::Sprite* sprite, const olc::vf2d& scale = { 1.0f,1.0f }, olc::Pixel tint = olc::WHITE, std::array<float, 4> depth = { 0.0f, 0.0f, 0.0f, 0.0f }) {
	//if (System < SysC.TestParticles.size()) { <-- not using since I'd rather have the thing crash so I can later make some error handling... TODO: error handling with output message to console stuff! for ifdef debug mode
	SysC.RandomRangeParticles[System].Adjust(elementCount, InitialToEnd, sprite, scale, tint, depth);
	//}
}

void RandomRangeParticleClassChangeBlend(int System, const olc::DecalMode& mode) {

	SysC.RandomRangeParticles[System].ChangeBlend(mode);

}

//void RandomRangeParticleClassChangeTexture TODO: this

#pragma endregion

#pragma region TestParticleFuncs
int DX11CreateTestParticleSystem(const olc::vf2d& pos, olc::Sprite* sprite, const olc::vf2d& scale = { 1.0f,1.0f }, olc::Pixel tint = olc::WHITE, std::array<float, 4> depth = {0.0f, 0.0f, 0.0f, 0.0f}) { //returns int that relates to simple particle systems system index in system - you use this int to call simple particle system draw
	TestParticleClass tmpClass;

	tmpClass.BlendState = dxBlendStateDefault;
	tmpClass.sprite = sprite;
	tmpClass.tint[0] = tint.r;
	tmpClass.tint[1] = tint.g;
	tmpClass.tint[2] = tint.b;
	tmpClass.tint[3] = tint.a;

	olc::vf2d vInvScreenSize = {
		(1.0f / float(PL.ScreenWidth())),
		(1.0f / float(PL.ScreenHeight()))
	};

	olc::vf2d vScreenSpacePos =
	{
		(std::floor(pos.x) * vInvScreenSize.x) * 2.0f - 1.0f,
		((std::floor(pos.y) * vInvScreenSize.y) * 2.0f - 1.0f) * -1.0f
	};

	olc::vf2d vScreenSpaceDim =
	{
		vScreenSpacePos.x + (2.0f * (float(sprite->width) * vInvScreenSize.x)) * scale.x,
		vScreenSpacePos.y - (2.0f * (float(sprite->height) * vInvScreenSize.y)) * scale.y
	};



	tmpClass.w[0] = depth[0];
	tmpClass.w[1] = depth[1];
	tmpClass.w[2] = depth[2];
	tmpClass.w[3] = depth[3];

	tmpClass.pos[0] = { vScreenSpacePos.x, vScreenSpacePos.y };
	tmpClass.pos[1] = { vScreenSpacePos.x, vScreenSpaceDim.y };
	tmpClass.pos[2] = { vScreenSpaceDim.x, vScreenSpaceDim.y };
	tmpClass.pos[3] = { vScreenSpaceDim.x, vScreenSpacePos.y };


	tmpClass.uv[0] = { 0.0f, 0.0f };
	tmpClass.uv[1] = { 0.0f, 1.0f };
	tmpClass.uv[2] = { 1.0f, 1.0f };
	tmpClass.uv[3] = { 1.0f, 0.0f };

	for (int i = 0; i < 4; i++) {
		tmpClass.pVertexMem[i] = { {tmpClass.pos[i].x,tmpClass.pos[i].y, tmpClass.w[i]}, {tmpClass.uv[i].x, tmpClass.uv[i].y},{tmpClass.tint[0],tmpClass.tint[1],tmpClass.tint[2],tmpClass.tint[3]} };
	}

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	vertexBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_VERTEX_BUFFER;

	vertexBufferDesc.ByteWidth = sizeof(tmpClass.pVertexMem);
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

	D3D11_SUBRESOURCE_DATA resourceData;
	ZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	resourceData.pSysMem = tmpClass.pVertexMem;

	dxDevice->CreateBuffer(&vertexBufferDesc, &resourceData, &tmpClass.vb_quad);

	tmpClass.CreateTexture();

	SysC.TestParticles.push_back(tmpClass);

	return SysC.TestParticles.size() - 1;

}


void DrawTestParticleSystem(int i) {
	SysC.TestParticles[i].ToDraw = true;
	
}

void AdjustTestParticleClass(int System, const olc::vf2d& pos, const olc::vf2d& scale = { 1.0f,1.0f }, olc::Pixel tint = olc::WHITE, std::array<float, 4> depth = { 0.0f, 0.0f, 0.0f, 0.0f }) {
	//if (System < SysC.TestParticles.size()) { <-- not using since I'd rather have the thing crash so I can later make some error handling... TODO: error handling with output message to console stuff! for ifdef debug mode
	SysC.TestParticles[System].Adjust(pos, scale, tint, depth);
	//}
}
void TestParticleClassChangeBlend(int System, const olc::DecalMode& mode) {

	SysC.TestParticles[System].ChangeBlend(mode);

}

//void TestParticleClassChangeTexture TODO: this

#pragma endregion

void ProgramLink::DrawFuncMain() {

	if (pge->GetLayers()[currentLayer].bUpdate)
	{
		olc::renderer->UpdateTexture(pge->GetLayers()[currentLayer].nResID, pge->GetLayers()[currentLayer].pDrawTarget);
		pge->GetLayers()[currentLayer].bUpdate = false;
	}

	olc::renderer->DrawLayerQuad(pge->GetLayers()[currentLayer].vOffset, pge->GetLayers()[currentLayer].vScale, pge->GetLayers()[currentLayer].tint);

	//draw test particles
	for (int i = 0; i < SysC.TestParticles.size(); i++) {

		if (SysC.TestParticles[i].ToDraw == true) {

			SysC.TestParticles[i].Draw();
			SysC.TestParticles[i].ToDraw = false;

		}
	}

	//draw RandomRange particles
	for (int i = 0; i < SysC.RandomRangeParticles.size(); i++) {

		if (SysC.RandomRangeParticles[i].ToDraw == true) {

			SysC.RandomRangeParticles[i].Draw();
			SysC.RandomRangeParticles[i].ToDraw = false;

		}
	}

	for (auto& decal : pge->GetLayers()[currentLayer].vecDecalInstance)
		olc::renderer->DrawDecal(decal);
	pge->GetLayers()[currentLayer].vecDecalInstance.clear();
	
}

void InitializeParticlesWorker(olc::PixelGameEngine* pge) {

	pge->pgex_Register(&PL);

}

void ProgramLink::MoveParticleLayer(int i) {
	//move to layer if real - and enabled
	if (i < pge->GetLayers().size() && pge->GetLayers()[i].bShow==true) {

		pge->SetLayerCustomRenderFunction(currentLayer, nullptr); 

		currentLayer = i;

		pge->SetLayerCustomRenderFunction(currentLayer, DrawerHandle); 


	}

}

void MoveParticleLayer(int i) {
	PL.MoveParticleLayer(i); //user called function to move particle layer - you choose where to move
}

#endif
