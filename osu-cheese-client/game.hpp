#pragma once

// Used for interfacing with the game's internals

#include "sdk/info_player.hpp"
#include "sdk/info_struct.hpp"
#include "sdk/position_info.hpp"
#include "sdk/raw_info.hpp"
#include "sdk/wnd_info.hpp"
#include "sdk/player_keys.hpp"
#include "sdk/hitobject.hpp"

namespace game
{
	inline sdk::pp_info_player_t   pp_info_player;
	inline sdk::pp_viewpos_info_t  pp_viewpos_info;
	inline sdk::unk_game_info_a  * p_game_info { nullptr };
	inline sdk::pp_raw_mode_info_t pp_raw_mode_info;
	inline sdk::pp_pplayer_keys_t  pp_pplayer_keys;
	inline sdk::pp_phitobject_t    pp_phitobject;
	inline HWND                    hwnd;

	auto initialize() -> bool;
}