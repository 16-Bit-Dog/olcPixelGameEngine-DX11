//TODO: annotate the test code and make it look good
//Example images are derivative work I had created by my own hand - it is being used for a project which is where my private commits were a while back - still under works... the other dev is just delayed to do stuff.
#define OLC_PGE_APPLICATION
//smaller is more efficent particles
#define OLC_PLATFORM_WINAPI
#define OLC_GFX_DIRECTX11 
#define OLC_GFX_DIRECTX11_FLIP_DISCARD
#define OLC_PGEX_DIRECTX11_SHADERS_PLUS
//#define OLC_GFX_DIRECTX11_FLIP_SEQUENTIAL


#include "olcPixelGameEngine.h" //this is 
#include "olcPGEX_DX11ShadersPlus.h"

using namespace SPDX11;

class FET : public olc::PixelGameEngine
{
public:


	FET()
	{
		sAppName = "TUTORIAL - DX Play Ground";

	}

public:

	std::unique_ptr<olc::Sprite> st2;
	std::unique_ptr<olc::Sprite> st;
	std::unique_ptr<olc::Decal> dt;
	std::unique_ptr<olc::Decal> dt2;

	int ComputeFloatBasicHandle;
	bool OnUserCreate() override
	{



		Clear(olc::DARK_BLUE);
		st = std::make_unique<olc::Sprite>("./1.png");
		dt = std::make_unique<olc::Decal>(st.get());


		st2 = std::make_unique<olc::Sprite>("./2.png");
		dt2 = std::make_unique<olc::Decal>(st2.get());

		InitializeParticlesWorker(this);


		std::vector<float> vec1(1000000); //remember that a gpu has to transfer data over pcie before it has math done... be aware of this as a limitation for small vector additions
		std::vector<float> vec2(1000000); //max is 67107840 - if someone acctually needs a higher possable number - feel free to send a github issue - or contact me

		for (int i = 0; i < 1000000 - 1; i++) { //why don't I use std::transformation you ask? - no idea  :P
			vec1[i] = i;
			vec2[i] = i;
		}

		std::vector<float> testVec1 = { 1 };

		std::vector<float> testVec2 = { 1 };

		ComputeFloatBasicHandle = DX11CreateVecBasicComputeFloat2(1, testVec1, testVec2);  //CANNOT BE 0

		AdjustVecBasicFloat2(ComputeFloatBasicHandle, vec1.size(), vec1, vec2);
		
		NewMathForVecBasicFloat2(ComputeFloatBasicHandle, "x+y"); //add x and y shader math

		return true;
	}
	std::vector<float> output;
	bool OnUserUpdate(float fElapsedTime) override
	{
		InitializeShadersAndBase();
		
		output = DispatchVecBasicFloat2(ComputeFloatBasicHandle);


		Clear(olc::BLUE);

		DrawStringDecal({ 0,0 }, "Tests Overlap", olc::RED);



		SetDecalMode(olc::DecalMode::NORMAL); //diffrent

		DrawDecal(olc::vi2d(0, 500), dt.get(), { 4.0f, 4.0f });

		//}
		DrawSprite(olc::vi2d{ 0,570 }, st2.get());



		return true;
	}
}feh;

int main()
{

	if (feh.Construct(640, 640, 1, 1, false, false, false)) {

		feh.Start();
		return true;

	}

}
