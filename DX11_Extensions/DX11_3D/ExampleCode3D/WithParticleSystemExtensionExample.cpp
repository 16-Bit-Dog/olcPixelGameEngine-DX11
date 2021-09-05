#define OLC_PGE_APPLICATION
#define OLC_PLATFORM_WINAPI
#define OLC_GFX_DIRECTX11 
#define OLC_GFX_DIRECTX11_FLIP_DISCARD
#define OLC_PGEX_DIRECTX11_SHADERS_PLUS

#define OLC_PGEX_DIRECTX11_3D //put before olc pgex for 3d
#define OLC_PGE_DIRECTX11_3D //put before olc pge for enabling 3d camrea and matrix manipulation [not required, but reccomended for stuff like camrea control

//remember to link https://github.com/tinyobjloader/tinyobjloader/blob/master/tiny_obj_loader.h

#include "olcPixelGameEngine.h"  

#include "olcPGEX_DX11ShadersPlus.h"

#include "olcPGEX_DX11_3D.h"

using namespace SPDX11;

class FET : public olc::PixelGameEngine
{
public:
	FET()
	{
		sAppName = "TUTORIAL - DX Play Ground";
	}
public:

	std::unique_ptr<olc::Sprite> st;
	std::unique_ptr<olc::Sprite> st2;
	std::unique_ptr<olc::Sprite> st3;
	std::unique_ptr<olc::Decal> dt;
	std::unique_ptr<olc::Decal> dt2;
	std::unique_ptr<olc::Decal> dt3;

	int HandleToBasicPointLight;
	int ParticleSystemHandleReturn;
	int ParticleSystemHandleReturnt2;
	int ParticleSystemHandleReturnt3;
	int HandleToBasicDirectionLight;

	std::vector< DOLC11::M3DR > MyModels;
	
	bool OnUserCreate() override
	{
		Clear(olc::DARK_BLUE);
		st = std::make_unique<olc::Sprite>("./1.png");
		dt = std::make_unique<olc::Decal>(st.get());
		st2 = std::make_unique<olc::Sprite>("./2.png");
		dt2 = std::make_unique<olc::Decal>(st2.get());

		st3 = std::make_unique<olc::Sprite>("./3.png");
		dt3 = std::make_unique<olc::Decal>(st3.get());


		DOLC11::Initialize3DMode(this);

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

		DOLC11::M3DR MyModelNP = DOLC11::M3DR(st3.get(), "./ToroObj.obj");

		MyModelNP.MSRObject(std::array<float, 3>{100.0f, 100.0f, 100.0f}, std::array<float, 3>{0.5f, 0.5f, 0.5f}, std::array<float, 3>{1.0f, 0.0f, 1.0f}); //move, scale, rotate object [rotate in Radians]
		//MyModelNP.Translate() returns the current translation
		//MyModelNP.Scale() returns the current scale 
		//MyModelNP.Radians() returns the current rotation in radians
		//MyModelNP.Quaternion() returns the quaternion
		MyModels.push_back(MyModelNP);

		CreateLayer();
		EnableLayer(1,true); //made layer and made transparent so I can mix this particle system extension with 3d!

		SetLayerTint(0, olc::Pixel(255, 255, 255, 0) );
		SetLayerTint(1, olc::Pixel(255, 255, 255, 0) );




		return true;

	}
	bool falser = false;
	float counter = 0.2;


	
	bool OnUserUpdate(float fElapsedTime) override
	{
		//std::cout << "translate x" << MyModels[0].Translate()[0] <<"\n";
		//std::cout << "Radian x: "<< MyModels[0].Radians()[3] << "\n";
		//std::cout << "quat 'x' value: " << MyModels[0].Quaternion()[0] << "\n";

		std::array<float, 3> tr = MyModels[0].Translate();

		float adjust = 1.0f;

		if (tr[0] > ScreenWidth()) adjust = -1.5*ScreenWidth();

		MyModels[0].MSRObject(std::array<float, 3> {tr[0]+adjust,tr[1],tr[2]}, std::array<float, 3>{0.5f, 0.5f, 0.5f}, std::array<float, 3>{1.0f, 0.0f, 1.0f}); //move, scale, rotate object


		if (!GetKey(olc::Key::K0).bPressed) {
	//		camXRot += 45;
			camYRot += 1;
	//		camZRot += 45;
			moveLeftRight += 1;
			moveBackForward += 1;
		}


		InitializeShadersAndBase(1);
		
		DOLC11::Initialize3DShaders(0);


		DOLC11::DrawM(&MyModels[0], true); // there is tmp values optional to use - need to document this... *sigh*
		//draw before so I am behind transparent objects and appear... behind them... - since I draw on layer 0 a regular decal

		counter += 0.1;

		if (counter > 10.5) {
			counter = 0.1;
		}
		if (counter >= 1) {
			if (falser == false) {
				GlobalIlluminationResetChange(olc::Pixel(40, 40, 40, 255)); //notice how this has to be used later in run time rather than immediatly - this is because of how DX11 runtime works... need to give it a bit - I did not add sync to stop this, so you need to wait a few seconds before calling
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
