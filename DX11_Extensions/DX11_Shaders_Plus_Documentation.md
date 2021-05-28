# SIDE NOTE: 
- want more features, just ask! - want some behavior adjusted, just ask! - making a github accoutn is free, and asking me to better something I made is equally good!

- also, olc::WHITE means the tint color is nothing* something for who do not know

# Blend modes (for future refrence)
		olc::DecalMode::NORMAL,
		olc::DecalMode::ADDITIVE,
		olc::DecalMode::MULTIPLICATIVE,
		olc::DecalMode::STENCIL,
		olc::DecalMode::ILLUMINATE,
		olc::DecalMode::WIREFRAME,
    
# Example Applications: 
every single item inside the header has a issolated example application with each particle system (or utility system) - https://github.com/16-Bit-Dog/olcPixelGameEngine-DX11/tree/master/DX11_Extensions/ExampleCodeTestParticleSystem

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

1. store particle system identifier in an int --> int SYSTEM_HANDLE = DX11CreateRandomRangeParticleSystem(...);
2. draw particle system with DrawRandomRangeParticleSystem(SYSTEM_HANDLE);
3. regenerate random positions with RegenRRforRandomRange(SYSTEM_HANDLE);

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
int i
)
```
- i: The integer handle to System To Draw from the Random Range Particle Systems 

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
int i
)
```
- i: The integer handle to System To Draw from the Random Life Time Particle Systems 

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

# What is + How to use VecAddBasicComputeFloat + Functions associated


### What is VecAddBasicComputeFloat
- add 2 arrays of floats (index 0 of array 1 adds with index 0 of array 2, ect.) with the max input vector size of 67107840 elements on a GPU for quick calculation versus a CPU and intrinsic's - and this result is returned when called 

### How to use VecAddBasicComputeFloat
1. 1. store particle system identifier in an int --> int SYSTEM_HANDLE = DX11CreateVecAddBasicComputeFloat(...);
2. std::vector<float> output = DispatchVecAddBasicFloat(SYSTEM_HANDLE);
3. OPTIONAL: AdjustVecAddBasicFloat(...); to change values of compute system
	
### Random Life Time Particle System Functions:
```
int DX11CreateVecAddBasicComputeFloat(
int elementCount, 
std::vector<float> vec1, 
std::vector<float> vec2
)
```
- elementCount: starting at index 0, how many elements do we read from the vectors vec1 and vec2
- vec1: input float vector to add
- vec2: input float vector to add
	
**(The CreatedVecAddBasicComputeFloat compute system integers are unique to the system type - DO NOT draw using values created with others since they may not relate to anything outside this systems).**
---
```
std::vector<float> DispatchVecAddBasicFloat(
int system
)
```
- system: BasicComputeFloat system to launch 

runs and retirives result of BasicComputeFloat system based on inputted system id
	
---
```
void AdjustVecAddBasicFloat(
int system, 
int elementCount, 
std::vector<float> vec1, 
std::vector<float> vec2) 
```	
- system: system from BasicComputeFloat to change values of
- elementCount: starting at index 0, how many elements do we read from the vectors vec1 and vec2
- vec1: input float vector to add (changes old vec1)
- vec2: input float vector to add (changes old vec2)
	
adjust and change BasicComputeFloat system values of inputted system
---
	
#LIGHTING IS UNDER CONSTRUCTION 😅
