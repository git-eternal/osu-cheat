#include <dncomlib/dncomlib.hpp>

#include <Windows.h>

auto dncomlib::clr_create_instance(const IID& clsid, const IID& riid) -> dncomlib::meta_host
{
	ICLRMetaHost* res{};

	HMODULE hmscoree = LoadLibraryW(L"MSCorEE.dll");

	if (hmscoree)
	{
		decltype(CLRCreateInstance)* _CLRCreateInstance;
		_CLRCreateInstance = reinterpret_cast<decltype(_CLRCreateInstance)>(GetProcAddress(hmscoree, "CLRCreateInstance"));

		if (_CLRCreateInstance)
			_CLRCreateInstance(clsid, riid, reinterpret_cast<LPVOID*>(&res));
	}

	return dncomlib::meta_host(res);
}