#include <dncomlib/runtime_host.hpp>

dncomlib::runtime_host::runtime_host(ICorRuntimeHost* i_)
	: dncomlib::unique_releasable<ICorRuntimeHost>(i_)
{
}

auto dncomlib::runtime_host::get_default_domain_thunk() -> dncomlib::unique_releasable<IUnknown>
{
	IUnknown* res{};
	instance->GetDefaultDomain(&res);
	return dncomlib::unique_releasable<IUnknown>(res);
}