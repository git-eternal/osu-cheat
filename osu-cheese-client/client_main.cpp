// Main entry point of the client

#include <Windows.h>
#include <sed/windows/smart_handle.hpp>
#include "oc_client.hpp"

auto WINAPI DllMain(HINSTANCE inst, DWORD reason, LPVOID res0) -> BOOL
{
	if (reason == DLL_PROCESS_ATTACH) sed::smart_handle(CreateThread(nullptr, NULL, [](LPVOID inst) -> DWORD
	{
		if (!oc::load())
			FreeLibraryAndExitThread(HMODULE(inst), 0);

		return 0;
		
	}, inst, NULL, nullptr));

	return 0;
}