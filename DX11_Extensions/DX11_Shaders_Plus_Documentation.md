# SIDE NOTE: 
- want more features, just ask! - want some behavior adjusted, just ask! - making a github accoutn is free, and asking me to better something I made is equally good!

- also, olc::WHITE means the tint color is nothing* something for who do not know

# Blend modes (for refrence if you hate reading header files):
		olc::DecalMode::NORMAL,
		olc::DecalMode::ADDITIVE,
		olc::DecalMode::MULTIPLICATIVE,
		olc::DecalMode::STENCIL,
		olc::DecalMode::ILLUMINATE,
		olc::DecalMode::WIREFRAME,
    
# Example Applications: 
every single item inside the header has a issolated example application with art work - https://github.com/16-Bit-Dog/olcPixelGameEngine-DX11/tree/master/DX11_Extensions/ExampleCodeTestParticleSystem

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
olc:vf2d InitialToEnd[2],
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

**(The Created Random Range particle system integers are unique to the random range particle systems - DO NOT draw using values created with others since they may not relate to anything inside Random Range particle systems).**
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
olc::vf2d InitialToEnd[2], 
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

# What is + How to use Random Life Time Particle System + Functions associated


