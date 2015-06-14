# EU4_WinXP_fix
Patch for Europa Universalis 4 to work on WinXp 32-bit

## Problem description
Since version 1.12.0 EU4 doesn't support WinXP anymore.

EU4 can't start on WinXP 32-bit because from now on game uses newer system functions aren't provided by this old OS.

## Solution
This project is a proxy-dll that implements absent functions and redirects existing ones to the original system dll-s.

## How to use
1. Go to Releases page.
2. Download fix for your game version.
3. Unpack archive to the game folder.

## How to build & apply by yourself
1. Build
  1. Open project in Visual Studio.
  2. Choose Release configuration.
  3. Build project --> you get EU4_WinXP_fix.dll
2. Apply
  1. Copy dll to the game folder and rename it into zernel32.dll
  2. Open eu4.exe in any HEX editor ([XVI32] (http://www.chmaas.handshake.de/delphi/freeware/xvi32/xvi32.htm)) and replace all encounters of kernel32.dll with zernel32.dll. 

## Acknowledgments
- This project is based on Sara Jessica Leen's patch for XCOM: Enemy Unknown (http://forums.steampowered.com/forums/showthread.php?t=2964408).
- Nice proxy-dll tutorial by Kontza: http://www.codeproject.com/Articles/17863/Using-Pragmas-to-Create-a-Proxy-DLL
