#include <dncomlib/app_domain.hpp>

#include <oleauto.h>

dncomlib::app_domain::app_domain(IUnknown* i_)
	: dncomlib::unique_releasable<IUnknown>(i_)
{
}

auto dncomlib::app_domain::get_friendly_name() -> std::wstring
{
	// x86: (_WORD *)(*(int (__stdcall **)(int, _WORD **))(*(_DWORD *)a3 + 212))(a3, &v78)
	// x64: (*(__int64 (__fastcall **)(__int64, _WORD **))(*(_QWORD *)v98 + 424i64))(v98, &v89);
	BSTR name{};

	if (vcall<53, HRESULT>(&name) != S_OK)
		return std::wstring();

	auto out = std::wstring(name);

	if (name)
		SysFreeString(name);

	return out;
}

auto dncomlib::app_domain::from_unknown(const dncomlib::unique_releasable<IUnknown>& unk) -> app_domain
{
	GUID _appdomain_uuid
	{
			.Data1 = 0x5F696DC,
			.Data2 = 0x2B29,
			.Data3 = 0x3663,
			.Data4 = { 0xAD, 0x8B, 0xC4, 0x38, 0x9C, 0xF2, 0xA7, 0x13 }
	};

	IUnknown* res{};

	unk->QueryInterface(_appdomain_uuid, reinterpret_cast<void**>(&res));

	return app_domain(res);
}