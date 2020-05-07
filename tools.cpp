#include <windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <ios>

namespace tools
{
	void RedirectIOToConsole(int posX, int posY, int maxConsoleLines)
	{
		CONSOLE_SCREEN_BUFFER_INFO coninfo;

		AllocConsole();

		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);

		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
		coninfo.dwSize.Y = maxConsoleLines;
		SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

		HWND consoleWindow = GetConsoleWindow();
		SetWindowPos(consoleWindow, 0, posX, posY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}
}
