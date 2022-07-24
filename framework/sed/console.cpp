#include "console.hpp"
#include <cstdio>

static HWND   _conwindow_handle { nullptr };
static HANDLE _stdout_handle    { nullptr };

auto sed::console::init() -> bool
{
	if (AllocConsole())
	{
		FILE * fp { nullptr };
		_wfreopen_s(&fp, L"CONOUT$", L"w", stdout);
		_wfreopen_s(&fp, L"CONOUT$", L"w", stderr);
		_wfreopen_s(&fp, L"CONIN$", L"r", stdin);
	}

	_conwindow_handle = GetConsoleWindow();
	_stdout_handle    = GetStdHandle(STD_OUTPUT_HANDLE);

	if (!_conwindow_handle || !_stdout_handle)
		return false;

	return true;
}
