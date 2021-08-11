//TODO: annotate the test code and make it look good
//Example images are derivative work I had created by my own hand - it is being used for a project which is where my private commits were a while back - still under works... the other dev is just delayed to do stuff.
#define OLC_PGE_APPLICATION

#define OLC_PLATFORM_WINAPI
#define OLC_GFX_DIRECTX11 
#define OLC_GFX_DIRECTX11_FLIP_DISCARD
#define OLC_PGEX_DIRECTX11_SHADERS_PLUS
//#define OLC_GFX_DIRECTX11_FLIP_SEQUENTIAL


#include "olcPixelGameEngine.h" 
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
	int ParticleSystemHandleReturn;

	bool OnUserCreate() override  
	{

		

		Clear(olc::DARK_BLUE);
		st = std::make_unique<olc::Sprite>("./1.png");
		dt = std::make_unique<olc::Decal>(st.get());


		st2 = std::make_unique<olc::Sprite>("./2.png");
		dt2 = std::make_unique<olc::Decal>(st2.get());
		
		InitializeParticlesWorker(this);
		ParticleSystemHandleReturn = DX11CreateRandomRangeParticleSystem(30, { olc::vf2d( 100.0f,0.0f ),olc::vf2d(100.0f,100.0f) }, st.get(), { 2.0f,2.0f }, olc::WHITE);
		//set decal mode individually
		// 
		//AdjustTestParticleClass(ParticleSystemHandleReturn, { 200,0 }, { 3.0f,3.0f }, olc::RED);
		AdjustRandomRangeParticleClass(0, 100, { olc::vf2d(100.0f,0.0f),olc::vf2d(100.0f,100.0f) }, st.get(), { 2.0f,2.0f }, olc::WHITE);
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		InitializeShadersAndBase();

		Clear(olc::BLUE);
		//MoveParticleLayer(1);
		//SetDrawTarget(1);
		DrawStringDecal({ 400,0 }, "asdasdasdasd", olc::RED);

		DrawRandomRangeParticleSystem(ParticleSystemHandleReturn);
		
		RegenRRforRandomRange(ParticleSystemHandleReturn); //SLOW TO REGENERATE CONSTANTLY (kinda at least) - REMEMBER to use everything in moderation

		//SetDrawTarget(nullptr);

		DrawStringDecal({ 0,0 }, "asdasdasdasd", olc::RED);
		FillRectDecal({ 200, 0 }, { 50, 100 }, olc::RED);

		DrawStringDecal({ 0,300 }, "asdasdasdasd", olc::RED, { 1,1 });

		
		DrawSprite({ 500,0 }, st.get());
		SetDecalMode(olc::DecalMode::NORMAL); //diffrent
		DrawDecal(olc::vi2d(300, 300), dt.get(), { 4.0f, 4.0f });

		DrawSprite(olc::vi2d{ 300,300 }, st2.get());

		

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
