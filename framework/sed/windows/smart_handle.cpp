#include "smart_handle.hpp"

#include <utility>

sed::smart_handle::smart_handle()
	: hnd(nullptr) {}

sed::smart_handle::smart_handle(HANDLE hnd_)
	: hnd(hnd_) {}

sed::smart_handle::~smart_handle()
{
	if (this->hnd)
		CloseHandle(this->hnd);

	this->hnd = nullptr;
}

sed::smart_handle::smart_handle(smart_handle && other)
{
	*this = std::move(other);
}

sed::smart_handle & sed::smart_handle::operator =(sed::smart_handle && rhs)
{
	if (this->hnd)
		CloseHandle(this->hnd);

	this->hnd = rhs.hnd;
	rhs.hnd = nullptr;

	return *this;
}

sed::smart_handle::operator HANDLE() const noexcept
{
	return this->hnd;
}

sed::smart_handle::operator bool() const noexcept
{
	return this->hnd != nullptr;
}
