#pragma once

#include <sed/macro.hpp>
#include <sed/memory.hpp>

namespace sdk
{
	struct player_key
	{
	private:
		char unk[0xC];
	public:
		DWORD vkey;
	};
	static_assert(sizeof(player_key) == 0x10, "invalid player_key struct size!");

	struct player_keys_t
	{
	private:
		char pad[0x8];
	public:
		struct
		{
			player_key left_click;
			player_key right_click;
			player_key smoke;
		} osu;

		player_key other_keys[];

		//struct
		//{
		//	player_key drum_centre_left;
		//	player_key drum_centre_right;
		//} taiko;
		// not doing this now
	};

	struct _PTRPK_
	{
	private:
		char pad[0x8];
	public:
		player_keys_t *ppk;
	};

	class pp_pplayer_keys_t : public sed::basic_ptrptr<_PTRPK_> {};
}