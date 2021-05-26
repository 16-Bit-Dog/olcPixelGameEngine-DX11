//TODO: annotate the test code and make it look good
//Example images are derivative work I had created by my own hand - it is being used for a project which is where my private commits were a while back - still under works... the other dev is just delayed to do stuff.
#define OLC_PGE_APPLICATION
//smaller is more efficent particles
#define OLC_PLATFORM_WINAPI
#define OLC_GFX_DIRECTX11 
#define OLC_GFX_DIRECTX11_FLIP_DISCARD
#define OLC_PGEX_DIRECTX11_SHADERS_PLUS
//#define OLC_GFX_DIRECTX11_FLIP_SEQUENTIAL
//unless you use constant fps with vsync enabledthe animated parts will be weird

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

	int HandleToBasicPointLight2;

	int HandleToBasicPointLight3;

	int HandleToBasicPointLight4;

	int HandleToBasicPointLight5;

	int HandleToBasicPointLight6;

	int HandleToBasicPointLight7;


	bool OnUserCreate() override  
	{

		

		Clear(olc::DARK_BLUE);
		st = std::make_unique<olc::Sprite>("./1.png");
		dt = std::make_unique<olc::Decal>(st.get());


		st2 = std::make_unique<olc::Sprite>("./2.png");
		dt2 = std::make_unique<olc::Decal>(st2.get());
		
		InitializeParticlesWorker(this);
		EnableLight();
		
		std::vector<float[2]> blockPix = {};

		//TODO: direction light (cone) with blocking of pixel 
		
		HandleToBasicPointLight = DX11CreateBasicPointLight(1.0f, { 50,100}, 1.0f, olc::Pixel(0,0,0,125), { 300,300 }, 0.0f); // I change this light below
			
		///to fade easily into the background light - making it non inverse with olc::black color works perfect*
		UpdateBasicPointLightData(HandleToBasicPointLight, 0.9f, { 50,40 }, 10.0f, olc::Pixel(255, 0, 0, 255), { 400,300 }, 1.0f); //add shine to middle of light source by making final item 1 - this inverses how I get lighting

		HandleToBasicPointLight7 = DX11CreateBasicPointLight(1.0f, { 80,70 }, 1.0f, olc::Pixel(0, 0, 0, 125), { 300,300 }, 0.0f); // I change this light below

		HandleToBasicPointLight2 = DX11CreateBasicPointLight(1.0f, { 80,80 }, 2.0f, olc::BLACK, { 200,400 }, 0.0f);



		HandleToBasicPointLight3 = DX11CreateBasicPointLight(0.5f, { 50,100 }, 0.5f, olc::RED, { 100,100 }, 0.0f);

		HandleToBasicPointLight4 = DX11CreateBasicPointLight(0.5f, { 50,100 }, 0.5f, olc::BLUE, { 100,100 }, 0.0f);

		

		HandleToBasicPointLight5 = DX11CreateBasicPointLight(0.5f, { 50,100 }, 0.5f, olc::RED, { 400,100 }, 0.0f);

		HandleToBasicPointLight6 = DX11CreateBasicPointLight(0.5f, { 50,100 }, 0.5f, olc::BLUE, { 400,100 }, 0.0f);


		return true;
	}
	float counter = 0.2;
	bool OnUserUpdate(float fElapsedTime) override
	{

		counter += 0.1;

		Clear(olc::BLUE);
		//MoveParticleLayer(1);
		//SetDrawTarget(1);
		if (counter > 3.10) {
			counter = 0.1;
		}

		UpdateBasicPointLightData(HandleToBasicPointLight6, 0.5f, { 50,100 }, sin(counter), olc::BLUE, { 250,500 }, 0.0f); //add shine to middle of light source by making final item 1 - this inverses how I get lighting

		
		//BEWARE YOU CAN ENABLE LIGHT AND DISABLE AT ANY POINT IN TIME with DiableLight for Games or programs with UI sections 
		DrawBasicPointLight(HandleToBasicPointLight);

		DrawBasicPointLight(HandleToBasicPointLight2);

		
		DrawBasicPointLight(HandleToBasicPointLight4);
		DrawBasicPointLight(HandleToBasicPointLight3);

		DrawBasicPointLight(HandleToBasicPointLight5);
		DrawBasicPointLight(HandleToBasicPointLight6);
		DrawBasicPointLight(HandleToBasicPointLight7);


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

	if (feh.Construct(640, 640, 1, 1, false, true, false)) {

		feh.Start();
		return true;

	}

}
