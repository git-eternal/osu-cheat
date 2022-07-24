#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <Windows.h>
#include <Psapi.h>
#include <memory>
#include <array>
#include "windows/protect_guard.hpp"

namespace sed
{
	auto abs2rel32(void * from, std::size_t size, void * to) -> std::int32_t;
	auto rel2abs32(void * instruction, std::size_t size) -> std::uintptr_t;
	auto abs32(void * instruction, std::size_t size) -> std::uintptr_t;

	auto op1rel32_apply(std::uint8_t opcode, void * from, void * to) -> bool;
	auto jmprel32_apply(void * from, void * to) -> bool;
	auto callrel32_apply(void * from, void * to) -> bool;

	class mempatch_interface
	{
	public:
		virtual bool patch() = 0;
		virtual bool restore() = 0;
		virtual operator bool() const noexcept = 0;
	};

	template <std::uint8_t opcode>
	class basic_mempatch_op1r32 : public mempatch_interface
	{
	public:
		basic_mempatch_op1r32(void * from, void * to)
			: from(from), to(to) {}

		bool patch() override
		{
			auto prot = sed::protect_guard(this->from, 0x5);
			if (!prot)
				return false;

			// backup
			std::memcpy(this->restore_buffer, this->from, 0x5);

			// patch
			std::uint8_t shell[] = { opcode, 0x00, 0x00, 0x00, 0x00 };
			*reinterpret_cast<std::uintptr_t *>(shell + 1) = sed::abs2rel32(this->from, sizeof(shell), this->to);
			std::memcpy(this->from, shell, sizeof(shell));

			this->patched = true;
			return true;
		}

		bool restore() override
		{
			if (!this->patched)
				return true;

			auto prot = sed::protect_guard(this->from, 0x5);
			if (!prot)
				return false;

			std::memcpy(this->from, this->restore_buffer, 0x5);
			this->patched = false;
			return true;
		}

		operator bool() const noexcept override
		{
			return this->patched;
		}

	private:
		bool   patched { false   };
		void * from    { nullptr },
			 * to      { nullptr };

		std::uint8_t restore_buffer[5] { 0x00 };
	};

	using mempatch_jmpr32  = basic_mempatch_op1r32<0xE9>;
	using mempatch_callr32 = basic_mempatch_op1r32<0xE8>; 
	
	[[deprecated("Use compile time IDA pattern scanner")]] auto pattern_scan(void * start_, std::size_t size, const char * pattern, const char * mask) -> std::uintptr_t;
	[[deprecated("Use compile time IDA pattern scanner")]] auto pattern_scan_exec_region(void * start_, std::size_t size, const char * pattern, const char * mask) -> std::uintptr_t;

	struct pattern_fragment
	{
		unsigned char byte;
		bool masked;
	};

	template <int len> requires (len % 3 == 0)
	struct ida_pattern
	{
		consteval ida_pattern(const char (& pattern)[len])
		{
			for (int i = 0; i < len / 3; ++i)
			{
				auto chunk = &pattern[i * 3];

				auto char_to_nibble = [](const char c) -> unsigned char
				{
					if (c == '?')
						return 0;

					if (c >= '0' && c <= '9')
						return c - '0';
					else if (c >= 'A' && c <= 'F')
						return c - 'A' + 0xA;
					else if (c >= 'a' && c <= 'f')
						return c - 'a' + 0xa;

					return 0;
				};

				unsigned char byte = char_to_nibble(chunk[0]) << 4 | char_to_nibble(chunk[1]);

				fragments[i] = {
					.byte = byte,
					.masked = chunk[0] != '?'
				};
			}
		}

		std::array<pattern_fragment, len / 3> fragments {};
	};

	// Implementation, do not use.
	auto _impl_pattern_scan(const pattern_fragment * fragments, int count) -> void *;

	template <ida_pattern pattern, typename T>
	auto pattern_scan(/*vargs_t... offsets*/) -> T *
	{
		return reinterpret_cast<T *>(_impl_pattern_scan(pattern.fragments.data(), pattern.fragments.size()));
	}

	template <class data>
	class basic_ptrptr
	{
	public:
		auto operator*() -> data *
		{
			if (this->ptr && *this->ptr)
				return *this->ptr;

			return nullptr;
		}

		auto operator->() -> data *
		{
			if (this->ptr && *this->ptr)
				return *this->ptr;

			return &this->dummy;
		}

		operator bool() const noexcept
		{
			return this->ptr && *this->ptr != nullptr;
		}

	public:
		data ** ptr    { nullptr };
		data dummy     { };
	};

}