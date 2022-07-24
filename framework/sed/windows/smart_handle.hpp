#pragma once

#include <Windows.h>

namespace sed
{
	class smart_handle
	{
	public:
		// Disable copying
		smart_handle(smart_handle &)       = delete;
		smart_handle(const smart_handle &) = delete;

		smart_handle();
		smart_handle(HANDLE hnd_);
		~smart_handle();
		
		smart_handle(smart_handle && other);
		smart_handle & operator =(smart_handle && rhs);

		operator HANDLE() const noexcept;
		operator bool() const noexcept;
	private:
		HANDLE hnd;
	};
}