/*TODO
	olcPGEX_DX11ShadersPlus.h

	+-------------------------------------------------------------+
	|         OneLoneCoder Pixel Game Engine Extension            |
	|                DX11 shaders Macro v0.1		              |
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
		dxDeviceContext->OMSetBlendState(dxBlendState, bState, 0xffffffff);

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
		"float2 tex : TEXCOORD;};\n"
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

#pragma region multiSprite

#pragma endregion

struct SystemsCollection {

	std::vector<TestParticleClass> TestParticles;

}SysC;


#pragma region TestParticleFuncs
int DX11CreateTestParticleSystem(const olc::vf2d& pos, olc::Sprite* sprite, const olc::vf2d& scale = { 1.0f,1.0f }, olc::Pixel tint = olc::WHITE, std::array<float, 4> depth = {0.0f, 0.0f, 0.0f, 0.0f}) { //returns int that relates to simple particle systems system index in system - you use this int to call simple particle system draw
	TestParticleClass tmpClass;

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

	SysC.TestParticles[System].Adjust(pos, scale, tint, depth);

}
#pragma endregion

void ProgramLink::DrawFuncMain() {

	if (pge->GetLayers()[currentLayer].bUpdate)
	{
		olc::renderer->UpdateTexture(pge->GetLayers()[currentLayer].nResID, pge->GetLayers()[currentLayer].pDrawTarget);
		pge->GetLayers()[currentLayer].bUpdate = false;
	}

	olc::renderer->DrawLayerQuad(pge->GetLayers()[currentLayer].vOffset, pge->GetLayers()[currentLayer].vScale, pge->GetLayers()[currentLayer].tint);

	for (int i = 0; i < SysC.TestParticles.size(); i++) {

		if (SysC.TestParticles[i].ToDraw == true) {

			SysC.TestParticles[i].Draw();
			SysC.TestParticles[i].ToDraw = false;

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
