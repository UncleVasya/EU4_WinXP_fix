#include "stdafx.h"
#include "d3d9.h"

extern "C" HRESULT WINAPI Direct3DCreate9Ex(UINT SDKVersion, IDirect3D9Ex **ppD3D)
{
	// Surprisingly this is enough to launch game on XP.
	// Game engine will fall back to the old Direct3DCreate9.
	return D3DERR_NOTAVAILABLE;
}
