#include "stdafx.h"
#include "d3d11.h"


extern "C" HRESULT WINAPI D3D11CreateDevice(
	void				*pAdapter,
	UINT				DriverType,
    HMODULE             Software,
    UINT                Flags,
    void				*pFeatureLevels,
    UINT                FeatureLevels,
    UINT                SDKVersion,
    void		        **ppDevice,
    void				*pFeatureLevel,
    void                **ppImmediateContext
)
{
	return D3DERR_NOTAVAILABLE;
}