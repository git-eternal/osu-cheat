#include "protect_guard.hpp"

sed::protect_guard::protect_guard(void * address, std::size_t size, DWORD prot_flags)
//	: address(address), size(size)
{
	if (!VirtualProtect(address, size, prot_flags, &this->oprot))
		return;

	this->address = address;
	this->size    = size;
}

sed::protect_guard::~protect_guard()
{
	if (!*this || !VirtualProtect(this->address, this->size, this->oprot, &this->oprot))
		return;

	this->address = nullptr;
	this->size = 0;
}

sed::protect_guard::operator bool() const noexcept
{
	return this->address && this->size;
}
