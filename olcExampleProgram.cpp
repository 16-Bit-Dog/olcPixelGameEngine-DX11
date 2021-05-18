#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
//get ready for AMD gpu's to get a new crazy good level of performance
#define OLC_GFX_DIRECTX11 // windows 7 platform update or later supports this
#define OLC_GFX_DIRECTX11_FLIP_DISCARD //remove this if not windows 10

class Example : public olc::PixelGameEngine
{
public:
	Example()
	{
		sAppName = "Example";
	}

public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// called once per frame
		for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
				Draw(x, y, olc::Pixel(rand() % 255, rand() % 255, rand()% 255));	
		return true;
	}
};


int main()
{
	Example demo;
	if (demo.Construct(256, 240, 4, 4))
		demo.Start();

	return 0;
}
