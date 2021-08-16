# SIDE NOTE: 
- want more features, just ask! - want some behavior adjusted, just ask! - making a github account is free, and asking me to better something I made is equally good!

- also, olc::WHITE means the tint color is nothing* something for who do not know

- #define's for compiling less shaders are provided as an option to speed up initial-load-time and/or Video RAM

- one more thing - either use ctrl+f to navigate, or use the .md heading navigator (to jump to headers of the markdown file)

# Blend modes (for future refrence)
		olc::DecalMode::NORMAL,
		olc::DecalMode::ADDITIVE,
		olc::DecalMode::MULTIPLICATIVE,
		olc::DecalMode::STENCIL,
		olc::DecalMode::ILLUMINATE, (when using light blend modes, Illuminate was adjusted to create a realistic ability to "un-darken" and provide soft light tinting if the background is of appropriate color) 
		olc::DecalMode::WIREFRAME, (when using light blend modes, Wireframe was adjusted to create a realistic ability to "un-darken" and add lighting color flare) 

# Shader Don't compile macros (use only if you don't use these shaders and want to save Video RAM and/or speed up initial-load-time)
*the #define name tells you which shaders it does not compile - ex: ZERO_POINT_LIGHT_S means you turned off point light compilation (and the point light function cannot be used any longer later in the code base)
- ZERO_TEST_SHADERS_S
- ZERO_RANDOM_RANGE_S
- ZERO_RANDOM_LIFETIME_S
- ZERO_POINT_LIGHT_S
- ZERO_DIRECTION_LIGHT_S
- ZERO_RESET_LIGHT_MAP_S

# Example Applications: 
every single item inside the header has a issolated example application with each particle system or utility system [in the Shaders_Plus folder with this markdown file] - https://github.com/16-Bit-Dog/olcPixelGameEngine-DX11/tree/master/DX11_Extensions

# BASIC START UP STEPS:
1. #define OLC_GFX_DIRECTX11 
2. OPTIONAL (better performance if on windows 10 - else this cannot be enabled): #define OLC_GFX_DIRECTX11_FLIP_DISCARD
3. #define OLC_PGEX_DIRECTX11_SHADERS_PLUS

4. inside OnUserCreate() before anything with the header is used, you MUST add `InitializeParticlesWorker(this);` like in the sample below
5. ```bool OnUserCreate() override  
	{

    InitializeParticlesWorker(this);
    
    
    //more code goes below
    ```
6. if you are doing lighting

---

# What is + How to use Random Range Particle System + Functions associated

### What it is:
- a particle system which is VERY performant for creating randomly appearing particles which just dissapear each time drawn 
- you choose bounds to draw, scale of image, tint, and element count  

### How To use Random Range Particle System:

1. put `using namespace SPDX11` (to not deal with namespace function calling)
2. in `void OnUserCreate` put before any DX11_Shaders_Plus function calls `InitializeParticlesWorker(this);`
3. in `void OnUserUpdate` put before everything (first item) - `InitializeShadersAndBase();` <-- this only has to run a minimum of once, and this case is handled internally
4. store particle system identifier in an int --> int SYSTEM_HANDLE = DX11CreateRandomRangeParticleSystem(...);
5. draw particle system with DrawRandomRangeParticleSystem(SYSTEM_HANDLE);
6. regenerate random positions with RegenRRforRandomRange(SYSTEM_HANDLE);

### Random Range Particle System Functions:
```
int DX11CreateRandomRangeParticleSystem(
int elementCount,
const std::array<olc:vf2d,2> InitialToEnd,
olc::Sprite sprite,
olc::vf2d scale, 
olc::Pixel tint
)
```
- elementCount: int which determines the number of sprites to draw
- InitialToEnd: [0] is start position in screen space (.x and .y from vf2d). [1] is the x and y off set from [0] that makes the range (square) to draw in 
- sprite: is a pointer to the sprite to draw
- scale: vf2d where .x is the scale of x and .y is scale of y (1.0f means unchanged since scale of 1)
- tint: olc::Pixel to tint with (r,g,b,a) <-- olc::WHITE means (255,255,255,255) which is zero tint

This Function creates a random range particle system - adding to the global count of them by 1 --> this value of the particle system made is returned as an integer. The Random Range particle system integers are unique to the random range particle systems 

**(The Created Random Range particle system integers are unique to the random range particle systems - DO NOT draw using values created with others since they may not relate to anything outside Random Range particle systems).**
---

```
void DrawRandomRangeParticleSystem(
int i,
bool before
)
```
- i: The integer handle to System To Draw from the Random Range Particle Systems 
- before: defaults to false; if true, draws the element BEFORE all decals (else it draws after)
draws a random range particle system

---
```
void RegenRRforRandomRange(
int System
)
```
- System: integer to system to regenerate random values for (for when drawing) 

recreates random values for random range particle system

---
```
void RandomRangeParticleClassChangeBlend (
int System,
const olc::DecalMode& mode
)
```

- System: integer to system to change blend mode of
- mode: blend mode from list of olc::Pixel Game engine blend modes (ex: olc::DecalMode::NORMAL)

changes blend mode of a specific system
---

```
int RandomRangeParticleClassCount()
```

returns integer size of how many random range particle systems exist

---
```
void AdjustRandomRangeParticleClass(
int System, 
int elementCount, 
const std::array<olc::vf2d,2> InitialToEnd, 
olc::Sprite* sprite, 
const olc::vf2d& scale, 
olc::Pixel tint,
) 
```
- System: integer handle to random range particle system to change
- elementCount: int which determines the number of sprites to draw
- InitialToEnd: [0] is start position in screen space (.x and .y from vf2d). [1] is the x and y off set from [0] that makes the range (square) to draw in
- sprite: is a pointer to the sprite to draw
- scale: vf2d where .x is the scale of x and .y is scale of y (1.0f means unchanged since scale of 1)
- tint: olc::Pixel to tint with (r,g,b,a) <-- olc::WHITE means (255,255,255,255) which is zero tint

---
```
int RandomRangeParticleClassCount()
```

returns integer number of Random Range Partical Systems made

---
```
std::pair<ID3D11UnorderedAccessView*, ID3D11ShaderResourceView*> GetTextureRandomRangeParticleSystem(
int i)

```
- i: integer to system to retrive texture data from

The only important part of retriving texture data is that this can be used for functions such as RunTextureModifierShader() as input for modifying of texture data on the fly

---

# What is + How to use Random Life Time Particle System + Functions associated

### What is Random Life Time Particle Systems
- based on many parameters of the users choice you can create dyanmically moving particles with life time (can also have a velocity of 0 and replace random range particles albet at a performance cost)

### How to use Random Life Time Particle System
1. 1. store particle system identifier in an int --> int SYSTEM_HANDLE = DX11CreateRandomLifeTimeParticleSystem(...);
2. draw particle system with DrawRandomLifeTimeParticleSystem(SYSTEM_HANDLE);

### Random Life Time Particle System Functions:

```
int DX11CreateRandomLifeTimeParticleSystem(
int elementCount, 
bool regenBasedOnAlpha, 
float regenerateAlphaRangeLow, 
float regenerateAlphaRangeHigh, 
bool regenBasedOnDist, 
const olc::vf2d regenerateDistRangeLow, 
const olc::vf2d regenerateDistRangeHigh, 
float opacityStrengthRange[2], 
float opacityChangeRange[2], 
const std::array<olc::vf2d, 2> InitialAndIncrease, 
const std::array<olc::vf2d, 2> InitialVelocityRange, 
const std::array<olc::vf2d, 2> AccelXYRange, 
olc::Sprite* sprite, 
const olc::vf2d& scale, 
olc::Pixel tint
)
```
- elementCount: integer of how many individual particles to have
- regenBasedOnAlpha: turn on (if true) the ability to kill and regenerate new particles based on current alpha value of each individual particles 
- regenerateAlphaRangeLow: minimum alpha until a particle regenerates [0 to 255 - but can be less than or greater than such values to cause a delay until degradation] (if regenBasedOnAlpha == true)
- regenerateAlphaRangeHigh: maximum alpha until a particle regenerates [0 to 255 - but can be less than or greater than such values to cause a delay until degradation] (if regenBasedOnAlpha == true)
- regenBasedOnDist: turn on (if true) the ability to kill and regenerate new particles based on current position in window  
- regenerateDistRangeLow: .x and .y is window position minimum until particles regenerate [screen size minimum to screen size maximum - but can be less than or greater than such values to cause a delay until degradation] (MUST BE LESS THAN OR EQUAL TO regenerateDistRangeHigh)
- regenerateDistRangeHigh: .x and .y is window position maximum until particles regenerate [screen size minimum to screen size maximum - but can be less than or greater than such values to cause a delay until degradation] (MUST BE GREATER THAN OR EQUAL TO regenerateDistRangeLow)
- opacityStrengthRange[2]: range of possible particle alpha to regenerate particles with [0 to 255] [0] is min, and [1] is max --> min must be less than or equal to max
- float opacityChangeRange[2]: range of possible alpha change values to adjust indiviual particles with every time DrawRandomLifeTimeParticleSystem() is called on a system [0] is minimum rate of change value, and [1] is maximum rate of change value --> min must be less than or equal to max 
- InitialAndIncrease: [0] is start position in screen space (.x and .y from vf2d). [1] is the x and y off set from [0] that makes the range (square) to draw in
- InitialVelocityRange: .x and .y of [0] is minimum of range for rate of change of position (velocity). [1] .x and .y is maximum possable rate of change for position (velocity) --> min must be less than max 
- AccelXYRange: .x and .y of [0] is minimum of range for rate of change of velocity. [1] .x and .y is maximum possable rate of change for velocity  
- sprite: pointer to sprite object to draw with
- scale: vf2d where .x is the scale of x and .y is scale of y (1.0f means unchanged since scale of 1)
- tint: olc::Pixel to tint with (r,g,b,a) <-- olc::WHITE means (255,255,255,255) which is zero tint

This Function creates a random life time particle system - adding to the global count of them by 1 --> this value of the particle system made is returned as an integer. The Random life time particle system integers are unique to the random life time particle systems 

**(The Created Random life time particle system integers are unique to the random life time particle systems - DO NOT draw using values created with others since they may not relate to anything outside Random life time particle systems).**
---
```
void AdjustRandomLifeTimeParticleSystem(
int i, 
int elementCount, 
bool regenBasedOnAlpha, 
float regenerateAlphaRangeLow, 
float regenerateAlphaRangeHigh, 
bool regenBasedOnDist, 
const olc::vf2d regenerateDistRangeLow, 
const olc::vf2d regenerateDistRangeHigh, 
float opacityStrengthRange[2], 
float opacityChangeRange[2], 
const std::array<olc::vf2d, 2> InitialAndIncrease, 
const std::array<olc::vf2d, 2> InitialVelocityRange, 
const std::array<olc::vf2d, 2> AccelXYRange, 
olc::Sprite* sprite, 
const olc::vf2d& scale = { 1.0f,1.0f }, 
olc::Pixel tint = olc::WHITE
)
```
- i: system to modify from the RandomLifeTime Particle Systems available
- elementCount: integer of how many individual particles to have
- regenBasedOnAlpha: turn on (if true) the ability to kill and regenerate new particles based on current alpha value of each individual particles 
- regenerateAlphaRangeLow: minimum alpha until a particle regenerates [0 to 255 - but can be less than or greater than such values to cause a delay until degradation] (if regenBasedOnAlpha == true)
- regenerateAlphaRangeHigh: maximum alpha until a particle regenerates [0 to 255 - but can be less than or greater than such values to cause a delay until degradation] (if regenBasedOnAlpha == true)
- regenBasedOnDist: turn on (if true) the ability to kill and regenerate new particles based on current position in window  
- regenerateDistRangeLow: .x and .y is window position minimum until particles regenerate [screen size minimum to screen size maximum - but can be less than or greater than such values to cause a delay until degradation] (MUST BE LESS THAN OR EQUAL TO regenerateDistRangeHigh)
- regenerateDistRangeHigh: .x and .y is window position maximum until particles regenerate [screen size minimum to screen size maximum - but can be less than or greater than such values to cause a delay until degradation] (MUST BE GREATER THAN OR EQUAL TO regenerateDistRangeLow)
- opacityStrengthRange[2]: range of possible particle alpha to regenerate particles with [0 to 255] [0] is min, and [1] is max --> min must be less than or equal to max
- float opacityChangeRange[2]: range of possible alpha change values to adjust indiviual particles with every time DrawRandomLifeTimeParticleSystem() is called on a system [0] is minimum rate of change value, and [1] is maximum rate of change value --> min must be less than or equal to max 
- InitialAndIncrease: [0] is start position in screen space (.x and .y from vf2d). [1] is the x and y off set from [0] that makes the range (square) to draw in
- InitialVelocityRange: .x and .y of [0] is minimum of range for rate of change of position (velocity). [1] .x and .y is maximum possable rate of change for position (velocity) --> min must be less than max 
- AccelXYRange: .x and .y of [0] is minimum of range for rate of change of velocity. [1] .x and .y is maximum possable rate of change for velocity  
- sprite: pointer to sprite object to draw with
- scale: vf2d where .x is the scale of x and .y is scale of y (1.0f means unchanged since scale of 1)
- tint: olc::Pixel to tint with (r,g,b,a) <-- olc::WHITE means (255,255,255,255) which is zero tint

This function adjusts values of a pre-existing Random Life Time Particle system

---
```
void DrawRandomLifeTimeParticleSystem(
int i,
bool before
)
```
- i: The integer handle to System To Draw from the Random Life Time Particle Systems 
- before: defaults to false; if true, draws the element BEFORE all decals (else it draws after)

draws a Random Life Time Particle Systems

---
```
int RandomLifeTimeParticleSystemDeathCount(
int i
) 
```
- i: integer handle to System To Draw from the Random Life Time Particle Systems 

returns integer of how many dead particles so far are associated with this particle system 

---
```
void RandomLifeTimeParticleClassChangeBlend(
int System, 
const olc::DecalMode& mode
)
```
- System: integer to system to change blend mode of
- mode: blend mode from list of olc::Pixel Game engine blend modes (ex: olc::DecalMode::NORMAL)

changes blend mode of a specific system 
---
```
int RandomLifeTimeParticleClassCount()
```

returns integer number of Random Life Time Partical Systems made

---
```
void RandomLifeTimeParticleClassResetDeathCount(
int i
)
```
- i: integer to system to reset death count of (to 0)

reset system death count to 0

---
```
std::pair<ID3D11UnorderedAccessView*, ID3D11ShaderResourceView*> GetTextureRandomLifeTimeParticleSystem(
int i)

```
- i: integer to system to retrive texture data from

The only important part of retriving texture data is that this can be used for functions such as RunTextureModifierShader() as input for modifying of texture data on the fly

---
# What is + How to use VecBasicComputeFloat2 + Functions associated


### What is VecBasicComputeFloat2
- take in 2 arrays/vectors of floats add modify 1 by 1 for each index (index 0 of array 1 will do math with array 2 index 0 ect...) with the max input vector size of 67107840 elements per vector on a GPU for quick calculation versus a CPU and intrinsic's - and this result is returned when called

### How to use VecBasicComputeFloat2
1. store particle system identifier in an int --> int SYSTEM_HANDLE = DX11CreateVecBasicComputeFloat2(...);
2. use NewMathForVecBasicFloat2(system, "x+y"); (or input what ever math you want to modify x and y --> x is array 1, array 2 is y)
3. std::vector<float> output = DispatchVecBasicFloat2(SYSTEM_HANDLE);
4. OPTIONAL: AdjustVecBasicFloat2(...); to change values of compute system vectors
	
### Random Life Time Particle System Functions:
```
int DX11CreateVecBasicComputeFloat2(
int elementCount, 
std::vector<float> vec1, 
std::vector<float> vec2
)
```
- elementCount: starting at index 0, how many elements do we read from the vectors vec1 and vec2 (67107840 is the max element count per vector)
- vec1: input float vector to modify as x
- vec2: input float vector to modify as y
	
This function creates a VecBasicComputeFloat2 type for doing math with and on the GPU  
**(The Created VecBasicComputeFloat2 compute system integers are unique to the system type - DO NOT draw using values created with others since they may not relate to anything outside this systems).**
---
```
NewMathForVecBasicFloat2(
int system,
std::string ModifyMath)
```
- system: BasicComputeFloat2 system to modify shader of 
- ModifyMath: a string of math that contains x and y, like (as some examples)
	``"x+y"`` or ``sin(x)*cos(y)/y*log(x)``

loads a new shader to do math with for when preparing to use DispatchVecBasicFloat2 --> YOU MUST CALL THIS AT-LEAST ONCE BEFORE DISPATCHING TO GET A USABLE RESULT. If you need help with usable mathimatical symbols and functions, you can ask, or look at the functions inside the HLSL math function library

---
```
std::vector<float> DispatchVecBasicFloat2(
int system
)
```
- system: BasicComputeFloat2 system to launch 

runs and retirives result of BasicComputeFloat2 system based on inputted system id (runs loaded shader - you must load a shader, nothing is defaulted)
	
---
```
void AdjustVecBasicFloat2(
int system, 
int elementCount, 
std::vector<float> vec1, 
std::vector<float> vec2) 
```	
- system: system from BasicComputeFloat2 to change values of
- elementCount: starting at index 0, how many elements do we read from the vectors vec1 and vec2
- vec1: input float vector to change as x (changes old vec1)
- vec2: input float vector to change as y (changes old vec2)

adjust and change BasicComputeFloat2 system values of inputted system

---
	
# What is + How to use VecBasicComputeFloat + Functions associated


### What is VecBasicComputeFloat
- do math with any number of float vectors up to the size of 67107840 elements per vector on a GPU for quick calculation versus a CPU and intrinsic's - and this result is returned when called. Does math with index 1 of vector 0,1,2,3 ect... in an issolated manner --> if you do not want to do math in such an issolated manner (index by index) with this shader, send a github issue and I can give direct instructions onto how to do math without this functional limitation.  

### How to use VecBasicComputeFloat
1. 1. store particle system identifier in an int --> int SYSTEM_HANDLE = DX11CreateVecBasicComputeFloat(...);
2. use NewMathForVecBasicFloat(system, "vec0*vec1*vec3"); (or input what ever math you want)
3. std::vector<float> output = DispatchVecBasicFloat(SYSTEM_HANDLE);
4. OPTIONAL: AdjustVecBasicFloat(...); to change values of compute system
	
### VecBasicComputeFloat Functions:
```
int DX11CreateVecBasicComputeFloat(
int elementCount, 
std::vector<std::vector<float>> vec
)
```
- elementCount: starting at index 0, how many elements do we read from the vectors (per vector — max is 67107840 elements per vector
- vec: vector of vectors which are passed on in order as vec0 (for vector index 0), vec1, vec2, ect... for when the procedural math will be done in the compute shader (and made for the shader).

This function creates a VecBasicComputeFloat type for gpu compute workloads
**(The Created VecBasicComputeFloat compute system integers are unique to the system type - DO NOT draw using values created with others since they may not relate to anything outside this systems).**
---
```
NewMathForVecBasicFloat(
int system,
std::string ModifyMath)
```
- system: BasicComputeFloat system to modify shader of 
- ModifyMath: a string of math that contains vec0 and vec1 (pattern continues for each extra vector that is added), like (as some examples)
	``"vec0+vec2*vec1"`` or ``sin(vec0)*cos(vec1)/y*log(vec4)+vec2-vec3``

loads a new shader to do math with for when preparing to use DispatchVecBasicFloat --> YOU MUST CALL THIS AT-LEAST ONCE BEFORE DISPATCHING TO GET A USABLE RESULT. If you need help with usable mathimatical symbols and functions, you can ask, or look at the functions inside the HLSL math function library

---
```
std::vector<float> DispatchVecBasicFloat(
int system
)
```
- system: BasicComputeFloat system to launch 

runs and retirives result of BasicComputeFloat system based on inputted system id (runs loaded shader - you must load a shader, nothing is defaulted)
	
---
```
void AdjustVecBasicFloat(
int system, 
int elementCount, 
std::vector<std::vector<float>> vec
```	
- system: system from BasicComputeFloat to change values of
- elementCount: starting at index 0, how many elements do we read from the vectors (per vector — max is 67107840 elements per vector
- vec: vector of vectors which are passed on in order as vec0 (for vector index 0), vec1, vec2, ect... for when the procedural math will be done in the compute shader (and made for the shader).

adjust and change BasicComputeFloat system values of inputted system

---
# What is + How to use TextureModifierShader + Functions associated


### What is TextureModifierShader
- input a decal, or texture from particle system (using the texture get function) - and modify it based on a user made shader 

### How to use TextureModifierShader
1. ChangeTextureModifierShader()
2. RunTextureModifierShader()
	
### TextureModifierShader Functions:
int CreateTextureModifierShader(
	std::string ModifyMathR, 
	std::string ModifyMathG, 
	std::string ModifyMathB, 
	std::string ModifyMathA)

- ModifyMathR: string for modifing red data from inputted texture ('r' is the current texture red value of pixel being read at the time
- ModifyMathG: string for modifing red data from inputted texture ('g' is the current texture green value of pixel being read at the time
- ModifyMathB: string for modifing blue data from inputted texture ('b' is the current texture red value of pixel being read at the time
- ModifyMathA: string for modifing alpha data from inputted texture ('a' is the current texture red value of pixel being read at the time

create a shader (and return int as handle to this TextureModifierShader system) to modify a texture --> this is done through modifying color values (which are already normalized to 0.0f-1.0f, so make sure color data is divided by 255 to be normalized to 0-1); example of using symantics would be ("r+0.1" "b*2", "g*r","1.0f-a") [intrinsics like sin, cos, tan, log, and others listed under HLSL intrinsics are usable as well]

---
int ChangeTextureModifierShader(
	int i,
	std::string ModifyMathR, 
	std::string ModifyMathG, 
	std::string ModifyMathB, 
	std::string ModifyMathA)

- int i: handle to system of TextureModifierShader to change shader of
- ModifyMathR: string for modifing red data from inputted texture ('r' is the current texture red value of pixel being read at the time
- ModifyMathG: string for modifing red data from inputted texture ('g' is the current texture green value of pixel being read at the time
- ModifyMathB: string for modifing blue data from inputted texture ('b' is the current texture red value of pixel being read at the time
- ModifyMathA: string for modifing alpha data from inputted texture ('a' is the current texture red value of pixel being read at the time

re-create a shader (and return int as handle to this TextureModifierShader system) to modify a texture --> this is done through modifying color values (which are already normalized to 0.0f-1.0f, so make sure color data is divided by 255 to be normalized to 0-1); example of using symantics would be ("r+0.1" "b*2", "g*r","1.0f-a") [intrinsics like sin, cos, tan, log, and others listed under HLSL intrinsics are usable as well]

---
void RunTextureModifierShader(
	int System,
	olc::Decal* Decal)
or
void RunTextureModifierShader(
	int System,
	std::pair<ID3D11UnorderedAccessView*,ID3D11ShaderResourceView*> Decal)

- System is the TextureModifierShader to run 
- the second parameter of Decal is the target to modify texture data of
	
run user made shader to modify texture data inputted directly off of the GPU; the second variant has data aquired  from GetTextureRandomLifeTimeParticleSystem() and related functions to this. 
	
---	
### generic texture copy functions for decals
	
void GenericTextureCopy(int System, std::pair<ID3D11UnorderedAccessView*, ID3D11ShaderResourceView*> DecalOUT, std::pair<ID3D11UnorderedAccessView*, ID3D11ShaderResourceView*> DecalIN)
void GenericTextureCopy(int System, std::pair<ID3D11UnorderedAccessView*, ID3D11ShaderResourceView*> DecalOUT, olc::Decal* DecalIN)
void GenericTextureCopy(int System, olc::Decal* DecalOUT, olc::Decal* DecalIN)
void GenericTextureCopy(int System, olc::Decal* DecalOUT, std::pair<ID3D11UnorderedAccessView*, ID3D11ShaderResourceView*> DecalIN)

- DecalOUT: Decal to change using DecalIN 
- DecalIN: Texture to change DecalOUT with

This function changes olc::Decal textures and my shader/particle system textures efficently through (ideally without looking at drivers) only GPU pass through - the inputs for the function for simplification can just be olc::Decal* and GetTextureRandomLifeTimeParticleSystem() [or related functions to retrive texture]

# LIGHTING IS UNDER CONSTRUCTION 😅
once I make a better light blend math system, and add some essentials like global illumination - I will add documentation. Despite this, the point lights, directional lights, and global illumination (budget fake lights of course) is all 'fully' working with examples in the examples section of the repos
