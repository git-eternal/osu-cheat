#include <dncomlib/meta_host.hpp>
#include <utility>
#include <Windows.h>

dncomlib::meta_host::meta_host(ICLRMetaHost* i_)
	: dncomlib::unique_releasable<ICLRMetaHost>(i_)
{
}

auto dncomlib::meta_host::enumerate_loaded_runtimes(void* process_handle) -> dncomlib::unknown_enumerator
{
	IEnumUnknown* res{};

	if (instance->EnumerateLoadedRuntimes(reinterpret_cast<HANDLE>(process_handle), &res) != S_OK)
		return dncomlib::unknown_enumerator(nullptr);

	return dncomlib::unknown_enumerator(res);
}