#include "stdafx.h"
#include "dllmain.h"


// Pathes game's .exe file to use my proxy-dll instead of original one
extern "C" void CALLBACK PatchExe(HWND hwnd, HINSTANCE hinst, LPCSTR lpszCmdLine, int nCmdShow)
{
	std::string old_name("d3d9.dll"); // original dll called by game
	std::string new_name("euXP.dll"); // my proxy-dll

	std::FILE *fp = std::fopen(lpszCmdLine, "r+b");
	if (fp) {
		// save backup copy of original file
		std::stringstream backup;
		backup << lpszCmdLine << ".bak";
		CopyFile(lpszCmdLine, backup.str().c_str(), FALSE);

		// read .exe file contents to string
		std::string contents;
		std::fseek(fp, 0, SEEK_END);
		contents.resize(std::ftell(fp));
		std::rewind(fp);
		std::fread(&contents[0], 1, contents.size(), fp);

		// replace original dll name with my proxy dll
		size_t pos;
		while( (pos = contents.find(old_name)) != std::string::npos) {
			contents.replace(pos, old_name.length(), new_name);
		}

		// write edited .exe file contents to disk
		std::rewind(fp);
		std::fwrite(contents.data(), contents.size(), 1, fp);
		std::fclose(fp);

		MessageBox(NULL, "Game patched!", "Stellaris WinXP fix", MB_OK);
	} else {
		std::stringstream ss;
		ss <<  "Can't open " << lpszCmdLine;
		MessageBox(NULL, ss.str().c_str(), "Stellaris WinXP fix", MB_OK);
	}
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//MessageBox(NULL, "d3d9.dll", "Stellaris WinXP fix", MB_OK);
		//break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
