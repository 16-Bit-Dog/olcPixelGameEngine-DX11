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

	int HandleToBasicDirectionLight;

	int HandleToBasicDirectionLight2;


	int HandleToBasicDirectionLight3;

	int HandleToBasicDirectionLight4;


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



		
		//olc::Pixel(0, 0, 0, 125) <-- low alpha colored lights can cut down on shade/end-blur
		HandleToBasicDirectionLight = DX11CreateBasicDirectionLight(1.0f, 100, -1.0f, olc::Pixel(0, 0, 0, 125), { 300,300 }, true, 0.5f, 0, 90); //best results when sub 90 degree lights

		UpdateBasicDirectionLightData(HandleToBasicDirectionLight, 1.0f, 100, 1.0f, olc::WHITE, { 200, 200}, true, 1.0f, 190, 350); 
		
		HandleToBasicDirectionLight2 = DX11CreateBasicDirectionLight(1.0f, 100, 1.0f, olc::YELLOW, { 400,200 }, true, 1.0f, 0, 170); //best results when sub 90 degree lights 

		HandleToBasicDirectionLight3 = DX11CreateBasicDirectionLight(1.0f, 100, 1.0f, olc::WHITE, { 200,400}, true, 1.0f, -80, 90); //cannot do 360 degree round lights

		HandleToBasicDirectionLight4 = DX11CreateBasicDirectionLight(1.0f, 100, 1.0f, olc::BLUE, { 400,400}, true, 1.0f, 120, 280); //best results when sub 90 degree lights

		ChangeLightBlendMode(olc::DecalMode::ILLUMINATE); //changed to be the "normal" mode for light - play with WIREFRAME as well!
		//WIREFRAME [always keeps color and lets more of the color of light through and ILLUMINATION (lets a scene to be uncovered from darkness when using WHITE very neatly) for light is both diffrent blend modes specifically modified for light

		return true;
	}
	float counter = 0.2;
	bool OnUserUpdate(float fElapsedTime) override
	{
		InitializeShadersAndBase();
		
		counter += 0.1;

		Clear(olc::Pixel(0,0,255,255));
		if (counter > 3.10) {
			counter = 0.1;
		}

		

		//BEWARE YOU CAN ENABLE LIGHT AND DISABLE AT ANY POINT IN TIME with DiableLight for Games or programs with UI sections 

		DrawBasicDirectionLight(HandleToBasicDirectionLight);
		
		DrawBasicDirectionLight(HandleToBasicDirectionLight2);

		DrawBasicDirectionLight(HandleToBasicDirectionLight3);
		
		DrawBasicDirectionLight(HandleToBasicDirectionLight4);


		
		DrawStringDecal({ 0,0 }, "Tests Overlap", olc::RED);

		//ChangeLightBlendMode(olc::DecalMode::MULTIPLICATIVE);

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
