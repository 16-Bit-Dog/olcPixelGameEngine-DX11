//TODO: annotate the test code and make it look good
//Example images are derivative work I had created by my own hand - it is being used for a project which is where my private commits were a while back - still under works... the other dev is just delayed to do stuff.
#define OLC_PGE_APPLICATION
//smaller is more efficent particles
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
	int tMod;

	bool OnUserCreate() override
	{



		Clear(olc::DARK_BLUE);
		st = std::make_unique<olc::Sprite>("./1.png");
		dt = std::make_unique<olc::Decal>(st.get());


		st2 = std::make_unique<olc::Sprite>("./2.png");
		dt2 = std::make_unique<olc::Decal>(st2.get());

		InitializeParticlesWorker(this);

		float  opacityStrengthRange[2] = { 180.0f,255.0f }; //min and then max
		float  opacityChangeRange[2] = { 0.05f, 0.1f };

		//I used 256 to say I have no max alpha I want to cause a particle regeneration
		ParticleSystemHandleReturn = DX11CreateRandomLifeTimeParticleSystem(100, true, 0, 256, true, olc::vf2d(0.0f, 0.0f), olc::vf2d(float(ScreenWidth()), float(ScreenHeight())), opacityStrengthRange, opacityChangeRange, { olc::vf2d(200.0f, 200.0f),  olc::vf2d(200.0f, 200.0f) }, { olc::vf2d(-0.0005f, -0.0005f),  olc::vf2d(0.0005f, 0.0005f) }, { olc::vf2d(-0.00005f, -0.00005f),  olc::vf2d(0.00005f, 0.00005f) }, st.get(), { 2.0f,2.0f }, olc::WHITE);

		AdjustRandomLifeTimeParticleSystem(ParticleSystemHandleReturn, 100, true, 0, 256, true, olc::vf2d(0.0f, 0.0f), olc::vf2d(float(ScreenWidth()), float(ScreenHeight())), opacityStrengthRange, opacityChangeRange, { olc::vf2d(200.0f, 200.0f),  olc::vf2d(200.0f, 200.0f) }, { olc::vf2d(-0.0005f, -0.0005f),  olc::vf2d(0.0005f, 0.0005f) }, { olc::vf2d(-0.00005f, -0.00005f),  olc::vf2d(0.00005f, 0.00005f) }, st.get(), { 2.0f,2.0f }, olc::RED);

		tMod = CreateTextureModifierShader("r+1", "g+1", "b+1", "a"); //will make while because I add 1.0f to a 0-1 range of color

		RunTextureModifierShader(tMod, dt.get());
		RunTextureModifierShader(tMod, GetTextureRandomLifeTimeParticleSystem(ParticleSystemHandleReturn));

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		InitializeShadersAndBase();

		Clear(olc::BLUE);
		//MoveParticleLayer(1);
		//SetDrawTarget(1);
		DrawStringDecal({ 400,0 }, "asdasdasdasd", olc::RED);

		DrawRandomLifeTimeParticleSystem(ParticleSystemHandleReturn);


		//RegenRRforRandomRange(ParticleSystemHandleReturn); //SLOW TO REGENERATE CONSTANTLY (kinda at least) - REMEMBER to use everything in moderation

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
