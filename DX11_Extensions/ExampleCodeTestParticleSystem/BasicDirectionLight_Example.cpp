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

	int HandleToBasicDirectionLight;

	int HandleToBasicDirectionLight2;



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

		//TODO: direction light (cone) with blocking of pixel for shadow to work - also need to soften edges?

		//olc::Pixel(0, 0, 0, 125) <-- no colored lights can cut down on shade/end-blur
		HandleToBasicDirectionLight = DX11CreateBasicDirectionLight(1.0f, 100, 1.0f, olc::Pixel(0, 0, 0, 125), { 300,300 }, true, 0.5f, 0, 90); //best results when sub 90 degree lights

		UpdateBasicDirectionLightData(HandleToBasicDirectionLight, 1.0f, 300, 1.0f, olc::BLACK, { 320, 320 }, true, 1.0f, 10, 80); //add shine to middle of light source by making final item 1 - this inverses how I get lighting
		
		HandleToBasicDirectionLight2 = DX11CreateBasicDirectionLight(1.0f, 300, 1.0f, olc::BLACK, { 300,300 }, true, 1.f, 90, 150); //best results when sub 90 degree lights



		return true;
	}
	float counter = 0.2;
	bool OnUserUpdate(float fElapsedTime) override
	{

		counter += 0.1;

		Clear(olc::BLUE);
		if (counter > 3.10) {
			counter = 0.1;
		}

		

		//BEWARE YOU CAN ENABLE LIGHT AND DISABLE AT ANY POINT IN TIME with DiableLight for Games or programs with UI sections 

		DrawBasicDirectionLight(HandleToBasicDirectionLight);
		
		DrawBasicDirectionLight(HandleToBasicDirectionLight2);

		

		
		DrawStringDecal({ 0,0 }, "Tests Overlap", olc::RED);



		SetDecalMode(olc::DecalMode::NORMAL); //diffrent
		
		DrawDecal(olc::vi2d(300, 100), dt.get(), { 4.0f, 4.0f });

		
		DrawSprite(olc::vi2d{ 0,570 }, st2.get());



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
