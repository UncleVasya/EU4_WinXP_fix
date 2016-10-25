#include <PeLib.h>
#include <iomanip>
#include "stdafx.h"
#include "dllmain.h"


template<int bits>
void patchPeHeader(PeLib::PeFile& pef)
{
	PeLib::PeHeaderT<bits>& peh = static_cast<PeLib::PeFileT<bits>&>(pef).peHeader();

	// 5.1 is WinXP version number
	peh.setMajorOperatingSystemVersion(5);
	peh.setMinorOperatingSystemVersion(1);
	
	peh.setMajorSubsystemVersion(5);
	peh.setMinorSubsystemVersion(1);

	// write updated header to disk
	peh.write(pef.getFileName(), pef.mzHeader().getAddressOfPeHeader());
}

class PatchPeHeaderVisitor : public PeLib::PeFileVisitor
{
public:
    virtual void callback(PeLib::PeFile32 &file) {patchPeHeader<32>(file);}
    virtual void callback(PeLib::PeFile64 &file) {patchPeHeader<64>(file);}
};

// pathes file's imports to use my proxy-dll instead of original one
void patchImports(LPCSTR filename) 
{
	std::string old_name("KERNEL32.dll"); // original dll called by game
	std::string new_name("ZERNEL32.dll"); // my proxy-dll

	std::FILE *fp = std::fopen(filename, "r+b");

	// read game file contents to string
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

	// write edited file contents to disk
	std::rewind(fp);
	std::fwrite(contents.data(), contents.size(), 1, fp);
	std::fclose(fp);
}

// patches file's PE header to make it compatible with WinXP
void patchHeader(LPCSTR filename)
{
	PeLib::PeFile* pef = PeLib::openPeFile(filename);
	pef->readMzHeader();
	pef->readPeHeader();

	PatchPeHeaderVisitor vis;
	pef->visit(vis);

	delete pef;
}

// patches given game file to make it compatible with WinXP
extern "C" void CALLBACK PatchFile(HWND hwnd, HINSTANCE hinst, LPCSTR lpszCmdLine, int nCmdShow)
{
	std::FILE *fp = std::fopen(lpszCmdLine, "r+b");
	if (fp) {
		// save backup copy of original file
		std::stringstream backup;
		backup << lpszCmdLine << ".bak";
		CopyFile(lpszCmdLine, backup.str().c_str(), FALSE);

		// patch file
		patchImports(lpszCmdLine);
		patchHeader(lpszCmdLine);

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
		//MessageBox(NULL, "zernel32.dll", "Stellaris WinXP fix", MB_OK);
		//break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}