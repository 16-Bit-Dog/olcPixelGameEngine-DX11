# BASIC DOCUMENTATION UNIQUE TO THIS DX IMPLEMENTATION:

to use DX11 - do #define OLC_GFX_DIRECTX11

This defaults to "OLC_GFX_DIRECTX11_FLIP_SEQUENTIAL" as the rendering technique - this means that **windows 7 with the platform update** and newer is supported

if you want MORE performance amd the userbase utilizes windows 10 - you can also put 
```#define OLC_GFX_DIRECTX11_FLIP_DISCARD```

example:
[include the olc pixelGameEngine header]
```
#define OLC_GFX_DIRECTX11
#define OLC_GFX_DIRECTX11_FLIP_DISCARD //requires windows 10
```
[your code...]


if you do not know the user base's operating system but want to dynamically choose the render api used - thats A-OK, just do the following:

inside the PGE header, next to the ```"#include <dxgi1_2.h>"``` - put ```#include <versionhelpers.h>```

now go to the code that looks like so:

```
#if defined(OLC_GFX_DIRECTX11_FLIP_DISCARD)
			WinVersion = 10;
			swapChainDescW.BufferCount = 2;
			swapChainDescW.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
#else
			WinVersion = 8;  
			swapChainDescW.BufferCount = 2; 
			swapChainDescW.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
#endif
```

change it to:
```
if (IsWindows10OrGreater()){
			WinVersion = 10;
			swapChainDescW.BufferCount = 2;
			swapChainDescW.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
}

else{ //you can use IsWindows7SP1OrGreater() to check if the user supports DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL
			WinVersion = 8;  
			swapChainDescW.BufferCount = 2; 
			swapChainDescW.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
}
```
[NOW THE PROGRAM USES DX11]

# COMPATABILITY WITH OPENGL:
- this implementation follows the logic of that it should look the same as its opengl counter parts - so this api is interchangable in use with OpenGL
- of course, DX is faster than OpenGl on hardware with worser openGL drivers (like all AMD GPU's ever unless running linux mesa drivers - remark made on 5/18/2021)
- drawing polygons will not be interchangable with opengl - it is the only thing that requires you to work around how DX works with triangle strips rather than triangle fans; if you work around how DX11 works, then opengl may produce the same results ( ONLY APPLIES TO DrawPolygonDecal() ) 
- any PGEX for directX11 may not work with openGL (most likley not work)

# FOR PEOPLE MAKING PGEX STUFF:
I left notes in the actual code which may help

# Features?
This may not be the official thing, but I can still add stuff...

# funny thing:
if you want to use DX11 but lack a supporting GPU... in the olc pixel game engine header, use ctrl+f and replace ```D3D_DRIVER_TYPE_HARDWARE``` with ```D3D_DRIVER_TYPE_SOFTWARE``` 
