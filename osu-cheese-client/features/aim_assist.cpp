#include "aim_assist.hpp"

#include <imgui.h>
#include <sed/macro.hpp>

#include "../game.hpp"
#include "../sdk/osu_vec.hpp"

#include "../manager/gamefield_manager.hpp"

#include <algorithm>

auto features::aim_assist::on_tab_render() -> void
{
	if (!ImGui::BeginTabItem("Aim assist"))
		return;

	ImGui::Checkbox("Aim assist", &enable);
	OC_IMGUI_HOVER_TXT("Enable aim assistance - Corrects your aim to the neareast hit object when moving your cursor.");

	ImGui::Checkbox("Silent", &silent);
	OC_IMGUI_HOVER_TXT("\"Server\" side only visible aim assist.");

	ImGui::SliderFloat("FOV", &fov, 0.f, 800.f);
	OC_IMGUI_HOVER_TXT("Distance between your cursor and the hit object required before aim assistance activates. (0 = Global)");

	ImGui::SliderFloat("Direction FOV", &dir_fov, 0.f, 180.f);
	OC_IMGUI_HOVER_TXT("Directional angle field of view for aim assist to activate. (0 = full 360)");

	ImGui::SliderFloat("Safezone FOV", &safezone, 0.f, 800.f);
	OC_IMGUI_HOVER_TXT("Disables the aim assist when the player cursor is within the safezone. (0 = Never)");

	ImGui::SliderFloat("Assist Scale", &scaleassist, 0.f, 2.f);
	OC_IMGUI_HOVER_TXT("Scales the aim assist amount.");

	ImGui::SliderFloat("Target time offset ratio", &timeoffsetratio, 0.f, 1.f);
	OC_IMGUI_HOVER_TXT("Amount of time ahead on recognizing a hit object as active.");

	ImGui::Combo("Assist movement method", reinterpret_cast<int*>(&method), "Linear\0Directional Curve\0");

	switch (method)
	{
	case features::aim_assist::method_e::DIRECTIONAL_CURVE:
	{
		ImGui::SliderFloat("Player to Hit object ratio", &mdc_ho_ratio, 0.f, 1.f);
		OC_IMGUI_HOVER_TXT("Ratio to take in account from the player to the hit object when calculating the curve used for the aim assist.");

		ImGui::SliderFloat("Player to direction ratio", &mdc_pdir_ratio, 0.f, 1.f);
		OC_IMGUI_HOVER_TXT("Ratio to take in account from the player to the forward direction when calculating the curve used for the aim assist.");

		ImGui::SliderFloat("Midpoint ratio", &mdc_midpoint_ratio, 0.f, 1.f);
		OC_IMGUI_HOVER_TXT("Ratio to calculate the mid point for the curve of the aim assist.");

		ImGui::Combo("Follow method", reinterpret_cast<int*>(&mdc_method), "Hit object to Player direction\0Player direction to Hit object\0Dynamic (Auto)\0");
		OC_IMGUI_HOVER_TXT("Starting point of where to calculate the midpoint ratio from.");

		break;
	}

	default:
		break;
	}

	ImGui::Separator();

	ImGui::Checkbox("Visualize Aim FOV", &vis_fov);
	ImGui::Checkbox("Visualize Safezone FOV", &vis_safezonefov);

	ImGui::EndTabItem();
}

auto features::aim_assist::on_wndproc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam, void* reserved) -> bool
{
	return false;
}

auto features::aim_assist::on_render() -> void
{
	if (!enable || !game::pp_phitobject || !game::pp_info_player->async_complete || game::pp_info_player->is_replay_mode)
		return;

	auto draw = ImGui::GetBackgroundDrawList();

	if (vis_fov)
		draw->AddCircle(game::pp_viewpos_info->pos, fov, 0xFFFFFFFF);

	auto [ho, i] = game::pp_phitobject.get_coming_hitobject(game::p_game_info->beat_time);

	if (!ho)
		return;

	if (vis_safezonefov)
		draw->AddCircle(ho->position.field_to_view(), safezone, 0xFFFFFFFF);
}

auto features::aim_assist::on_osu_set_raw_coords(sdk::vec2* raw_coords) -> void
{
	if (auto _velocity = last_tick_point.distance(*raw_coords); _velocity != 0.f)
	{
		velocity = _velocity;
		player_direction = last_tick_point.normalize_towards(*raw_coords);
		last_tick_point = *raw_coords;
	}

	if (!enable || !game::pp_phitobject || !game::pp_info_player->async_complete || game::pp_info_player->is_replay_mode || !game::p_game_info->is_playing)
		return;

	auto [ho, i] = game::pp_phitobject.get_coming_hitobject(game::p_game_info->beat_time);

	if (!ho) return;

	// Check time offset
	if (timeoffsetratio != 0.f && i != 0)
	{
		auto prev = ho - 1;

		auto time_sub = ho->time.start - ((ho->time.start - prev->time.end) * (1.f - timeoffsetratio));

		if (game::p_game_info->beat_time < time_sub)
			return;
	}

	auto player_field_pos = game::pp_viewpos_info->pos.view_to_field();
	auto dist_to_ho = player_field_pos.distance(ho->position);

	// Check fov
	if (fov != 0.f && dist_to_ho > fov)
		return;

	// Check direction
	if (dir_fov != 0.f && player_direction.vec2vec_angle(player_field_pos.normalize_towards(ho->position)) > dir_fov)
		return;

	// Check if we're at the same point
	if (player_field_pos == ho->position)
		return;

	// Safezone override
	if (safezone != 0.f && dist_to_ho <= safezone)
		return;

	sdk::vec2 new_coords, target /* temporary for optimization, might be irrelevant in the future */;

	switch (method)
	{
	case method_e::LINEAR:
	{
		target = ho->position;
		break;
	}

	case method_e::DIRECTIONAL_CURVE:
	{
		auto p2ho_p = player_field_pos.forward_towards(ho->position, dist_to_ho * mdc_ho_ratio);
		auto p2dir_p = player_field_pos.forward(player_direction, dist_to_ho * mdc_pdir_ratio);

		sdk::vec2 start, end;

		switch (mdc_method)
		{
		case mdc_mpoint_method_e::HO_TO_PDIR:
		{
			start = p2ho_p;
			end = p2dir_p;
			break;
		}

		case mdc_mpoint_method_e::DYNAMIC: // TODO: implement this
		case mdc_mpoint_method_e::PDIR_TO_HO:
		{
			start = p2dir_p;
			end = p2ho_p;
			break;
		}

		default:
			break;
		}

		target = start.forward_towards(end, start.distance(end) * mdc_midpoint_ratio);
		break;
	}

	default:
		return;
	}

	new_coords = player_field_pos.forward_towards(target, std::clamp(velocity * scaleassist, 0.f, dist_to_ho)).field_to_view();
	last_tick_point = new_coords; // update last tick point to our new coordinates since the new coords will now be our current point for the tick this also prevents over calculating the velocity
	*raw_coords = new_coords; // update the ingame coordinates

	if (!game::pp_raw_mode_info->is_raw)
	{
		POINT pscr = new_coords;
		ClientToScreen(game::hwnd, &pscr);
		SetCursorPos(pscr.x, pscr.y);
	}

	return;
}

auto features::aim_assist::osu_set_field_coords_rebuilt(sdk::vec2* out_coords) -> void
{
}

auto features::aim_assist::run_aim_assist() -> void
{
}
