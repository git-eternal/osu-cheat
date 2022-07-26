#include "hooks.hpp"

#include <Windows.h>
#include <tuple>
#include <sed/macro.hpp>
#include <sed/memory.hpp>
#include <sed/strings.hpp>

#include "manager/gamefield_manager.hpp"
#include <GL/gl3w.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_win32.h>
#include <imgui.h>
#include "game.hpp"
#include "menu.hpp"
#include "features/features.hpp"

enum class CallWindowProc_variant : int
{
	A = 0,
	MOUSE = A,
	W = 1,
	KEY = W
};

static auto CALLBACK CallWindowProc_hook(CallWindowProc_variant variant, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) -> bool
{
	if (oc::menu::wndproc(hWnd, Msg, wParam, lParam))
		return true;

	if (variant == CallWindowProc_variant::MOUSE)
	{
		if (features::dispatcher::on_wndproc(hWnd, Msg, wParam, lParam, nullptr))
			return true;
	}

	return false;
}

static decltype(CallWindowProcA)* CallWindowProcA_target = CallWindowProcA;
static decltype(CallWindowProcW)* CallWindowProcW_target = CallWindowProcW;

static auto [[naked]] CallWindowProc_proxy(WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) -> LRESULT
{
	__asm
	{
		push ebp
		mov ebp, esp
		push eax
		push[ebp + 24]
		push[ebp + 20]
		push[ebp + 16]
		push[ebp + 12]
		push eax
		call CallWindowProc_hook;
		test al, al
			jnz LBL_CWP_SKIP_ORIGINAL
			pop eax
			test al, al
			jnz LBL_CWP_VARIANT_A

			LBL_CWP_VARIANT_W :
		mov eax, CallWindowProcA_target
			jmp LBL_CWP_CALL_ORIGINAL

			// Call A variant
			LBL_CWP_VARIANT_A :
		mov eax, CallWindowProcW_target
			jmp LBL_CWP_CALL_ORIGINAL

			// Call original
			LBL_CWP_SKIP_ORIGINAL :
		pop eax
			pop ebp
			mov eax, 1
			ret 0x14

			LBL_CWP_CALL_ORIGINAL :
			lea eax, [eax + 5]
			jmp eax
	}
}

static auto [[naked]] CallWindowProcW_proxy(WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) -> LRESULT
{
	__asm
	{
		mov eax, CallWindowProc_variant::W
		jmp CallWindowProc_proxy
	};
}

static auto [[naked]] CallWindowProcA_proxy(WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) -> LRESULT
{
	__asm
	{
		mov eax, CallWindowProc_variant::A // can just be xor eax, eax but this looks more verbose...
		jmp CallWindowProc_proxy
	};
}

static auto WINAPI gdi32full_SwapBuffers_hook(HDC hdc) -> void
{
	if (static bool init = true; init)
	{
		//HGLRC ctx = wglCreateContext(hdc);
		gl3wInit();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui::GetIO().IniFilename = nullptr;
		ImGui_ImplWin32_Init(WindowFromDC(hdc));
		ImGui_ImplOpenGL3_Init();
		init = false;
	}

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	features::dispatcher::on_render();
	oc::menu::render();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

static decltype(SwapBuffers)* gdi32full_SwapBuffers_target{ nullptr };

static auto [[naked]] gdi32full_SwapBuffers_proxy(HDC hdc) -> BOOL
{
	__asm
	{
		push ebp
		mov ebp, esp

		push[ebp + 8]
		call gdi32full_SwapBuffers_hook

		mov eax, gdi32full_SwapBuffers_target
		lea eax, [eax + 5]
		jmp eax
	}
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static auto WINAPI SetWindowTextW_hook(HWND hWnd, LPCWSTR lpString) -> void
{
	if (!sed::str_starts_with(lpString, L"osu!"))
		return;

	// TODO: dynamically load these values, we already have the sig stop being lazy.
	RECT wr{};
	GetClientRect(hWnd, &wr);

	manager::game_field::resize(wr.right, wr.bottom);
}

static decltype(SetWindowTextW)* SetWindowTextW_target = SetWindowTextW;

static auto [[naked]] SetWindowTextW_proxy(HWND hWnd, LPCWSTR lpString) -> BOOL
{
	__asm
	{
		push ebp
		mov ebp, esp

		push[ebp + 12]
		push[ebp + 8]
		call SetWindowTextW_hook

		mov eax, SetWindowTextW_target // we cant do load effective address because clang (msvc?) does some funny
		lea eax, [eax + 5]			   // things like using the ecx register causing the ctx to get corrupted
		jmp eax
	}
}

// Name: #=zP4nKUSUPOssQxNF6$g==::#=z9UGmDcmwjvbl

static auto __fastcall osu_set_field_coords_rebuilt(void* ecx, sdk::vec2* out_coords) -> void
{
	// Can do psilent here by setting the field coordinates
	*out_coords = game::pp_viewpos_info->pos.view_to_field();
	features::dispatcher::osu_set_field_coords_rebuilt(out_coords);
}

static auto [[naked]] osu_set_field_coords_proxy(void* ecx, sdk::vec2* out_coords) -> void
{
	__asm
	{
		push esi
		sub esp, 8
		call osu_set_field_coords_rebuilt
		add esp, 8
		pop esi
		ret 8
	}
}

static auto __fastcall osu_set_raw_coords_rebuilt(sdk::vec2* raw_coords) -> void
{
	if (oc::menu::visible && game::pp_raw_mode_info->is_raw)
	{
		*raw_coords = oc::menu::freeze_view_point;
		return;
	}

	// TODO: actually rebuild this function from assembly
	// but seems like there are other functions that does our
	// job for us so we don't have to worry about it but it's
	// a better idea to actually rebuild it and restore functionality
	features::dispatcher::on_osu_set_raw_coords(raw_coords);
}

static auto [[naked]] osu_set_raw_coords_proxy() -> void
{
	__asm
	{
		mov ecx, [ebp - 0x34]
		add ecx, 0x24
		jmp osu_set_raw_coords_rebuilt
	};
}

void* osu_ac_flag_original{ nullptr };

static auto __stdcall osu_ac_flag() -> void
{
	DEBUG_PRINTF("\n[!] Anti-cheat flag triggered!");
}

static auto [[naked]] osu_ac_flag_proxy() -> void
{
	__asm
	{
		call osu_ac_flag
		jmp osu_ac_flag_original
	};
}

// TODO: write a small ghetto hooking class so we dont get ugly chunks of code like this

using mouse_get_position_t = void(__fastcall*)(std::uintptr_t);

mouse_get_position_t* mouse_get_position_callptr = nullptr;
mouse_get_position_t mouse_get_position_original = nullptr;

auto __fastcall mouse_get_position_hook(std::uintptr_t ecx) -> void
{
	if (!ecx)
		return;

	if (oc::menu::visible)
	{
		POINT p = oc::menu::freeze_view_point;
		ClientToScreen(game::hwnd, &p);
		*reinterpret_cast<LPPOINT>(ecx + 0x4) = p;
		return;
	}

	return mouse_get_position_original(ecx);
}

using hook_instances_t = std::vector<std::unique_ptr<sed::mempatch_interface>>;
static hook_instances_t hook_instances;

auto hooks::install() -> bool
{
	DEBUG_PRINTF("\n[+] Installing hooks..."
		"\n[+] Importing gdi32full.SwapBuffers...");

	// TODO: port these signatures

	// Swap buffers
	gdi32full_SwapBuffers_target = reinterpret_cast<decltype(gdi32full_SwapBuffers_target)>(GetProcAddress(GetModuleHandleW(L"gdi32full.dll"), "SwapBuffers"));

	if (!gdi32full_SwapBuffers_target)
	{
		DEBUG_PRINTF("\n[!] Failed to import gdi32full.SwapBuffers");
		return false;
	}

	DEBUG_PRINTF(" 0x%p", gdi32full_SwapBuffers_target);

	// Set field coordinates

	DEBUG_PRINTF("\n[+] Searching for osu_set_field_coords... ");

	auto osu_set_field_coords_target = sed::pattern_scan<"56 83 EC ?? 8B F2 8D 41 ?? D9 00 D9 40 ?? d9 44", void>();

	if (!osu_set_field_coords_target)
	{
		DEBUG_PRINTF("\n[!] Failed to look for osu_set_field_coords!");
		return false;
	}

	DEBUG_PRINTF(" 0x%p", osu_set_field_coords_target);

	// Set raw input coordinates
	DEBUG_PRINTF("\n[+] Searching for osu_set_raw_coords...");

	auto cond_raw_coords = sed::pattern_scan<"74 ?? 8b 75 ?? 83 c6 ?? 8b 45", std::uint8_t>();

	if (!cond_raw_coords)
	{
		DEBUG_PRINTF("\n[!] Failed to look for osu_set_raw_coords!");
		return false;
	}

	DEBUG_PRINTF(" 0x%p", cond_raw_coords);
	auto cond_raw_rel8 = *(cond_raw_coords + 1);
	DEBUG_PRINTF("\n[+] raw coords rel8 and abs -> 0x%x", cond_raw_rel8);
	auto cond_raw_abs = cond_raw_coords + 2 + cond_raw_rel8;
	DEBUG_PRINTF(" -> 0x%p", cond_raw_abs);

	// Mouse get position
	DEBUG_PRINTF("\n[+] Searching for mouse.get_Position's call to GetCursorPos...");
	auto mouse_get_position_target = sed::pattern_scan<"FF 15 ?? ?? ?? ?? 8B 56 04 8B 46 08 89 17 89 47 04 5E 5F 5D", std::uint8_t>();

	if (!mouse_get_position_target)
	{
		DEBUG_PRINTF("\n[!] Failed to locate call to GetCursorPos!");
		return false;
	}

	DEBUG_PRINTF(" 0x%p", mouse_get_position_target);
	mouse_get_position_callptr = *reinterpret_cast<decltype(mouse_get_position_callptr)*>(mouse_get_position_target + 2);
	DEBUG_PRINTF(" -> 0x%p (callptr)", mouse_get_position_callptr);

	// Anticheat flag sig
	DEBUG_PRINTF("\n[+] Searching for ac_flag_call...");
	auto ac_flag_call = sed::pattern_scan<"E8 ?? ?? ?? ?? 83 C4 ?? 89 45 ?? 8B 4D ?? 8B 11 8B 42 ?? 89 45 ?? 0F B6 4D", void>(); // TODO: this doesn't necessarily need to be scanned through regions

	if (!ac_flag_call)
	{
		DEBUG_PRINTF("\n[!] Failed to look for ac_flag_call");
		return false;
}

	DEBUG_PRINTF(" 0x%p", ac_flag_call);
	osu_ac_flag_original = reinterpret_cast<void*>(sed::rel2abs32(reinterpret_cast<void*>(ac_flag_call), 0x5));
	DEBUG_PRINTF(" -> 0x%p", osu_ac_flag_original);

#if 0
	// WindowProc
	DEBUG_PRINTF("\n[+] Loading WindowProcedure...");
	auto wp = GetWindowLongPtrA(game::hwnd, GWLP_WNDPROC);
	DEBUG_PRINTF(" 0x%p", wp);
	auto wp_jmp = wp + 5;
	DEBUG_PRINTF(" -> rel(0x%p)", *reinterpret_cast<void**>(wp_jmp + 1));
	WindowProc_original = reinterpret_cast<void*>(sed::rel2abs32(reinterpret_cast<void*>(wp_jmp), 0x5));
	DEBUG_PRINTF(" -> abs(0x%p)", WindowProc_original);
#endif

#define _OC_ADD_HOOK_INSTANCE(patchtype, from, to) \
		_instances.push_back(std::make_unique<sed::mempatch_##patchtype##r32>(reinterpret_cast<void *>(from), reinterpret_cast<void *>(to)))

	hook_instances_t _instances {};

	_OC_ADD_HOOK_INSTANCE(jmp, CallWindowProcA_target, CallWindowProcA_proxy);
	_OC_ADD_HOOK_INSTANCE(jmp, CallWindowProcW_target, CallWindowProcW_proxy);
	_OC_ADD_HOOK_INSTANCE(jmp, SetWindowTextW, SetWindowTextW_proxy);
	_OC_ADD_HOOK_INSTANCE(jmp, gdi32full_SwapBuffers_target, gdi32full_SwapBuffers_proxy);
	_OC_ADD_HOOK_INSTANCE(jmp, osu_set_field_coords_target, osu_set_field_coords_proxy);
	_OC_ADD_HOOK_INSTANCE(call, cond_raw_coords, osu_set_raw_coords_proxy);
	_OC_ADD_HOOK_INSTANCE(jmp, cond_raw_coords + 5, cond_raw_abs);
	_OC_ADD_HOOK_INSTANCE(call, ac_flag_call, osu_ac_flag_proxy);

#undef _OC_ADD_HOOK_INSTANCE

	for (auto& h : _instances)
	{
		if (!h->patch())
		{
			DEBUG_PRINTF("\n[!] Failed to install hooks!");
			return false;
		}
	}

	hook_instances = std::move(_instances);

	mouse_get_position_original = *mouse_get_position_callptr;
	*mouse_get_position_callptr = mouse_get_position_hook;

	return true;
}

auto hooks::uninstall() -> bool
{
	if (hook_instances.empty())
		return false;

	for (auto& h : hook_instances)
	{
		if (!h->restore())
		{
			DEBUG_PRINTF("\n[!] Failed to uninstall hook!");
		}
	}

	*mouse_get_position_callptr = mouse_get_position_original;

	hook_instances.clear();

	return true;
}
