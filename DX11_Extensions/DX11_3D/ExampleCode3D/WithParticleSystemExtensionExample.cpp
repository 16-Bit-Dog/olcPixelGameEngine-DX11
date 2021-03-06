//TODO: refactor this so it makes sense for documentation
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
	std::unique_ptr<olc::Sprite> st4;
	std::unique_ptr<olc::Sprite> st5;

	std::unique_ptr<olc::Decal> dt;
	std::unique_ptr<olc::Decal> dt2;
	std::unique_ptr<olc::Decal> dt3;
	std::unique_ptr<olc::Decal> dt4;
	std::unique_ptr<olc::Decal> dt5;

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
		st2 = std::make_unique<olc::Sprite>("./2.jpg");
		dt2 = std::make_unique<olc::Decal>(st2.get());

		st3 = std::make_unique<olc::Sprite>("./3.png");
		dt3 = std::make_unique<olc::Decal>(st3.get());

		st4 = std::make_unique<olc::Sprite>("./specular.jpg");
		dt4 = std::make_unique<olc::Decal>(st4.get());
	
		st5 = std::make_unique<olc::Sprite>("./normal.jpg");
		dt5 = std::make_unique<olc::Decal>(st5.get());


		InitializeParticlesWorker(this);
		EnableLight();
		std::vector<float[2]> blockPix = {};
		float  opacityStrengthRange[2] = { 180.0f,255.0f };
		float  opacityChangeRange[2] = { 0.05f, 0.1f };
		ParticleSystemHandleReturn = DX11CreateRandomLifeTimeParticleSystem(1, true, 0, 256, true, olc::vf2d(0.0f, 0.0f), olc::vf2d(float(ScreenWidth()), float(ScreenHeight())), opacityStrengthRange, opacityChangeRange, { olc::vf2d(200.0f, 200.0f),  olc::vf2d(200.0f, 200.0f) }, { olc::vf2d(-0.0005f, -0.0005f),  olc::vf2d(0.0005f, 0.0005f) }, { olc::vf2d(-0.00005f, -0.00005f),  olc::vf2d(0.00005f, 0.00005f) }, st.get(), { 2.0f,2.0f }, olc::WHITE);
		HandleToBasicPointLight = DX11CreateBasicPointLight(1.0f, { 50,100 }, 1.0f, olc::Pixel(0, 0, 0, 125), { 300,300 }, 0.0f);
		UpdateBasicPointLightData(HandleToBasicPointLight, 1.0f, { 50,40 }, 1.0f, olc::WHITE, { 400,300 }, 0.0f);
		
		DOLC11::Initialize3DMode(this);

		DOLC11::EnableDebugLights();

		DOLC11::SetMaxLights(5);
		//zq
		DOLC11::M3DR MyModelNP = DOLC11::M3DR(st2.get(), "./sq.fbx", true); //the true at parameter 3 makes me turn on armature mode... much slower to draw

		MyModelNP.MSRObject(std::array<float, 3>{0.0f, -50.0, 0.0f}, std::array<float, 3>{0.5f, 0.5f, 0.5f}, std::array<float, 3>{1.0f, 0.0f, 1.0f}); //move, scale, rotate object [rotate in Radians]
		
		DOLC11::SetLightPosition(0, 0.0f, -50.0f, -400.0f);
		
		DOLC11::SetLightPosition(1, 0.0f, 50.0f, -200.0f);

		DOLC11::SetGlobalAmbient({0.05f,0.05f,0.05f,1.0f}); //affects non lit as well sinc
		//MyModelNP.Translate() returns the current translation
		//MyModelNP.Scale() returns the current scale 
		//MyModelNP.Radians() returns the current rotation in radians
		//MyModelNP.Quaternion() returns the quaternion
		
		MyModelNP.SetTexEqual(dt2.get(), 0, DOLC11::AMBIENT_TEX); //decal is now equal to the olc::decal - all non-temp changes to decal carries over to model -- the 0 means object 0 is set with this texture incase fbx has many models
		MyModelNP.SetTexEqual(dt2.get(), 0, DOLC11::DIFFUSE_TEX); //decal is now equal to the olc::decal - all non-temp changes to decal carries over to model -- the 0 means object 0 is set with this texture incase fbx has many models


		MyModelNP.MakeAnimVCache(20, 0); //60 intevals for animation - works only if 1 anim exists
		MyModelNP.MakeAnimVCache(60, 1); //60 intevals for animation - works only if 2 anims exist

		MyModelNP.SetTexEqual(dt4.get(),0, DOLC11::SPECULAR_TEX); //"set object to tex value" larger than max obj count means all get fixed to texture
		MyModelNP.SetTexEqual(dt5.get(),0, DOLC11::NORMAL_TEX);


		MyModelNP.SetUseAmbientTexture(100, true); //set all tex to have and use tex in model by choosing greater than max mat count
		MyModelNP.SetUseDiffuseTexture(100, true); //set all tex to have and use tex in model by choosing greater than max mat count
		//MyModelNP.SetUseNormalTexture(100, true); //set all tex to have and use tex in model by choosing greater than max mat count
		
		MyModelNP.SetFaceDrawType(0);

		MyModels.push_back(MyModelNP);

		CreateLayer();
		EnableLayer(1,true); //made layer and made transparent so I can mix this particle system extension with 3d!

		SetLayerTint(0, olc::Pixel(255, 255, 255, 0) );
		SetLayerTint(1, olc::Pixel(255, 255, 255, 0) );

		DOLC11::AddToCamAngle(3.2f, 0.0f, 0.0f); // tilt right along x axis - pitch

		
		return true;

	}
	bool falser = false;
	float counter = 0.2;

	float animc = 0.0f;
	int animNum = 0;

	int boneANum = 0;

	bool OnUserUpdate(float fElapsedTime) override
	{
//		Clear(olc::DARK_BLUE);


	//	if (GetKey(olc::Key::U).bPressed) {
			MyModels[0].SetAllBoneToAnim(animc, animNum, true); //animation time of 2 seconds is iterated to and pose is set to that -- true == use anim cache is stated, only works if anim cache exists - I made it with MakeAnimVCache
			animc += fElapsedTime;
	//	}
		if (GetKey(olc::Key::P).bPressed) {
			animNum++;
			boneANum++;
		}
		if (animc >= MyModels[0].EndTimeOfAnim(animNum)) {
			animc = 0;
		}

		std::cout << "cam pos: " << DOLC11::ReturnCamPosition()[0] << "\n";

		//std::cout << "translate x" << MyModels[0].Translate()[0] <<"\n";
		//std::cout << "Radian z: "<< MyModels[0].Radians()[2] << "\n";
		//std::cout << "Radian y: "<< MyModels[0].Radians()[1] << "\n";
		//std::cout << "quat 'x' value: " << MyModels[0].Quaternion()[0] << "\n";

		std::array<float, 3> tr = MyModels[0].Translate();

		float adjust = 1.0f;

		if (tr[0] > ScreenWidth()) adjust = -1.5*ScreenWidth();

		//MyModels[0].MSRObject(std::array<float, 3> {tr[0]+adjust,tr[1],tr[2]}, std::array<float, 3>{0.5f, 0.5f, 0.5f}, std::array<float, 3>{-1.0f, 1.5f, 1.0f}); //move, scale, rotate object


		std::array<float, 3> t2 = { 0,0,0};
		std::array<float, 3> r2 = { 0,1.5,0};
		std::array<float, 3> s2 = { 1,1,1 };

		if (GetKey(olc::Key::B).bPressed) {
			MyModels[0].MSRBone(boneANum, t2, s2, r2); //slow but it does a thing with moving bones individually
		}
		
		

		if (GetKey(olc::Key::W).bPressed) {
			DOLC11::AddToCamAngle(0.0f,-0.5f, 0.0f); // tilts up by 0.5 rad more
		}
		if (GetKey(olc::Key::D).bPressed) {
			DOLC11::AddToCamAngle(0.5f, 0.0f, 0.0f); // tilt right along x axis - pitch
		}
		if (GetKey(olc::Key::A).bPressed) {
			DOLC11::AddToCamAngle(-0.5f, 0.0f,0.0f); // tilt left along the x axis by -0.5 rad - pitch
		}
		if (GetKey(olc::Key::S).bPressed) {
			DOLC11::AddToCamAngle(0.0f,0.5f, 0.0f); // add -0.5 radians to angle of yaw, also known as the vertical up down y axis - tilts down
		}
		if (GetKey(olc::Key::RIGHT).bPressed) {
			DOLC11::SetEndFrameMoveCam(100, 0, 0); //100 pixel right
		}
		if (GetKey(olc::Key::LEFT).bPressed) {
			DOLC11::SetEndFrameMoveCam(-100, 0, 0); //100 pixels left
		}
		if (GetKey(olc::Key::UP).bPressed) {
			DOLC11::SetEndFrameMoveCam(0, 0, 100); //100 pixels forward
		}
		if (GetKey(olc::Key::DOWN).bPressed) {
			DOLC11::SetEndFrameMoveCam(0, 0, -200); //100 pixels backward
		}
		if (GetKey(olc::Key::T).bPressed) {
			DOLC11::SetEndFrameMoveCam(0, 100, 0); //100 pixels up
		}
		if (GetKey(olc::Key::G).bPressed) {
			DOLC11::MoveCamNowWithCurrentRotation(0, -100, 0); //100 pixels camrea down - slower because it does not delay until end of frame to move the cam position
		}
		if (GetKey(olc::Key::L).bPressed) {
			DOLC11::LerpCamPos(true, true, true, 500, 500, -800, 2.0f); //lerp cam to position 1000 in 3 seconds
		}
		if (GetKey(olc::Key::L).bPressed) {
			DOLC11::LerpCamRot(true, true, true, 3, 6, 6, 2.0f); //lerp cam to rotation radians in 3 seconds
		}
		//notes of things people may notice with this example and find bothersome
		//MoveCamAsIfRotationIs would mean that if tilting forward I can still move as if not tilted
		//MoveCamNowWithCurrentRotation moves Camrea now* - but its slower than the end frame method and therefore bad practice if not needed
		DrawDecal(olc::vi2d(200, 300), dt.get(), { 4.0f, 4.0f });



		InitializeShadersAndBase(1);
		
		DOLC11::Initialize3DShaders(0);

		tr = MyModels[0].Translate();

		
		DOLC11::DrawM(&MyModels[0], true, true, { tr[0]-100,tr[1]-100,tr[2]-300 }, { 0.01,0.01,0.01 }, {0.0f,0.0f,0.0f}); // there is tmp values optional to use - need to document this... *sigh* - also the before var - means draw before everything... which puts this behind decal if I want, right now its false
		DOLC11::DrawM(DOLC11::GetDebugLightObject(0), true);

																												   
																												   //DOLC11::DrawM(&MyModels[0]);
		//MyModels[0].ExtractV(0);
		if (GetKey(olc::Key::C).bPressed) {
			MyModels[0].LitToggle(100, !MyModels[0].GetLitToggle(0));
		}

																				  
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
		DrawBasicPointLight(HandleToBasicPointLight);
		
		DrawStringDecal({ 0,0 }, "Tests Overlap", olc::RED);
		SetDecalMode(olc::DecalMode::NORMAL); //diffrent
	//	DrawSprite(olc::vi2d{ 0,570 }, st2.get());
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
