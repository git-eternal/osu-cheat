#pragma once

#include <cstdint>
#include <cstddef>
#include <sed/macro.hpp>

namespace sdk
{
	union unk_game_info_a
	{
		struct { std::uint32_t beat_time; };

		OC_UNS_PAD(0x30, bool, is_playing)
		OC_UNS_PAD(0x38, std::uint32_t, gain_thing_idk)
		OC_UNS_PAD(0x40, std::uint32_t, game_tick)
	};
}