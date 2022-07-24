#include <dncomlib/runtime_info.hpp>

dncomlib::runtime_info::runtime_info(ICLRRuntimeInfo* i_)
	: dncomlib::unique_releasable<ICLRRuntimeInfo>(i_)
{
}

auto dncomlib::runtime_info::is_started(DWORD* out_startup_flags) -> bool
{
	BOOL  flag{};
	DWORD dummy_flags{};

	if (instance->IsStarted(&flag, out_startup_flags ? : /* hello msvc compiler users :) */ &dummy_flags) != S_OK)
		return false;

	return flag;
}

auto dncomlib::runtime_info::get_host() -> runtime_host
{
	ICorRuntimeHost* res{};

	instance->GetInterface(CLSID_CorRuntimeHost, IID_ICorRuntimeHost, reinterpret_cast<LPVOID*>(&res));

	return runtime_host(res);
}

auto dncomlib::runtime_info::from_unknown(const dncomlib::unique_releasable<IUnknown>& unk) -> runtime_info
{
	ICLRRuntimeInfo* res{};

	unk->QueryInterface(IID_ICLRRuntimeInfo, reinterpret_cast<void**>(&res));

	return runtime_info(res);
}