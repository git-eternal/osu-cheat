#pragma once

#include <Windows.h>
#include "smart_handle.hpp"

namespace sed
{
	class suspend_guard
	{
	public:
		suspend_guard(suspend_guard &) = delete;
		suspend_guard(const suspend_guard &) = delete;

		suspend_guard() = default;
		suspend_guard(DWORD id_);
		~suspend_guard();

		suspend_guard(suspend_guard && other);
		suspend_guard & operator=(suspend_guard && rhs);
		operator bool() const noexcept;
		operator HANDLE() const noexcept;

		bool suspend();
		bool resume();

		DWORD get_id();

	private:
		sed::smart_handle thread_handle { nullptr };
		DWORD id { 0 };
	};
}