#pragma once

#include <cstdint>
#include "osu_vec.hpp"
#include <sed/memory.hpp>

namespace sdk
{
	// osu!master\osu!\GameplayElements\HitObjects\HitObjectBase.cs
	enum class hit_type : std::uint32_t
	{
		Normal = 1,
		Slider = 2,
		NewCombo = 4,
		NormalNewCombo = 5,
		SliderNewCombo = 6,
		Spinner = 8,
		ColourHax = 112,
		Hold = 128,
		ManiaLong = 128
	};

	enum class sound_type : std::uint32_t
	{
		None = 0,
		Normal = 1,
		Whistle = 2,
		Finish = 4,
		Clap = 8
	};

	struct hitobject
	{
	private:
		// The 0xC is from a register but seems to be constant, perhaps this is due
		// to not being able to encode 0x10 into the move instruction while only being
		// able to encode 0x4.
		// mov esi, [eax+ecx+0x04] where EAX is 0xC
		char pad[0x4 + 0xC];

	public:
		struct
		{
			std::int32_t start;
			std::int32_t end;
		} time;

		hit_type type;
		sound_type sound_type;
		std::int32_t segment_count;
		std::int32_t segment_length;
		/*double*/ char spatial_length[4]; // internally a double but it's only 4 bytes when reversed so it's padded instead since double is 8 bytes
		std::int32_t combo_color_offset;
		std::int32_t combo_color_index;
		std::uint32_t raw_color;
		sdk::vec2 position;
		// not sure with the rest below
		std::int32_t stack_count;
		std::int32_t last_in_combo;
	};

	struct ho_array
	{
	private:
		char pad[0x8];
	public:
		// Array of 4 byte pointers
		// mov ecx,[eax+ebx*4+08]
		hitobject * hitobjects[];
	};

	struct ho_vector
	{
	private:
		char pad[0x4];
	public:
		ho_array * container;
	private:
		char pad1[0x4];
	public:
		std::uint32_t count;

	public:
		auto get_coming_hitobject(std::uint32_t time) -> std::pair<hitobject *, int>;
		auto begin() -> hitobject *;
		auto end() -> hitobject *;
	};

	struct ho_1
	{
	private:
		// 0x44 is in register EAX but seems to be constant
		// mov ecx,[eax+ecx+0x04]
		char pad[0x04 + 0x44];
	public:
		ho_vector * ho_vec;
	};

	struct ho_2
	{
	private:
		char pad[0x3C];
	public:
		ho_1 * ho1;
	};

	struct hitobject_pointer
	{
	private:
		char pad[0x60];
	public:
		ho_2 * ho2;
	};

	class pp_phitobject_t : public sed::basic_ptrptr<hitobject_pointer>
	{
	public:
		auto begin() -> hitobject *;
		auto end() -> hitobject *;

		auto get_coming_hitobject(std::uint32_t time) -> std::pair<hitobject *, int>;
		auto count() -> std::uint32_t;

		auto operator[](int index) -> hitobject *;
		operator bool() const noexcept;
	};
}