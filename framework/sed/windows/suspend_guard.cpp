#include "suspend_guard.hpp"

#include <utility>

sed::suspend_guard::suspend_guard(DWORD id_)
{
	if (!id_)
		return;

	this->thread_handle = OpenThread(THREAD_ALL_ACCESS, FALSE, id_);
	if (!this->thread_handle)
		return;

	if (SuspendThread(this->thread_handle) == static_cast<DWORD>(-1))
	{
		this->thread_handle = nullptr;
		return;
	}

	this->id = id_;
}

sed::suspend_guard::~suspend_guard()
{
	if (this->thread_handle)
		ResumeThread(this->thread_handle);

	this->id = 0;
	this->thread_handle = nullptr;
}

sed::suspend_guard::suspend_guard(suspend_guard && other)
{
	*this = std::move(other);
}

sed::suspend_guard & sed::suspend_guard::operator=(suspend_guard && rhs)
{
	if (this->thread_handle)
		ResumeThread(this->thread_handle);

	this->id = rhs.id;
	this->thread_handle = std::move(rhs.thread_handle);

	rhs.id = 0;
	rhs.thread_handle = nullptr;

	return *this;
}

sed::suspend_guard::operator bool() const noexcept
{
	return this->id && this->thread_handle;
}

sed::suspend_guard::operator HANDLE() const noexcept
{
	return this->thread_handle;
}

bool sed::suspend_guard::suspend()
{
	return this->thread_handle && SuspendThread(this->thread_handle) != static_cast<DWORD>(-1);
}

bool sed::suspend_guard::resume()
{
	return this->thread_handle && ResumeThread(this->thread_handle) != static_cast<DWORD>(-1);
}

DWORD sed::suspend_guard::get_id()
{
	return this->id;
}