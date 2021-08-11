#define OLC_PGE_APPLICATION
#define OLC_PLATFORM_WINAPI
#define OLC_GFX_DIRECTX11 
#define OLC_GFX_DIRECTX11_FLIP_DISCARD
#define OLC_PGEX_DIRECTX11_SHADERS_PLUS
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
	int HandleToBasicPointLight;
	int ParticleSystemHandleReturn;
	int ParticleSystemHandleReturnt2;
	int ParticleSystemHandleReturnt3;
	int HandleToBasicDirectionLight;
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
		float  opacityStrengthRange[2] = { 180.0f,255.0f }; 
		float  opacityChangeRange[2] = { 0.05f, 0.1f };
		ParticleSystemHandleReturn = DX11CreateRandomLifeTimeParticleSystem(100, true, 0, 256, true, olc::vf2d(0.0f, 0.0f), olc::vf2d(float(ScreenWidth()), float(ScreenHeight())), opacityStrengthRange, opacityChangeRange, { olc::vf2d(200.0f, 200.0f),  olc::vf2d(200.0f, 200.0f) }, { olc::vf2d(-0.0005f, -0.0005f),  olc::vf2d(0.0005f, 0.0005f) }, { olc::vf2d(-0.00005f, -0.00005f),  olc::vf2d(0.00005f, 0.00005f) }, st.get(), { 2.0f,2.0f }, olc::WHITE);
		HandleToBasicPointLight = DX11CreateBasicPointLight(1.0f, { 50,100 }, 1.0f, olc::Pixel(0, 0, 0, 125), { 300,300 }, 0.0f); 
		ParticleSystemHandleReturnt2 = DX11CreateRandomRangeParticleSystem(30, { olc::vf2d(100.0f,0.0f),olc::vf2d(100.0f,100.0f) }, st.get(), { 2.0f,2.0f }, olc::WHITE);
		UpdateBasicPointLightData(HandleToBasicPointLight, 1.0f, { 50,40 }, 1.0f, olc::WHITE, { 400,300 }, 0.0f); 
		ParticleSystemHandleReturnt3 = DX11CreateTestParticleSystem({ 100,0 }, st.get(), { 2.0f,2.0f }, olc::WHITE);
		HandleToBasicDirectionLight = DX11CreateBasicDirectionLight(1.0f, 100, 1.0f, olc::WHITE, { 200,400 }, true, 1.0f, -80, 90); //cannot do 360 degree round lights		ChangeLightBlendMode(olc::DecalMode::ILLUMINATE); 
		//WIREFRAME and ILLUMINATE
		return true;
	}
	bool falser = false;
	float counter = 0.2;
	bool OnUserUpdate(float fElapsedTime) override
	{
		InitializeShadersAndBase();

		counter += 0.1;
		Clear(olc::BLUE);
		if (counter > 10.5) {
			counter = 0.1;
		}
		if (counter >= 1) {
			if (falser == false) {
 				GlobalIlluminationResetChange(olc::Pixel(40, 40, 40,255)); //notice how this has to be used later in run time rather than immediatly - this is because of how DX11 runtime works... need to give it a bit - I did not add sync to stop this, so you need to wait a few seconds before calling
				falser = true;
				
			}
		}
		DrawRandomLifeTimeParticleSystem(ParticleSystemHandleReturn);
		DrawRandomRangeParticleSystem(ParticleSystemHandleReturnt2);
		RegenRRforRandomRange(ParticleSystemHandleReturnt2);
		DrawBasicPointLight(HandleToBasicPointLight);
		DrawTestParticleSystem(ParticleSystemHandleReturnt3);
		DrawBasicDirectionLight(HandleToBasicDirectionLight);

		DrawStringDecal({ 0,0 }, "Tests Overlap", olc::RED);
		SetDecalMode(olc::DecalMode::NORMAL); //diffrent
		DrawDecal(olc::vi2d(200, 300), dt.get(), { 4.0f, 4.0f });
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
