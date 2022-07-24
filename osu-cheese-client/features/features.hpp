#include "aim_assist.hpp"
#include "relax.hpp"
#include "esp.hpp"

namespace features
{
	template <class... fts>
	struct _features_dispatch_pack
	{
		static auto on_tab_render() -> void
		{
			(fts::on_tab_render(), ...);
		}

		static auto on_wndproc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam, void * reserved) -> bool
		{
			return (fts::on_wndproc(hWnd, Msg, wParam, lParam, reserved) || ...);
		}

		static auto on_render() -> void
		{
			(fts::on_render(), ...);
		}

		static auto on_osu_set_raw_coords(sdk::vec2 * raw_coords) -> void
		{
			(fts::on_osu_set_raw_coords(raw_coords), ...);
		}

		static auto osu_set_field_coords_rebuilt(sdk::vec2 * out_coords) -> void
		{
			(fts::osu_set_field_coords_rebuilt(out_coords), ...);
		}
	};

	using dispatcher = _features_dispatch_pack<
		aim_assist,
		relax,
		esp
	>;
}