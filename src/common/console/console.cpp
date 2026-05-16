#include "console.hpp"

#include <stdio.h>
#include <Windows.h>

namespace console
{
	namespace
	{
		FILE* console = nullptr;
	}

	void open(const char* console_title)
	{
		AllocConsole();
		SetConsoleTitleA(console_title);
		freopen_s(&console, "CONOUT$", "w", stdout);
	}

	void close()
	{
		if (console) {
			fclose(console);
			console = nullptr;
		}
		FreeConsole();
	}
}
