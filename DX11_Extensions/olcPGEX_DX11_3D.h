/*TODO
	olcPGEX_DX11_3D.h

	+-------------------------------------------------------------+
	|         OneLoneCoder Pixel Game Engine Extension            |
	|                DX11 3d extension v0.26   	                  |
	+-------------------------------------------------------------+

	What is this?
	~~~~~~~~~~~~~
	This is an extension to the olcPixelGameEngine, which provides
	basic 3d with DX11... YAY - more DX for all! - maybe I'll even add FSR :smile:

	Use of this extension requires the olcPixelGameEngine-DX11-Fork's olcPixelGameEngine header

	ALSO: for FBX export from blender, turn off normals and uv, ect if not needed - these are extra duplicate verticies normally

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

// 
// 
// 
// 
// 
// //TODO: animation.add(std::string "FilePath", M3DR my Model) to add fbx files with animation but no mesh to end of anim
// 
// TODO: normals with bone Math for lighting 
// 
// TODO: load mat data for in game-engine lighting 
// 
// TODO: add "more textures" - make texture handler a std::vector defaulted every 4 element to black single pix - make the shader have a fixed texture array with 4 textures - pass all that to shader - when loading fbx it also checks same dir for tex - when dealing with textures add a parameter that is an int control - "which tecture from 0-5 do you want to change" -  ALSO, need to load during indice load the current texture of each indice to seperate how many tex are applied - in the pix shader I pass a resource dedicated to holding "current indice texture number" from 0-4
// maybe raise tex array size? -- thing is, I need to pass a texture int to each vertex loaded from each V shader
// 
// TODO: cycle through animation function - with toggle bool for update bone dat, which then updates bone data accordingly
//
// TODO: test and fix MSRBone
// 
// 
// TODO: lighting
// 
// // // TODO: fix big anim problem; program binds animation with weight directly to bone with weight equally
// 
//
// // TODO: re add indice optimiser option - only static models? - no idea if it is broken - maybe its bone load? double loading?
// 
// 
//TODO: add boolean pointer to lerp functions that toggles false when done lerping for that bool if not nullptr, else true if running <-- add run through animation function - same as lerp but does animation funny buissness
// 
//TODO: with lights handle normals properly - I loaded them to a buffer but 
//TODO: lights - have boolean which toggles light shader binding stuff on or off [have fastLight int toggle where type is 0-... ?] (changes pixel shader and constant buffer with boolean - last of the buffers are for light(s)?) - (light shaders are binded at the start of 3d Model render phase,at the start of Draw Func in case many layers and to bind less
//
//make default for 2d model pre programmed in case people use it for sprites (like sprites)
//TODO: blend state options for models - individual - then make billboard?!?!
//TODO: mip's
//TODO: billboard plane for 2d but acts like 3d with cam
//TODO: particle systems
//TODO: chain lerp function for linking lerps
//TODO: make work with animated decals
//
#pragma once

#define TINYOBJLOADER_IMPLEMENTATION
//#include "tiny_obj_loader.h"
#include "ofbxMini.h"
#include <algorithm>
#include <functional>

#if defined(OLC_PGEX_DIRECTX11_3D)

namespace DOLC11 {

	float MyPi = 3.141592653;

	using namespace DirectX;

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

	struct VertexBoneData {
		std::vector<float> IDs; //float for predictable shader pass
		std::vector<float> weights;

		VertexBoneData() {

			IDs.reserve(5);
			weights.reserve(5);

		}

	};

	struct TopBoneIDs {


		INT32 id[4] = { 0,0,0,0 };

		void ZeroID() {
			for (int i = 0; i < sizeof(id) / sizeof(INT32); i++) {
				id[i] = 0;
			}
		}
	};

	struct TopBoneWeight {
		float w[4] = { 0,0,0,0 };

		void ZeroW() {
			for (int i = 0; i < sizeof(w) / sizeof(float); i++) {
				w[i] = 0;
			}
		}
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

	std::vector< DataLerpFunc > LerpCamFunc;
	//std::vector< DataLerpFunc > LerpCamRotFunc;

	std::vector< DataLerpFunc > LerpModelFunc;

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

		TopBoneIDs tbi; //uint 4
		TopBoneWeight tbw; //float 4

	};
}

namespace DOLC11{

	struct ObjTuneStatReg { //TODO: make scale and translate work in shader - make obj visible
		std::array<float, 3> Translate = { 0.0f,0.0f,0.0f }; //not using xm float, so sad... :(
		float pad1 = 0.0f;
		std::array<float, 3> Scale = { 1.0f,1.0f,1.0f };
		float pad2 = 0.0f;
		XMFLOAT4 Quat = { 0.0f,0.0f,0.0f,0.0f };
				//XMFLOAT4 pad2 = {0.0f,0.0f,0.0f,0.0f};
	};

//	struct BoneInfoStruct {
		//UINT ID;
//		 t_m; //trans matrix
//		XMFLOAT4X4 t_l_m; //trans link matrix
//	};

	void SeperateTransformationMatrix(XMFLOAT4X4* Im, ObjTuneStatReg* transformDat) {
		//TODO: matrix rot and stuff extraction
	}
	XMFLOAT4X4 ofbxMatToXM(ofbx::Matrix* TMPtm) {
		return XMFLOAT4X4( static_cast<float>(TMPtm->m[0]), static_cast<float>(TMPtm->m[1]), static_cast<float>(TMPtm->m[2]), static_cast<float>(TMPtm->m[3]), static_cast<float>(TMPtm->m[4]), static_cast<float>(TMPtm->m[5]), static_cast<float>(TMPtm->m[6]), static_cast<float>(TMPtm->m[7]), static_cast<float>(TMPtm->m[8]), static_cast<float>(TMPtm->m[9]), static_cast<float>(TMPtm->m[10]), static_cast<float>(TMPtm->m[11]), static_cast<float>(TMPtm->m[12]), static_cast<float>(TMPtm->m[13]), static_cast<float>(TMPtm->m[14]), static_cast<float>(TMPtm->m[15]) );
	}
	ofbx::Matrix XMToofbxMat(XMFLOAT4X4* TMPtm) {
		return { TMPtm->m[0][0],TMPtm->m[0][1],TMPtm->m[0][2],TMPtm->m[0][3],TMPtm->m[1][0],TMPtm->m[1][1],TMPtm->m[1][2],TMPtm->m[1][3] ,TMPtm->m[2][0],TMPtm->m[2][1],TMPtm->m[2][2],TMPtm->m[2][3] ,TMPtm->m[3][0],TMPtm->m[3][1],TMPtm->m[3][2],TMPtm->m[3][3] };
		
	}


	std::array<float, 3> GetTranslate(ObjTuneStatReg* ObjTune) {
		olc::vf2d vInvScreenSize = {
	(1.0f / float(PL.ScreenWidth())),
	(1.0f / float(PL.ScreenHeight()))
		};
		return std::array<float, 3> { ToPGESpace(&vInvScreenSize.x, &ObjTune->Translate[0]), ToPGESpace(&vInvScreenSize.y, &ObjTune->Translate[1]), ToPGESpace(&vInvScreenSize.x, &ObjTune->Translate[2]) };
	}

	std::array<float, 3> GetScale(ObjTuneStatReg* ObjTune) {
		return ObjTune->Scale;
	}
	std::array<float, 4> GetQuaternion(ObjTuneStatReg* ObjTune) {
		return std::array<float, 4> {ObjTune->Quat.x, ObjTune->Quat.y, ObjTune->Quat.z, ObjTune->Quat.w};
	}
	std::array<float, 3> GetRadians(ObjTuneStatReg* ObjTune) { //ObjTune.Quat
		float quatDiv = sqrt(1 - ObjTune->Quat.w * ObjTune->Quat.w);
		return std::array<float, 3> {(ObjTune->Quat.x / quatDiv), (ObjTune->Quat.y / quatDiv), (ObjTune->Quat.z / quatDiv)};
	}

	ofbx::Vec3 GetDataFromAnimation(double time, const ofbx::AnimationCurveNode* node) {

		return node->getNodeLocalTransform(time); //every of these calls it recomputes getLocalTransform TODO : check why time not working

	}

	

	struct Joint {
		int id; // index/ID of bone
		std::string name;
		const ofbx::Object* Bone;

		std::vector <const ofbx::AnimationCurveNode*> tNode;
		std::vector <const ofbx::AnimationCurveNode*> rNode;
		std::vector <const ofbx::AnimationCurveNode*> sNode;

		XMMATRIX LBT;
		XMMATRIX InvBP = XMMatrixIdentity();
		XMMATRIX BP = XMMatrixIdentity();
		XMMATRIX SkinM = XMMatrixIdentity();

		XMMATRIX BoneOffsetMatrix;

		std::vector<std::shared_ptr<Joint>> children;
		std::shared_ptr<Joint> parent;

		void GetInvBindT(XMMATRIX parentBindT) {
			
			XMMATRIX BT = parentBindT * LBT;

			BP = BT;
			InvBP = XMMatrixInverse(nullptr, BT);
			SkinM = XMMatrixTranspose(BP * InvBP);

			for (int i = 0; i < children.size(); i++) {
				children[i]->GetInvBindT(LBT);
			}
		}



	};


	struct M3DR { //3d model with all data - I need seperate obj loader - regular model format

		//FOR ANIMATION export in blender have deform and no leaf bones

		std::vector<Joint> Bones;
		const ofbx::Object* rootObj;
		
		XMMATRIX globalInverseTransform;
		XMMATRIX globalTransform;

		std::vector<XMFLOAT4X4> BoneDataTLMA; //adjusted values from animation


		bool ToUpdateCBoneBuf = false;
		
		//TODO: make this a bitmap
		std::map<std::string, int> animNameS; //contains anim position in stack
		std::map<int, std::string> animNameI; //contains anim position in stack reverse
		
		std::vector< const ofbx::AnimationStack* > animStack;
		std::vector< float > animStackMaxTime;
		std::vector< bool > AnimVCacheMade; //VCache is more so a Bone Cache*
		std::vector< std::vector< std::vector<XMFLOAT4X4> > > AnimVCacheData; //per anim, per interval - I have a set of bones


		std::vector<XMMATRIX> animDat; //animation collection
		
		std::vector<const ofbx::Cluster*> ClusterObject;
		
		std::vector<VertexBoneData> VboneDat; //TODO: filter to same indices instead of including duplicate verticies part

		ObjTuneStatReg ObjTune;

		ID3D11Buffer* CBuf;

		ID3D11Buffer* ArmatureCBuf;
	
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

		bool MakeAnimVCache(int TotalIntervals, int AnimNum) {
			if (AnimNum >= animStack.size()) { return false; }

			else {
				std::vector<XMFLOAT4X4> currBoneDat = BoneDataTLMA;
				AnimVCacheMade[AnimNum] = true;

				float interval = animStackMaxTime[AnimNum]/TotalIntervals;
				int numPass = 0;
				float CurrI = 0.0f;
				AnimVCacheData[AnimNum].resize(TotalIntervals);
				for (int i = 0; i < AnimVCacheData[AnimNum].size(); i++) {
					AnimVCacheData[AnimNum][i].resize(Bones.size());
				}
				while (numPass < TotalIntervals) {

					for (int i = 0; i < Bones.size(); i++) {
						animDat[i] = GetBoneLocalOfAnim(CurrI, i, AnimNum);

					}

					ApplyLocalOfAnim(XMMatrixIdentity(), 0);

					for (int x = 0; x < BoneDataTLMA.size(); x++) {
						AnimVCacheData[AnimNum][numPass][x] = BoneDataTLMA[x];
					}

					CurrI += interval;
					numPass += 1;
				}

				BoneDataTLMA = currBoneDat; //reset bone dat to what it was before incase - stop funky behavior

			}
		}

		float EndTimeOfAnim(int Anim) {
			return animStackMaxTime[Anim];
		}

		void AddBoneChildren(Joint* BoneP) {
			for (int i = 0; i < Bones.size(); i++) {

				if (Bones[i].Bone->getParent() == BoneP->Bone) {
					BoneP->children.push_back(std::make_shared<Joint>(Bones[i]));
				}

			}
		}

		void SetAllBonesChildren() {

			for (int i = 0; i < Bones.size(); i++) {
				AddBoneChildren(&Bones[i]);
			}

		}
		void SetAllBonesParent() {

			for (int i = 0; i < Bones.size(); i++) {
				for (int ii = 0; ii < Bones.size(); ii++) {
					if (Bones[i].Bone->getParent() == Bones[ii].Bone) {
						Bones[i].parent = std::make_shared<Joint>(Bones[ii]);
					}
				}
			}

		}
		void SetAllBonesGetInvBindT() {
			if (Bones.size() > 0) {
				Bones[0].GetInvBindT(XMMatrixIdentity());
			}
		}


		void ReCalcBonesAhead(std::array<float, 3> t, std::array<float, 3> r, std::array<float, 3> s, int BoneParent) {

			ReCalcBonesAheadLogic(t, r, s, BoneParent);

			//Bones[0].GetAnimInvBindT(Bones[0].AnimLBT); //calc
		
			//BindAnimPoseBones();
			
		}

		void ReCalcBonesAheadLogic(std::array<float,3> t, std::array<float, 3> r, std::array<float, 3> s, int BoneParent) { //TODO: fix this bone calculator as a whole and anims

			SetBoneDataWithCurrent(t, r, s, BoneParent); //set
					

			for (int i = 0; i < Bones[BoneParent].children.size(); i++) {
				
				ReCalcBonesAheadLogic(t, r, s, Bones[BoneParent].children[i]->id);

			}


		}

		void MSRRootBone(std::array<float, 3> t = { 0.0f,0.0f,0.0f }, std::array<float, 3> s = { 1.0f,1.0f,1.0f }, std::array<float, 3> r = { 0.0f,0.0f,0.0f }) {
			//TODO: see if I need to make a root finder - in case many armature - instead of choose 0
			MSRBone(0, t, s, r);

		}

		void MSRBone(int Bone, std::array<float, 3> XYZTranslate = { 0.0f,0.0f,0.0f }, std::array<float, 3> scale = { 1.0f,1.0f,1.0f }, std::array<float, 3> r = { 0.0f,0.0f,0.0f }) {
			if (Bone < Bones.size()) {

				ToUpdateCBoneBuf = true;


				olc::vf2d vInvScreenSize = {
			(1.0f / float(PL.ScreenWidth())),
			(1.0f / float(PL.ScreenHeight()))
				};

				std::array<float,3> boneT = std::array<float, 3>{ToNotPGESpace(&vInvScreenSize.x, &XYZTranslate[0]), ToNotPGESpace(&vInvScreenSize.y, &XYZTranslate[1]), ToNotPGESpace(&vInvScreenSize.x, &XYZTranslate[2]) };

				r[0] = r[0] * 180 / MyPi;
				r[1] = r[1] * 180 / MyPi;
				r[2] = r[2] * 180 / MyPi;

				ReCalcBonesAhead(boneT,r,scale,Bone);
			}

		}

		int GetAnimationNum(std::string name) {
			return animNameS[name];
		}
		std::string GetAnimationStr(int num) {
			animNameI[num];
		}
		std::vector<std::pair<std::string, int>> GetAllAnims() {
			std::vector<std::pair<std::string, int>> tmp;
			for (const auto& i : animNameS) {
				tmp.push_back(i);
			}
			return tmp;
		}

		void SetBoneDataWithCurrent(std::array<float, 3> t, std::array<float, 3> r, std::array<float, 3> s, int Bone) {
			
			ofbx::Vec3 tv = { t[0],t[1],t[2] };
			ofbx::Vec3 rv = { r[0],r[1],r[2] };
			ofbx::Vec3 sv = { s[0],s[1],s[2] };

			ofbx::Matrix sm = ofbx::makeIdentity();
			sm.m[0] = sv.x;
			sm.m[5] = sv.y;
			sm.m[10] = sv.z;
			
			ofbx::Matrix finalM = ofbx::getRotationMatrix(rv, ofbx::RotationOrder::EULER_XYZ)*sm;
			setTranslation(tv,&finalM);
			
			//ofbx::Matrix finalM = Bones[Bone].Bone->evalLocal({ t[0],t[1],t[2] }, {r[0], r[1],r[2]}, { s[0],s[1],s[2] });

			XMFLOAT4X4 tmp = ofbxMatToXM(&finalM);

			XMMATRIX tmp1 = XMLoadFloat4x4(&tmp);

			XMMATRIX tmp2 = XMLoadFloat4x4(&BoneDataTLMA[Bone]);

			XMStoreFloat4x4(&BoneDataTLMA[Bone], (tmp1*tmp2));

			
			//tmp2 += tmp1*tmp2;

			//XMStoreFloat4x4(&BoneDataTLMA[Bone], tmp2);
		}

		void ResizeAllBonesNodeArr() {
			for (int i = 0; i < Bones.size(); i++) {
				Bones[i].tNode.resize(animStack.size());
				Bones[i].rNode.resize(animStack.size());
				Bones[i].sNode.resize(animStack.size());
			}
		}

		void GetAllBoneNodes() {
			for (int i = 0; i < Bones.size(); i++) {
				GetAllLTRSNodeForEveryAnim(i);
			}
		}

		void GetAllLTRSNodeForEveryAnim(int Bone) {
			for (int i = 0; i < animStack.size(); i++) {
				GetLTRSNode(Bone, i);
			}
		}

		void GetLTRSNode(int Bone, int Anim) {
			Bones[Bone].tNode[Anim] = GetNodeCurve(Anim, "Lcl Translation", Bones[Bone].Bone);
			Bones[Bone].rNode[Anim] = GetNodeCurve(Anim, "Lcl Rotation", Bones[Bone].Bone);
			Bones[Bone].sNode[Anim] = GetNodeCurve(Anim, "Lcl Scaling", Bones[Bone].Bone);
		}

		std::array<ofbx::Vec3, 3> GetLocalAnimData(double time, int Bone, int Anim) {
			ofbx::Vec3 t = { 0,0,0 };
			ofbx::Vec3 r = { 0,0,0 };
			ofbx::Vec3 s = { 1,1,1 };

			//TODO: cache these nodes


			if (Bones[Bone].tNode[Anim] != nullptr) {
				t = GetDataFromAnimation(time, Bones[Bone].tNode[Anim]);
			}
			if (Bones[Bone].rNode[Anim] != nullptr) {
				r = GetDataFromAnimation(time, Bones[Bone].rNode[Anim]);
			}
			if (Bones[Bone].sNode[Anim] != nullptr) {
				s = GetDataFromAnimation(time, Bones[Bone].sNode[Anim]);
			}

			return std::array<ofbx::Vec3, 3>{t,r,s};

		}

		ofbx::Matrix GetLocalAnimDataMat(double time, int Bone, int Anim) {

			std::array<ofbx::Vec3, 3> tmp = GetLocalAnimData(time, Bone, Anim);
			
			ofbx::Matrix finalM = Bones[Bone].Bone->evalLocal(tmp[0], tmp[1], tmp[2]);

			return finalM;
		}
		ofbx::Matrix GetGlobalAnimData(double time, int BonePosRoot, int Anim) {
			
			ofbx::Matrix tmp = GetLocalAnimDataMat(time, BonePosRoot, Anim);

			if (Bones[BonePosRoot].Bone->getParent()) {
				ofbx::Matrix tmp2 = GetGlobalAnimData(time, Bones[BonePosRoot].parent->id, Anim);
				tmp = tmp * tmp2; //OR do I get parent of parent global matrix?
			}


			return tmp;
		}

		const ofbx::AnimationCurveNode* GetNodeCurve(int Anim, std::string valueType, const ofbx::Object* Bone) {
			
			
				for (int ii = 0; animStack[Anim]->getLayer(ii); ++ii)
				{

					const ofbx::AnimationLayer* layer = animStack[Anim]->getLayer(ii);

					if (layer->getCurveNode(*Bone, valueType.c_str())) {
						return layer->getCurveNode(*Bone, valueType.c_str());
						
					}
					else {
						return nullptr;
					}
				}
		}
		std::array<std::array<float, 3>, 3> Vec33ToArrF3(std::array<ofbx::Vec3, 3> t) {
			return std::array<std::array<float, 3>, 3> { std::array<float, 3>{float(t[0].x), float(t[0].y), float(t[0].z)}, std::array<float, 3>{float(t[1].x), float(t[1].y), float(t[1].z)}, std::array<float, 3>{float(t[2].x), float(t[2].y), float(t[2].z)}};
		}
		std::array < std::array<float, 3>, 3> GetLocalAnimDataA(double time, int Bone, int Anim) {
			return Vec33ToArrF3(GetLocalAnimData(time, Bone, Anim));
		}

		void ApplyLocalOfAnim(XMMATRIX Parent,int Bone) {
			XMMATRIX currTrans = animDat[Bone] * Parent ;
			
			
			for (int i = 0; i < Bones[Bone].children.size(); i++) {
				ApplyLocalOfAnim(currTrans, Bones[Bone].children[i]->id);
			}

			XMStoreFloat4x4(&BoneDataTLMA[Bone], XMMatrixTranspose( (currTrans) * (Bones[Bone].BoneOffsetMatrix)));
			
		}

		XMMATRIX GetBoneLocalOfAnim(double time, int BonePos, int Anim) {
			
			ofbx::Matrix finalM = GetLocalAnimDataMat(time, BonePos, Anim);
			XMFLOAT4X4 tmp = ofbxMatToXM(&finalM);
			return XMLoadFloat4x4(&tmp);

		}
		int MaxLayer() {
			return animStack.size();
		}

		void ApplyAnimVCache(double time, int AnimNum) {
			float Interval = animStackMaxTime[AnimNum]/ AnimVCacheData[AnimNum].size();
			
			int CurrInterval = std::floor(time / Interval);
		
			for (int x = 0; x < BoneDataTLMA.size(); x++) {
				BoneDataTLMA[x] = AnimVCacheData[AnimNum][CurrInterval][x];
			}
		}

		int SetAllBoneToAnim( double time, int num, bool UseVCache) { 
			if (num >= animStack.size()) { return 0; }

			ToUpdateCBoneBuf = true;

			if (UseVCache == true && AnimVCacheMade[num] == true) {
				ApplyAnimVCache(time, num);
			}
			else {
				
				for (int i = 0; i < Bones.size(); i++) {
					animDat[i] = GetBoneLocalOfAnim(time, i, num);
				}

				ApplyLocalOfAnim(XMMatrixIdentity(), 0);
			}
			return 1;
		}
		int SetAllBoneToAnim(double time, std::string name, bool UseVCache) {
			if (animNameS.count(name) == 0) { return 0; }
			
			SetAllBoneToAnim(time, animNameS[name], UseVCache);
			
			return 1;
		}

		int SetBoneToBindPose() {
			if (Bones.size() > 0) {
				return 0;
			}
			else {
				BindPoseBones();
				return 1;
			}
		}

		void FillTopBones() { //indice copies bone if same
			//if I need to do vertex all I have a setup from old git history
			std::map<int, int> sDubC; //shuffle doubles calc - shuffle through numbered index to reduce sort

			for (int i = 0; i < Indice.size(); i++) {
				if (sDubC.count(Indice[i]) == 0) {
					sDubC[Indice[i]] = i;



						if (VboneDat[Indice[i]].weights.size() < 4) {
							VboneDat[Indice[i]].weights.resize(4); VboneDat[Indice[i]].IDs.resize(4);
							for (int xx = 0; xx < VboneDat[Indice[i]].weights.size(); xx++) {
								if (VboneDat[Indice[i]].weights[xx] ==	0) {
									VboneDat[Indice[i]].weights[xx] = 0.0f;
									VboneDat[Indice[i]].IDs[xx] = 0;
								}
							}
						}
						
						float tmpWH = 0.0f;
						int tmpIDH = 0;

						for (int ii = 0; ii < VboneDat[Indice[i]].weights.size(); ii++) { //TODO: test if this max sorter 
							for (int iii = VboneDat[Indice[i]].weights.size()-1; iii > ii; iii--) { //TODO: test if this max sorter 

								if (VboneDat[Indice[i]].weights[iii] > VboneDat[Indice[i]].weights[ii]) {
									tmpWH = VboneDat[Indice[i]].weights[ii];
									VboneDat[Indice[i]].weights[ii] = VboneDat[Indice[i]].weights[iii];
									VboneDat[Indice[i]].weights[iii] = tmpWH;

									tmpIDH = VboneDat[Indice[i]].IDs[ii];
									VboneDat[Indice[i]].IDs[ii] = VboneDat[Indice[i]].IDs[iii];
									VboneDat[Indice[i]].IDs[iii] = tmpIDH;

									ii = 0;
									iii = VboneDat[Indice[i]].weights.size()-1;
								}
							}
						}
						modelDat[Indice[i]].tbw.w[0] = VboneDat[Indice[i]].weights[0];
						modelDat[Indice[i]].tbw.w[1] = VboneDat[Indice[i]].weights[1];
						modelDat[Indice[i]].tbw.w[2] = VboneDat[Indice[i]].weights[2];
						modelDat[Indice[i]].tbw.w[3] = VboneDat[Indice[i]].weights[3];

						modelDat[Indice[i]].tbi.id[0] = VboneDat[Indice[i]].IDs[0];
						modelDat[Indice[i]].tbi.id[1] = VboneDat[Indice[i]].IDs[1];
						modelDat[Indice[i]].tbi.id[2] = VboneDat[Indice[i]].IDs[2];
						modelDat[Indice[i]].tbi.id[3] = VboneDat[Indice[i]].IDs[3];

					}

				//}
				

			}
			
			for (const std::pair<int, int>& kv : sDubC) { //TODO: rationalize weights to 1, else pos is bad

				float max = 0.0f;

				for (int ii = 0; ii < 4; ii++) {
					max += modelDat[kv.first].tbw.w[ii];;
				}

				if (max > 0) { //to stop error of 1
					for (int ii = 0; ii < 4; ii++) {
						modelDat[kv.first].tbw.w[ii] = modelDat[kv.first].tbw.w[ii] / max;
					}
				}



			}
		}

		void CreateArmatureCBuf() {
			

			if (BoneDataTLMA.size() == 0) {
				XMFLOAT4X4 tmpForFill = {0.0f,0.0f ,0.0f ,0.0f ,0.0f ,0.0f ,0.0f ,0.0f ,0.0f ,0.0f ,0.0f ,0.0f ,0.0f ,0.0f ,0.0f ,0.0f };

				BoneDataTLMA.push_back(tmpForFill);
			}
			/*
			if (BoneDataTLM.size() == 0) {
				XMFLOAT4X4 tmpForFill = { 0.0f,0.0f ,0.0f ,0.0f ,0.0f ,0.0f ,0.0f ,0.0f ,0.0f ,0.0f ,0.0f ,0.0f ,0.0f ,0.0f ,0.0f ,0.0f };

				BoneDataTLM.push_back(tmpForFill);
			}
			if (BoneDataTM.size() == 0) {
				XMFLOAT4X4 tmpForFill = { 0.0f,0.0f ,0.0f ,0.0f ,0.0f ,0.0f ,0.0f ,0.0f ,0.0f ,0.0f ,0.0f ,0.0f ,0.0f ,0.0f ,0.0f ,0.0f };

				BoneDataTM.push_back(tmpForFill);
			}*/

			D3D11_BUFFER_DESC bufDesc;
			ZeroMemory(&bufDesc, sizeof(bufDesc));
			bufDesc.Usage = D3D11_USAGE_DEFAULT;
			bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufDesc.CPUAccessFlags = 0;
			bufDesc.ByteWidth = sizeof(XMFLOAT4X4) * BoneDataTLMA.size(); //for now max 64 bones
			//bufDesc.StructureByteStride = sizeof(XMFLOAT4X4);

			dxDevice->CreateBuffer(&bufDesc, nullptr, &ArmatureCBuf);
			
			if (BoneDataTLMA.size() != 0) {
				dxDeviceContext->UpdateSubresource(ArmatureCBuf, 0, nullptr, &BoneDataTLMA[0], 0, 0);
			}
			else {

			}
		}


		void updateArmatureCBuf() {
			dxDeviceContext->UpdateSubresource(ArmatureCBuf, 0, nullptr, &BoneDataTLMA[0], 0, 0);
		}

		void CheckToUpdateArmatureCBuf() {
			if (ToUpdateCBoneBuf == true) { updateArmatureCBuf(); ToUpdateCBoneBuf = false; }
		}

		std::array<float, 3> Translate() {
			return GetTranslate(&ObjTune);
		}

		std::array<float, 3> Scale() {
			return GetScale(&ObjTune);
		}
		std::array<float, 4> Quaternion() {
			return GetQuaternion(&ObjTune);
		}
		std::array<float, 3> Radians() { //ObjTune.Quat
			return GetRadians(&ObjTune);
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
		void SetTexEqual(olc::Decal* Tex) { //SETS THE TEXTURE EXACTLY TO THE SPECIFIED DECAL, THIS IS HOW YOU INTERPOLATE THIS PROGRAM TO USE ANIMATED DECAL.h THING ECT;
			SafeRelease(Tex1R);

			olc::DecalTSR[Tex->id]->QueryInterface(IID_ID3D11Texture2D, (void**)&Tex1R);
			Tex1SRV = olc::DecalTSV[Tex->id];
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

		/*
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
		*/

		void LoadFBXFile(std::string path) {
			FILE* fp;

			fopen_s(&fp, path.c_str(), "rb");

			if (!fp) { std::cout << "no file found at fbx path"; }
			else {
				fseek(fp, 0, SEEK_END);
				long file_size = ftell(fp);
				fseek(fp, 0, SEEK_SET);
				ofbx::u8* content = new ofbx::u8[file_size];
				fread(content, 1, file_size, fp);

				ofbx::IScene* g_scene = ofbx::load((ofbx::u8*)content, file_size, (ofbx::u64)ofbx::LoadFlags::TRIANGULATE);
			
				modelDat.clear();
				Indice.clear();
				VboneDat.clear();
				Bones.clear();
				BoneDataTLMA.clear();
				animDat.clear();
				animStack.clear();

				VNT tmpV;
				
				XMFLOAT3 TmpPos;
				XMFLOAT3 TmpNor;
				XMFLOAT2 TmpTex;
				UINT TmpInd;

				int obj_idx = 0;
				int indices_offset = 0;
				int normals_offset = 0;
				int mesh_count = g_scene->getMeshCount();
//				std::map<std::tuple<std::tuple<float, float, float>,std::tuple<float, float>>, int> b;
				
				rootObj = g_scene->getRoot();
				ofbx::Matrix emp = rootObj->getGlobalTransform();

				XMFLOAT4X4 tmpgl = ofbxMatToXM(&emp);

				globalTransform = XMLoadFloat4x4(&tmpgl);

				globalInverseTransform = XMMatrixInverse(nullptr, globalTransform);

				for (int i = 0; i < mesh_count; i++)
				{
					//b.clear();

					const ofbx::Mesh& mesh = *g_scene->getMesh(i);
					const ofbx::Geometry& geom = *mesh.getGeometry();
					int vertex_count = geom.getVertexCount();
					const ofbx::Vec3* vertices = geom.getVertices();
					const ofbx::Vec2* uvs = geom.getUVs();
					const int indiceC = geom.getIndexCount();
					const ofbx::Vec3* normals = geom.getNormals();
					const ofbx::Pose* cPose = mesh.getPose();
	//				std::map<int, std::tuple<std::tuple<float, float, float>, std::tuple<float, float>>> bA;
			//POS NORMAL  TEX

					
					for (int ii = 0; ii < vertex_count; ii++)
					{
						
						tmpV.Position = { static_cast<float>(vertices[ii].x), static_cast<float>(vertices[ii].y), static_cast<float>(vertices[ii].z) };



						if (normals != nullptr) {
							tmpV.Normal = { static_cast<float>(normals[ii].x),static_cast<float>(normals[ii].y),static_cast<float>(normals[ii].z) };
						}
						else {
							tmpV.Normal = { 0.0f,0.0f,0.0f };
						}

						if (uvs != nullptr) {
							tmpV.Tex = { static_cast<float>(uvs[ii].x),static_cast<float>(uvs[ii].y) };
						}
						else {
							tmpV.Tex = { 0.0f,0.0f };
						}

//						std::tuple < std::tuple<float, float, float>, std::tuple<float, float>> tmpTup = std::make_tuple(std::make_tuple(tmpV.Position.x, tmpV.Position.y, tmpV.Position.z), std::make_tuple(tmpV.Tex.x, tmpV.Tex.y));

						//TODO: redo incide stuff for bones to work
			//			if (b.count(tmpTup) == 0) {

//							b[tmpTup] = modelDat.size();

							modelDat.push_back(tmpV);
			//			}

						Indice.push_back(Indice.size());

//						Indice.push_back(b[tmpTup]);
			//			bA[Indice.size()] = tmpTup;
					}

					VboneDat.resize(modelDat.size());
					
					const ofbx::Skin* skin = geom.getSkin();
					if (skin) {
						int gctR = skin->getClusterCount();
						for (int ii = 0; ii < gctR; ii++)
						{
								
								const ofbx::Cluster* cluster = skin->getCluster(ii);

								if (cluster) {
									int indiceCount = cluster->getIndicesCount();
									//int counter = 0;
									if (indiceCount > 0) {
										const int* indList = cluster->getIndices();
										const double* tmpW = cluster->getWeights();

//										std::map<int, bool> GotA; //
										for (int iii = 0; iii < indiceCount; iii++) {
//											if (GotA[b[bA[indList[iii]]]] == false) { //get indice with bones - but limit to once per indice, all i's share - filter :thumbup:
											VboneDat[indList[iii]].weights.push_back(static_cast<float>(tmpW[iii]));
											VboneDat[indList[iii]].IDs.push_back(ii); //bone id

//												VboneDat[b[bA[indList[iii]]]].weights.push_back(static_cast<float>(tmpW[iii]));
//												VboneDat[b[bA[indList[iii]]]].IDs.push_back(ii); //bone id
//												GotA[b[bA[indList[iii]]]] = true;
//											}
										}
										//counter += 1;
										//tmpForFill.ID = i; //index is ID of bone
									}
									ofbx::Matrix TMPtm = cluster->getTransformMatrix();

									
									ofbx::Matrix TMPtlm = cluster->getTransformLinkMatrix(); //correct matrix

									ofbx::Matrix GeoTrans = mesh.getGeometricMatrix();

									ofbx::Matrix ModelMatrix = cluster->getLink()->getGlobalTransform();

									ofbx::Matrix LocalMatrix = cluster->getLink()->getLocalTransform();

									XMFLOAT4X4 XMIB = ofbxMatToXM(&GeoTrans);
									XMMATRIX tmpMMM = XMLoadFloat4x4(&XMIB);
							//		ofbx::Matrix linkGeo = cluster->getLink()->getGlobalTransform();


									

									ClusterObject.push_back(cluster);
									//TMPtlm = GeoTrans* TMPtlm;
									//TMPtm = TMPtm*linkGeo;
									
									XMFLOAT4X4 tmpTMtmp = ofbxMatToXM(&LocalMatrix);
									XMMATRIX tmpTM = XMLoadFloat4x4(&tmpTMtmp);

									XMFLOAT4X4 tmpGtmp = ofbxMatToXM(&ModelMatrix);
									XMMATRIX tmpG = XMLoadFloat4x4(&tmpGtmp);

									
									XMFLOAT4X4 tmpTLMF = ofbxMatToXM(&TMPtlm);
									XMMATRIX tmpTLMM = XMLoadFloat4x4(&tmpTLMF);
									tmpTLMM = XMMatrixInverse(nullptr,tmpTLMM);

									XMFLOAT4X4 tmptmpTLMMma = ofbxMatToXM(&TMPtm);
									XMMATRIX tmpTLMMma = XMLoadFloat4x4(&tmptmpTLMMma);

									//
									
									Joint nJ;
									nJ.Bone = cluster->getLink();
									nJ.name = nJ.Bone->name;
									nJ.id = Bones.size();
									nJ.LBT = (tmpTM)* XMMatrixInverse(nullptr, tmpG);
									nJ.BoneOffsetMatrix = tmpTLMMma * tmpMMM;
									Bones.push_back(nJ);
								}
						}
					}
				}

				BoneDataTLMA.resize(Bones.size());
				animDat.resize(Bones.size());

				int animCount = g_scene->getAnimationStackCount();
				
				for (int i = 0; i < animCount; i++)  //TODO: aimation stack i goes out of range
				{
					const ofbx::AnimationStack* astack = g_scene->getAnimationStack(i);

					animStack.push_back(astack);
					animNameI[i] = astack->name;
					animNameS[astack->name] = i;
				}

			//load data from g_scene now
			}

			fclose(fp);
		}

		/*
		void MakeVertexGlobal(XMMATRIX* VERTEX, ) {

			XMMATRIX tmp = XMLoadFloat4x4(&rootObjM);
			for (int i = 0; i < modelDat.size(); i++) {

				XMVECTOR tmp2 = XMLoadFloat3(&modelDat[i].Position);

				tmp2 = XMVector3Transform(tmp2,tmp);
				
				XMStoreFloat3(&modelDat[i].Position, tmp2);
			}
		}
		*/

		XMMATRIX GetWorldMat(const ofbx::Object* obj, ofbx::Matrix Geo) {

			ofbx::Matrix ModelMatrix = obj->getGlobalTransform();
			ModelMatrix = ModelMatrix * Geo;

			XMFLOAT4X4 tmpGtmp = ofbxMatToXM(&ModelMatrix);
			return XMMatrixInverse(nullptr,XMLoadFloat4x4(&tmpGtmp));

		}


		void BindPoseBones() {

			for (int i = 0; i < Bones.size(); i++) {
				
				XMStoreFloat4x4(&BoneDataTLMA[i], XMMatrixTranspose(Bones[i].SkinM));
					//BoneDataTLMA[i] = rootObjM;
			}
		}

		void GetAnimEndTime(int Anim) {
			float max = 0; // max time getter func - iterate through keys

			for (int i = 0; i < Bones.size(); i++) {

				for (int x = 0; x < 3; x++) {
					ofbx::i64 tcnV = 0.0f;
					ofbx::i64 rcnV = 0.0f;
					ofbx::i64 scnV = 0.0f;

					if (Bones[i].tNode[Anim]->getCurve(x)) {
						const ofbx::AnimationCurve* tc = Bones[i].tNode[Anim]->getCurve(x);
						int tcn = tc->getKeyCount() - 1;
						tcnV = tc->getKeyTime()[tcn];
					}

					if (Bones[i].rNode[Anim]->getCurve(x)) {
						const ofbx::AnimationCurve* rc = Bones[i].rNode[Anim]->getCurve(x);
						int rcn = rc->getKeyCount() - 1;
						rcnV = rc->getKeyTime()[rcn];
					}

					if (Bones[i].sNode[Anim]->getCurve(x)) {
						const ofbx::AnimationCurve* sc = Bones[i].sNode[Anim]->getCurve(x);
						int scn = sc->getKeyCount() - 1;
						scnV = sc->getKeyTime()[scn];
					}


					ofbx::i64 maxTmp = std::max({ tcnV,rcnV,scnV });

					if (max < maxTmp) max = static_cast<float>(ofbx::fbxTimeToSeconds(maxTmp));
				}

			}
			animStackMaxTime[Anim] = max;
		}

		void GetAllAnimEndTime() {
			for (int i = 0; i < animStack.size(); i++) {
				GetAnimEndTime(i);
			}
		}

		void ResizeAndPrepAnimBoneStuff() {
			animStackMaxTime.resize(animStack.size());
			AnimVCacheMade.resize(animStack.size());
			AnimVCacheData.resize(animStack.size());
//			for (int i = 0; i < AnimVCacheData.size(); i++) {
//				AnimVCacheData[i].resize(Bones.size());
//			}
		}

		void LoadFBXFileWithVertex(std::string path) {
			LoadFBXFile(path);

			SetAllBonesChildren();
			SetAllBonesParent();
			SetAllBonesGetInvBindT();

			ResizeAllBonesNodeArr();
			GetAllBoneNodes();

			ResizeAndPrepAnimBoneStuff();

			GetAllAnimEndTime();

			BindPoseBones();

			CreateArmatureCBuf();
			FillTopBones(); //TODO: broken, fix!!!
			LoadVertexIndiceData();
		}
		//TODO: , make translate in pixels for x, y - and z is x depth for pixels

		void PassCBufToGPU() {
			dxDeviceContext->UpdateSubresource(CBuf, 0, nullptr, &ObjTune, 0, 0);
		}

		void MSRObject(std::array<float, 3> XYZTranslate = { 0.0f,0.0f,0.0f }, std::array<float, 3> scale = { 1.0f,1.0f,1.0f }, std::array<float, 3> rotateXYZaxis = { 0.0f,0.0f,0.0f }) { // pass in OG values to not change the ones not wanted to modify
			
			olc::vf2d vInvScreenSize = {
		(1.0f / float(PL.ScreenWidth())),
		(1.0f / float(PL.ScreenHeight()))
			};

			ObjTune.Translate = std::array<float, 3>{ToNotPGESpace(&vInvScreenSize.x, &XYZTranslate[0]), ToNotPGESpace(&vInvScreenSize.y, &XYZTranslate[1]), ToNotPGESpace(&vInvScreenSize.x, &XYZTranslate[2]) };

			ObjTune.Scale = scale;

			XMStoreFloat4(&ObjTune.Quat, XMQuaternionRotationRollPitchYaw(rotateXYZaxis[0], rotateXYZaxis[1], rotateXYZaxis[2]) );

			PassCBufToGPU();
		}

		void MSRDefBone(std::array<float, 3> XYZTranslate = { 0.0f,0.0f,0.0f }, std::array<float, 3> scale = { 1.0f,1.0f,1.0f }, std::array<float, 3> rotateXYZaxis = { 0.0f,0.0f,0.0f }) { 

		}

		M3DR(olc::Decal* Tex, std::string path = "", bool LinearTOrPoint = true, bool ClampTOrMirror = true) {
			

			//SetupTexLinkResource();
			SetTexCopy(Tex, LinearTOrPoint, ClampTOrMirror);
			SetupBlendStateDefault();
			if (path != "") {
				LoadFBXFileWithVertex(path);
			}
			else {
				std::cout << "no file at path";
			}
			DefaultCBuf();
		}
		M3DR(olc::Sprite* Tex, std::string path = "", bool LinearTOrPoint = true, bool ClampTOrMirror = true) {
			
			//SetupTexLinkResource();
			SetTexCopy(Tex, LinearTOrPoint, ClampTOrMirror);
			SetupBlendStateDefault();
			if (path != "") {
				LoadFBXFileWithVertex(path);
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
				//{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BLENDID", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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
				//	"float4 color: COLOR;\n"
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
//				"#pragma pack_matrix(row_major)\n"
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

				"cbuffer Armature : register(b7){\n"
				"matrix armature[72];}\n"

				"struct AppData{\n"
				"float3 position : POSITION;\n"
				"float3 normal : NORMAL;\n"
				//		"float4 color: COLOR;\n"
				"float2 tex : TEXCOORD;\n"
				"int4 bID : BLENDID;\n"
				"float4 bW : BLENDWEIGHT;\n"
				"};\n"

				"struct VertexShaderOutput{\n"
				"float4 position : SV_POSITION;\n"
				"float3 normal: NORMAL;\n"
				//	"float4 color: COLOR;\n"
				"float2 tex : TEXCOORD0;\n"
				"float4 PositionWS : TEXCOORD1;};\n"

				"float3 QuatRotate(float3 pos, float4 quat){\n"
				"return pos + 2.0 * cross(quat.xyz, cross(quat.xyz, pos) + quat.w * pos);\n"
				"}\n"
				

				"VertexShaderOutput SimpleVS(AppData IN){\n"
				"VertexShaderOutput OUT;\n"
			
				"float4 bP = float4(0,0,0,0);\n"

				"float4 nF = float4(0,0,0,0);\n" //normals

				"if(IN.bW[0]+IN.bW[1]+IN.bW[2]+IN.bW[3] == 0) { bP = float4(IN.position,1); nF = float4(IN.normal,1);}\n"
				"else{\n"
				"matrix tmpM = armature[IN.bID[0]]*IN.bW[0]+armature[IN.bID[1]]*IN.bW[1]+armature[IN.bID[2]]*IN.bW[2]+armature[IN.bID[3]]*IN.bW[3];\n"
				"bP = mul(tmpM,float4(IN.position,1));\n"
				"nF = mul(tmpM,float4(IN.normal,1));\n"
				"}"

				"float3 adjust = ((QuatRotate(bP.xyz, Quat)*Scale)+Translate);"

				"bP = float4(adjust,1);\n"

				"adjust = ((QuatRotate(nF.xyz, Quat)*Scale)+Translate);"

				"nF = float4(adjust,1);\n"


				"matrix mvp = mul(projectionMatrix, mul(viewMatrix, worldMatrix));\n"
				"OUT.position = mul(mvp, bP );\n"
				"OUT.normal = mul(worldMatrix, nF).xyz;\n" 
				"OUT.PositionWS = mul(worldMatrix, bP);\n"
				"OUT.tex = IN.tex;\n"
			//	"OUT.color = IN.color;\n"
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
			//	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BLENDID", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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
				//"float4 color: COLOR;\n"
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

				"cbuffer Armature : register(b7){\n"
				"matrix armature[72];}\n"

				"struct AppData{\n"
				"float3 position : POSITION;\n"
				"float3 normal : NORMAL;\n"
				//	"float4 color: COLOR;\n"
				"float2 tex : TEXCOORD;\n"
				"int4 bID : BLENDID;\n"
				"float4 bW : BLENDWEIGHT;\n"
				"};\n"

				"struct VertexShaderOutput{\n"
				"float4 position : SV_POSITION;\n"
				"float3 normal: NORMAL;\n"
				//	"float4 color: COLOR;\n"
				"float2 tex : TEXCOORD0;\n"
				"float4 PositionWS : TEXCOORD1;};\n"

				"float3 QuatRotate(float3 pos, float4 quat){\n"
				"return pos + 2.0 * cross(quat.xyz, cross(quat.xyz, pos) + quat.w * pos);\n"
				"}\n"

				"VertexShaderOutput SimpleVS(AppData IN){\n"
				"VertexShaderOutput OUT;\n"
			
				"float4 bP = float4(0,0,0,0);\n"

				"float4 nF = float4(0,0,0,0);\n" //normals

				"if(IN.bW[0]+IN.bW[1]+IN.bW[2]+IN.bW[3] == 0) { bP = float4(IN.position,1); nF = float4(IN.normal,1);}\n"
				"else{\n"
				"matrix tmpM = armature[IN.bID[0]]*IN.bW[0]+armature[IN.bID[1]]*IN.bW[1]+armature[IN.bID[2]]*IN.bW[2]+armature[IN.bID[3]]*IN.bW[3];\n"
				"bP = mul(tmpM,float4(IN.position,1));\n"
				"nF = mul(tmpM,float4(IN.normal,1));\n"
				"}"

				"float3 adjust = ((QuatRotate(bP.xyz, Quat)*Scale)+Translate);"

				"bP = float4(adjust,1);\n"

				"adjust = ((QuatRotate(nF.xyz, Quat)*Scale)+Translate);"

				"nF = float4(adjust,1);\n"


				"matrix mvp = mul(projectionMatrix, mul(viewMatrix, worldMatrix));\n"
				"OUT.position = float4(bP[0]+viewMatrix[0][3],bP[1]+viewMatrix[1][3],bP[2]+viewMatrix[2][3],1);\n" //+viewMatrix[3][0] 
				"OUT.normal = mul(worldMatrix, nF).xyz;\n"
				"OUT.PositionWS = mul(worldMatrix, bP);\n"
				"OUT.tex = IN.tex;\n"
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

		std::vector<ID3D11Buffer*> CBufTmp;

		void DrawM(M3DR* Model, bool usingTmps = false, std::array<float,3> XYZtmpTranslate = {0.0f,0.0f,0.0f}, std::array<float, 3> tmpScale = { 1.0f,1.0f,1.0f }, std::array<float, 3> TmpRotateXYZaxis = { 0.0f,0.0f,0.0f }) {
			
			Model->CheckToUpdateArmatureCBuf();

			ID3D11Buffer* CBufTmpOne;

			ObjTuneStatReg ObjTuneTmp;

			if (usingTmps == true) {

				CBufTmp.push_back(CBufTmpOne);
				
				ObjTuneTmp.Scale = tmpScale;
				ObjTuneTmp.Translate = XYZtmpTranslate;
				XMStoreFloat4(&ObjTuneTmp.Quat, XMQuaternionRotationRollPitchYaw(TmpRotateXYZaxis[0], TmpRotateXYZaxis[1], TmpRotateXYZaxis[2]));

				D3D11_BUFFER_DESC bufDesc;
				ZeroMemory(&bufDesc, sizeof(bufDesc));
				bufDesc.Usage = D3D11_USAGE_DEFAULT;
				bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				bufDesc.CPUAccessFlags = 0;
				bufDesc.ByteWidth = sizeof(ObjTuneStatReg);
				dxDevice->CreateBuffer(&bufDesc, nullptr, &CBufTmp[CBufTmp.size()-1]);

				dxDeviceContext->UpdateSubresource(CBufTmp[CBufTmp.size() - 1], 0, nullptr, &ObjTuneTmp, 0, 0);

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
				dxDeviceContext->VSSetConstantBuffers(6, 1, &CBufTmp[CBufTmp.size() - 1]);
			}

			dxDeviceContext->VSSetConstantBuffers( //in case no decals were drawn I need to fill const buf with the proper matrix's
				7,
				1,
				&Model->ArmatureCBuf
			);
			
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
			
			Model->CheckToUpdateArmatureCBuf();

			ID3D11Buffer* CBufTmpOne;

			ObjTuneStatReg ObjTuneTmp;

			if (usingTmps == true) {

				CBufTmp.push_back(CBufTmpOne);

				ObjTuneTmp.Scale = tmpScale;
				ObjTuneTmp.Translate = XYZtmpTranslate;
				XMStoreFloat4(&ObjTuneTmp.Quat, XMQuaternionRotationRollPitchYaw(TmpRotateXYZaxis[0], TmpRotateXYZaxis[1], TmpRotateXYZaxis[2]));

				D3D11_BUFFER_DESC bufDesc;
				ZeroMemory(&bufDesc, sizeof(bufDesc));
				bufDesc.Usage = D3D11_USAGE_DEFAULT;
				bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				bufDesc.CPUAccessFlags = 0;
				bufDesc.ByteWidth = sizeof(ObjTuneStatReg);
				dxDevice->CreateBuffer(&bufDesc, nullptr, &CBufTmp[CBufTmp.size()-1]);

				dxDeviceContext->UpdateSubresource(CBufTmp[CBufTmp.size() - 1], 0, nullptr, &ObjTuneTmp, 0, 0);

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
				dxDeviceContext->VSSetConstantBuffers(6, 1, &CBufTmp[CBufTmp.size() - 1]);
			}

			dxDeviceContext->VSSetConstantBuffers( //in case no decals were drawn I need to fill const buf with the proper matrix's
				7,
				1,
				&Model->ArmatureCBuf
			);

			dxDeviceContext->IASetInputLayout(
				ShaderData.BMSIl);

			dxDeviceContext->IASetPrimitiveTopology(
				D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ


			dxDeviceContext->VSSetShader(
				ShaderData.BMS2dVs,
				nullptr,
				0);


			dxDeviceContext->RSSetState(dxRasterizerStateF);

			float bState[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			dxDeviceContext->OMSetBlendState(Model->BlendState, bState, 0xffffffff);

			dxDeviceContext->OMSetDepthStencilState(dxDepthStencilStateDefault, 1);

			dxDeviceContext->PSSetShader(
				ShaderData.BMS2dPs,
				nullptr,
				0);


			dxDeviceContext->PSSetShaderResources(0, 1, &Model->Tex1SRV);

			dxDeviceContext->PSSetSamplers(0, 1, &Model->Sampler);

			dxDeviceContext->DrawIndexed(
				Model->Indice.size(),
				0,
				0);

		}

		void LerpModelPosLogic(DataLerpFunc* tmp, M3DR* mod) {

			float LaS = PL.LastSec();
			float TimeLeft = tmp->MaxTime - tmp->CurTime;
			float ratio = TimeLeft / LaS;

			//std::cout <<"ratio:"<< ratio << "\n";
			//std::cout << "time left:" << TimeLeft << "\n";


			if (TimeLeft > 0) { //should always pass through - for now for debug I keep - but I can trash this later - TODO:

				//camXRot

				if (tmp->useX) {
					float DXLeftRatio = (tmp->X - mod->ObjTune.Translate[0]) / ratio;

					camXRot += mod->ObjTune.Translate[0];
				}
				if (tmp->useY) {
					float DYLeftRatio = (tmp->Y - mod->ObjTune.Translate[1]) / ratio;

					camYRot += mod->ObjTune.Translate[1];
				}
				if (tmp->useZ) {
					float DZLeftRatio = (tmp->Z - mod->ObjTune.Translate[2]) / ratio;

					camZRot += mod->ObjTune.Translate[2];

				}

				//UpdateCamForce = true;

				tmp->CurTime += LaS;

				mod->PassCBufToGPU();
			}
		}
		void LerpModelRotLogic(DataLerpFunc* tmp, M3DR* mod) {

			float LaS = PL.LastSec();
			float TimeLeft = tmp->MaxTime - tmp->CurTime;
			float ratio = TimeLeft / LaS;

			//std::cout <<"ratio:"<< ratio << "\n";
			//std::cout << "time left:" << TimeLeft << "\n";


			if (TimeLeft > 0) { //should always pass through - for now for debug I keep - but I can trash this later - TODO:

				//camXRot
				float quatDiv = sqrt(1 - mod->ObjTune.Quat.w * mod->ObjTune.Quat.w);

				float xRad;
				float yRad;
				float zRad;

				if (tmp->useX) {
					xRad = (mod->ObjTune.Quat.x / quatDiv);

					xRad += (tmp->X - xRad) / ratio;
				}
				if (tmp->useY) {
					yRad = (mod->ObjTune.Quat.y / quatDiv);

					yRad += (tmp->Y - yRad) / ratio;
				}
				if (tmp->useZ) {
					zRad = (mod->ObjTune.Quat.z / quatDiv);

					zRad += (tmp->Z - zRad) / ratio;
				}

				//UpdateCamForce = true;
				XMStoreFloat4(&mod->ObjTune.Quat, XMQuaternionRotationRollPitchYaw(xRad, yRad, yRad));

				tmp->CurTime += LaS;

				mod->PassCBufToGPU();
			}
		}

	}MDFs;

	void DrawM(M3DR* Model, bool before = false, bool usingTmps = false, std::array<float, 3> XYZtmpTranslate = { 0.0f,0.0f,0.0f }, std::array<float, 3> tmpScale = { 1.0f,1.0f,1.0f }, std::array<float, 3> rotateXYZaxis = { 0.0f,0.0f,0.0f }) {
		DataDrawOrderAndFunc tmp;

		olc::vf2d inv = {
(1.0f / float(PL.ScreenWidth())),
(1.0f / float(PL.ScreenHeight()))
		};

		std::array<float, 3> tmpd = { ToNotPGESpace(&inv.x, &XYZtmpTranslate[0]), ToNotPGESpace(&inv.y, &XYZtmpTranslate[1]), ToNotPGESpace(&inv.x, &XYZtmpTranslate[2]) };

		tmp.func = [=]() {MDFs.DrawM(Model, usingTmps, tmpd, tmpScale, rotateXYZaxis); };

		if (before == false) {
			DrawOrder.push_back(tmp);
		}
		else {
			DrawOrderBefore.push_back(tmp);
		}

	}

	void DrawM2D(M3DR* Model, bool before = false, bool usingTmps = false, std::array<float, 3> XYZtmpTranslate = { 0.0f,0.0f,0.0f }, std::array<float, 3> tmpScale = { 1.0f,1.0f,1.0f }, std::array<float, 3> rotateXYZaxis = { 0.0f,0.0f,0.0f }) {
		DataDrawOrderAndFunc tmp;

		olc::vf2d inv = {
(1.0f / float(PL.ScreenWidth())),
(1.0f / float(PL.ScreenHeight()))
		};

		std::array<float, 3> tmpd = { ToNotPGESpace(&inv.x, &XYZtmpTranslate[0]), ToNotPGESpace(&inv.y, &XYZtmpTranslate[1]), ToNotPGESpace(&inv.x, &XYZtmpTranslate[2]) };

		tmp.func = [=]() {MDFs.DrawM2D(Model, usingTmps, tmpd, tmpScale, rotateXYZaxis); };

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
		void LerpCamRotLogic(DataLerpFunc* tmp) {
			//use fLastElapsed to get steps

			float LaS = PL.LastSec();
			float TimeLeft = tmp->MaxTime - tmp->CurTime;
			float ratio = TimeLeft / LaS;

			//std::cout <<"ratio:"<< ratio << "\n";
			//std::cout << "time left:" << TimeLeft << "\n";


			if (TimeLeft > 0) { //should always pass through - for now for debug I keep - but I can trash this later - TODO:

				//camXRot

				if (tmp->useX) {
					float DXLeftRatio = (tmp->X - camXRot) / ratio;

					camXRot += DXLeftRatio;
				}
				if (tmp->useY) {
					float DYLeftRatio = (tmp->Y - camYRot) / ratio;

					camYRot += DYLeftRatio;
				}
				if (tmp->useZ) {
					float DZLeftRatio = (tmp->Z - camZRot) / ratio;

					camZRot += DZLeftRatio;

				}

				//UpdateCamForce = true;

				tmp->CurTime += LaS;
			}
		}
	}CFL;

	void LerpCamPos(bool useX = false, bool useY = false, bool useZ = false, float X2 = 0, float Y2 = 0, float Z2 = 0, float MaxTime = 3.0f) {
		DataLerpFunc tmp;
		LerpCamFunc.push_back(tmp);

		olc::vf2d vInvScreenSize = {
	(1.0f / float(PL.ScreenWidth())),
	(1.0f / float(PL.ScreenHeight()))
		};

		LerpCamFunc[LerpCamFunc.size()-1].X = ToNotPGESpace(&vInvScreenSize.x, &X2);
		LerpCamFunc[LerpCamFunc.size() - 1].useX = useX;
		
		LerpCamFunc[LerpCamFunc.size() - 1].Y = ToNotPGESpace(&vInvScreenSize.y, &Y2);
		LerpCamFunc[LerpCamFunc.size() - 1].useY = useY;

		LerpCamFunc[LerpCamFunc.size() - 1].Z = ToNotPGESpace(&vInvScreenSize.x, &Z2);
		LerpCamFunc[LerpCamFunc.size() - 1].useZ = useZ;

		LerpCamFunc[LerpCamFunc.size() - 1].CurTime = 0.0f;
		LerpCamFunc[LerpCamFunc.size() - 1].MaxTime = MaxTime;
		
		LerpCamFunc[LerpCamFunc.size() - 1].func = [&]() {CFL.LerpCamPosLogic(LerpCamFunc[LerpCamFunc.size() - 1].me() ); };

	}
	void LerpCamRot(bool useX = false, bool useY = false, bool useZ = false, float X2 = 0, float Y2 = 0, float Z2 = 0, float MaxTime = 3.0f) {
		
		DataLerpFunc tmp;
		LerpCamFunc.push_back(tmp);

		LerpCamFunc[LerpCamFunc.size() - 1].X = X2;
		LerpCamFunc[LerpCamFunc.size() - 1].useX = useX;

		LerpCamFunc[LerpCamFunc.size() - 1].Y = Y2;
		LerpCamFunc[LerpCamFunc.size() - 1].useY = useY;

		LerpCamFunc[LerpCamFunc.size() - 1].Z = Z2;
		LerpCamFunc[LerpCamFunc.size() - 1].useZ = useZ;

		LerpCamFunc[LerpCamFunc.size() - 1].CurTime = 0.0f;
		LerpCamFunc[LerpCamFunc.size() - 1].MaxTime = MaxTime;

		LerpCamFunc[LerpCamFunc.size() - 1].func = [&]() {CFL.LerpCamRotLogic(LerpCamFunc[LerpCamFunc.size() - 1].me()); };

	}

	void LerpModelPos(M3DR* mod, bool useX = false, bool useY = false, bool useZ = false, float X2 = 0, float Y2 = 0, float Z2 = 0, float MaxTime = 3.0f) {
		DataLerpFunc tmp;
		LerpModelFunc.push_back(tmp);

		olc::vf2d vInvScreenSize = {
	(1.0f / float(PL.ScreenWidth())),
	(1.0f / float(PL.ScreenHeight()))
		};

		LerpModelFunc[LerpModelFunc.size() - 1].X = ToNotPGESpace(&vInvScreenSize.x, &X2);
		LerpModelFunc[LerpModelFunc.size() - 1].useX = useX;

		LerpModelFunc[LerpModelFunc.size() - 1].Y = ToNotPGESpace(&vInvScreenSize.y, &Y2);
		LerpModelFunc[LerpModelFunc.size() - 1].useY = useY;

		LerpModelFunc[LerpModelFunc.size() - 1].Z = ToNotPGESpace(&vInvScreenSize.x, &Z2);
		LerpModelFunc[LerpModelFunc.size() - 1].useZ = useZ;

		LerpModelFunc[LerpModelFunc.size() - 1].CurTime = 0.0f;
		LerpModelFunc[LerpModelFunc.size() - 1].MaxTime = MaxTime;

		LerpModelFunc[LerpModelFunc.size() - 1].func = [&]() {MDFs.LerpModelPosLogic(LerpModelFunc[LerpCamFunc.size() - 1].me(), mod); };

	}
	void LerpModelRot(M3DR* mod, bool useX = false, bool useY = false, bool useZ = false, float X2 = 0, float Y2 = 0, float Z2 = 0, float MaxTime = 3.0f) {

		DataLerpFunc tmp;
		LerpModelFunc.push_back(tmp);

		LerpModelFunc[LerpModelFunc.size() - 1].X = X2;
		LerpModelFunc[LerpModelFunc.size() - 1].useX = useX;

		LerpModelFunc[LerpModelFunc.size() - 1].Y = Y2;
		LerpModelFunc[LerpModelFunc.size() - 1].useY = useY;

		LerpModelFunc[LerpModelFunc.size() - 1].Z = Z2;
		LerpModelFunc[LerpModelFunc.size() - 1].useZ = useZ;

		LerpModelFunc[LerpModelFunc.size() - 1].CurTime = 0.0f;
		LerpModelFunc[LerpModelFunc.size() - 1].MaxTime = MaxTime;

		LerpModelFunc[LerpModelFunc.size() - 1].func = [&]() {MDFs.LerpModelRotLogic(LerpModelFunc[LerpModelFunc.size() - 1].me(), mod); };

	}

	//void ChainedLerpCamPoves() {
		//TODO: when done lerp it starts next in chain like linked list - which can start another, ect- and can loop the chain as option when done until stopped
	//}
	//void ChainedLerpCamRotation() {
		//TODO: when done lerp it starts next in chain like linked list - which can start another, ect- and can loop the chain as option when done until stopped
	//}

	/*
	
	END OF FUN CAM STUFF
	
	*/
	void ProgramLink_3D_DX::DrawFuncMain() {

		for (int i = 0; i < LerpCamFunc.size(); i++) {
			if (LerpCamFunc[i].CurTime < LerpCamFunc[i].MaxTime) {
				LerpCamFunc[i].func();
			}
			else {
				LerpCamFunc.erase(LerpCamFunc.begin() + i);
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
			SafeRelease(MDFs.CBufTmp[i]);			
		}
		MDFs.CBufTmp.clear();
		//clear tmp buf : end
	}



}


#endif
