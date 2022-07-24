#include "game.hpp"

#include <sed/console.hpp>
#include <sed/memory.hpp>
#include <sed/strings.hpp>

template <sed::ida_pattern pattern, typename T>
static auto pattern_scan_helper(const char * name, T & out, int sz32 = 0, bool is_abs = true) -> bool
{
	DEBUG_PRINTF("\n[+] Searching for %s...", name);
	auto res = sed::pattern_scan<pattern, void>();
	DEBUG_PRINTF(" 0x%p", res);

	if (!res)
		return false;

	out = reinterpret_cast<T>(res);

	if (sz32)
	{
		out = reinterpret_cast<T>(is_abs ? sed::abs32(out, sz32) : sed::rel2abs32(out, sz32));

		DEBUG_PRINTF(" -> 0x%p", out);
	}
	
	return true;
}

auto game::initialize() -> bool
{
	DEBUG_PRINTF("\n[+] Loading game information..."
				 "\n[+] Obtaining handle to window...");

	while (!game::hwnd)
	{
		EnumWindows([](HWND hwnd, LPARAM lparam) -> BOOL
		{
			DWORD pid { 0 };
			GetWindowThreadProcessId(hwnd, &pid);
			wchar_t text_buffer[MAX_PATH] { 0 };
			constexpr auto text_buffer_sz = ARRAYSIZE(text_buffer);

			if (pid != GetCurrentProcessId() 
				|| !GetWindowTextW(hwnd, text_buffer, text_buffer_sz)
				|| !sed::str_starts_with(text_buffer, L"osu!")
				|| !RealGetWindowClassW(hwnd, text_buffer, text_buffer_sz)
				|| sed::str_starts_with(text_buffer, L"Console")
			) {
				return TRUE;
			}
			
			game::hwnd = hwnd;
			return FALSE;
		}, NULL);

		Sleep(800);
	}

	DEBUG_WPRINTF(L"0x%p", game::hwnd);

	if (!pattern_scan_helper<"DB 05 ?? ?? ?? ?? D9 5D F8">                                              (DEBUG_OPT_NULL("game::p_game_info"),      game::p_game_info,          0x6)
	||  !pattern_scan_helper<"8b 05 ?? ?? ?? ?? d9 40 ?? 8b 15">                                        (DEBUG_OPT_NULL("game::pp_viewpos_info"),  game::pp_viewpos_info.ptr,  0x6)
	||  !pattern_scan_helper<"FF 50 0C 8B D8 8B 15">                                                    (DEBUG_OPT_NULL("game::pp_info_player"),   game::pp_info_player.ptr,   0xB)
	||  !pattern_scan_helper<"8b ec 83 ec ?? a1 ?? ?? ?? ?? 85 c0 74">                                  (DEBUG_OPT_NULL("game::pp_raw_mode_info"), game::pp_raw_mode_info.ptr, 0xA)
	||  !pattern_scan_helper<"8b 0d ?? ?? ?? ?? 8b 55 ?? 39 09 ff 15 ?? ?? ?? ?? 85 c0 74">             (DEBUG_OPT_NULL("game::pp_pplayer_keys"),  game::pp_pplayer_keys.ptr,  0x6)
	||  !pattern_scan_helper<"8B 0D ?? ?? ?? ?? 85 C9 75 ?? B8 ?? ?? ?? ?? EB ?? 8B 01 8B 40 ?? FF 50"> (DEBUG_OPT_NULL("game::pp_phitobject"),    game::pp_phitobject.ptr,    0x6)
	) {
		return false;
	}

	return true;
}
