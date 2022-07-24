#include "memory.hpp"

#include <iostream>

auto sed::abs2rel32(void * from, std::size_t size, void * to) -> std::int32_t
{
	return static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(to) - (reinterpret_cast<std::uintptr_t>(from) + size));
}

auto sed::rel2abs32(void * instruction, std::size_t size) -> std::uintptr_t
{
	std::uintptr_t next = reinterpret_cast<std::uintptr_t>(instruction) + size;
	return next + *reinterpret_cast<std::int32_t *>(next - sizeof(std::int32_t));
}

auto sed::abs32(void * instruction, std::size_t size) -> std::uintptr_t
{
	return *reinterpret_cast<std::uintptr_t *>(reinterpret_cast<std::uintptr_t>(instruction) + size - sizeof(std::uintptr_t));
}

auto sed::op1rel32_apply(std::uint8_t opcode, void * from, void * to) -> bool
{
	std::uint8_t shell[] = { opcode, 0x00, 0x00, 0x00, 0x00 };
	*reinterpret_cast<std::uintptr_t *>(shell + 1) = sed::abs2rel32(from, sizeof(shell), to);

	DWORD oprot { 0 };
	if (!VirtualProtect(from, sizeof(shell), PAGE_EXECUTE_READWRITE, &oprot))
		return false;

	for (int i = 0; i < sizeof(shell); ++i)
		reinterpret_cast<std::uint8_t *>(from)[i] = shell[i];

	if (!VirtualProtect(from, sizeof(shell), oprot, &oprot))
		return false;

	return true;
}

auto sed::jmprel32_apply(void * from, void * to) -> bool
{
	return sed::op1rel32_apply(0xE9, from, to);
}

auto sed::callrel32_apply(void * from, void * to) -> bool
{
	return sed::op1rel32_apply(0xE8, from, to);
}

auto sed::pattern_scan(void * start_, std::size_t size, const char * pattern, const char * mask) -> std::uintptr_t
{
	std::uint8_t * start = reinterpret_cast<decltype(start)>(start_);
	const auto     len   = strlen(mask);
	
	for (std::size_t rva = 0; rva < size - len; ++rva)
	{
		for (int pat_i = 0; pat_i < len; ++pat_i)
		{
			if (mask[pat_i] == '?')
				continue;

			if (mask[pat_i] != 'x' || pattern[pat_i] != reinterpret_cast<const char *>(start)[rva + pat_i])
				break;

			if (pat_i == len - 1)
				return reinterpret_cast<std::uintptr_t>(start_) + rva;
		}
	}

	return 0;
}

auto sed::pattern_scan_exec_region(void * start_, std::size_t size, const char * pattern, const char * mask) -> std::uintptr_t
{
	std::uint8_t * current = reinterpret_cast<decltype(current)>(start_);
	std::uint8_t * end     = size == -1 ? reinterpret_cast<std::uint8_t *>(-1) : current + size;

	MEMORY_BASIC_INFORMATION mbi { 0 };
	while (VirtualQuery(current, &mbi, sizeof(mbi)) && current < end)
	{
		constexpr DWORD any_execute = PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;
		if (mbi.State != MEM_COMMIT || !(mbi.Protect & any_execute))
		{
			current += mbi.RegionSize;
			continue;
		}
		
		if (auto match = sed::pattern_scan(current, mbi.RegionSize, pattern, mask); match)
			return match;

		current += mbi.RegionSize;
	}

	return 0;
}

auto sed::_impl_pattern_scan(const pattern_fragment * fragments, int count) -> void *
{
	std::uint8_t * current = nullptr;

	MEMORY_BASIC_INFORMATION mbi { 0 };
	while (VirtualQuery(current, &mbi, sizeof(mbi)))
	{
		constexpr DWORD any_execute = PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;
		if (mbi.State != MEM_COMMIT || !(mbi.Protect & any_execute))
		{
			current += mbi.RegionSize;
			continue;
		}

		for (auto pscan = current, end = current + mbi.RegionSize - count; pscan < end; ++pscan)
		{
			for (int i = 0; i < count; ++i)
			{
				const auto & frag = fragments[i];

				if (!frag.masked)
					continue;

				if (frag.byte != pscan[i])
					break;

				if (i == count - 1)
					return pscan;
			}
		}

		current += mbi.RegionSize;
	}

	return nullptr;
}