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
	
	int HandleToBasicPointLight;
	bool OnUserCreate() override  
	{

		

		Clear(olc::DARK_BLUE);
		st = std::make_unique<olc::Sprite>("./1.png");
		dt = std::make_unique<olc::Decal>(st.get());


		st2 = std::make_unique<olc::Sprite>("./2.png");
		dt2 = std::make_unique<olc::Decal>(st2.get());
		
		InitializeParticlesWorker(this);
		EnableLight();
		

		//TODO: direction light (cone) with blocking of pixel 
		
		HandleToBasicPointLight = DX11CreateBasicPointLight(1.0f, { 50,100}, 1.0f, olc::BLACK, { 300,300 });
			
		UpdateBasicPointLightData(HandleToBasicPointLight, 1.0f, { 90,100 }, 1.0f, olc::BLACK, { 300,300 });

		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override
	{
		
		
		Clear(olc::BLUE);
		//MoveParticleLayer(1);
		//SetDrawTarget(1);
		
		DrawBasicPointLight(HandleToBasicPointLight);
		//RegenRRforRandomRange(ParticleSystemHandleReturn); //SLOW TO REGENERATE CONSTANTLY (kinda at least) - REMEMBER to use everything in moderation

		//SetDrawTarget(nullptr);
		
		
		//////////////////////////////////////////////////////////set a break point in VS to see that values added

		DrawStringDecal({ 0,0 }, "Tests Overlap", olc::RED);
		
		
		
		//DrawSprite({ 500,0 }, st.get());
		SetDecalMode(olc::DecalMode::NORMAL); //diffrent
		//for (int i = 0; i < 100000; i++) {

		DrawDecal(olc::vi2d(200, 300), dt.get(), { 4.0f, 4.0f });

		//}
		DrawSprite(olc::vi2d{ 0,570}, st2.get());

		

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
