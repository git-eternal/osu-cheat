#pragma once

#include <sed/macro.hpp>
#include <Windows.h>
#include <sed/memory.hpp>

namespace sdk
{
	union wnd_info_t
	{
		OC_UNS_PAD(0xC, HWND, handle)
	};

	class pp_wnd_info_t : public sed::basic_ptrptr<wnd_info_t> {};
}