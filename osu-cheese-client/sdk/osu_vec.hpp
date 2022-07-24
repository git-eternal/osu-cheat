#pragma once

#include <imgui.h>
#include <Windows.h>
#include <cmath>
#include <numbers>

namespace sdk
{
	struct vec2
	{
		float x, y;

		vec2()
			: x(0.f), y(0.f) {}

		vec2(float x_, float y_)
			: x(x_), y(y_) {}

		vec2(int x_, int y_)
			: x(float(x_)), y(float(y_)) {}

		auto distance(const vec2 & to) const -> float;

		auto view_to_field() const -> vec2;
		static auto view_to_field(vec2 & v) -> void;
		auto field_to_view() const -> vec2;
		static auto field_to_view(vec2 & v) -> void;
		
		auto magnitude() const -> float;

		auto normalize_towards(const vec2 & to) const -> vec2;
		auto forward_towards(const vec2 & to, float fwd_distance = 1.f) const -> vec2;

		auto forward(const float direction /* in degrees */, float fwd_distance = 1.f) -> vec2;
		auto forward(const vec2 & direction, float fwd_distance = 1.f) const -> vec2;

		auto dot_product(const vec2 & other) const -> float;

		// angle to another vector
		auto vec2vec_angle(const vec2 & other) const -> float;

		auto from_norm_to_deg() -> float; // lol

		static auto from_rad(float rad) -> vec2;
		static auto from_deg(float deg) -> vec2;

		auto operator ==(const vec2 & rhs) const noexcept -> bool;
		auto operator *(const float rhs) const noexcept -> vec2;
		auto operator +(const vec2 & rhs) const noexcept -> vec2;
		auto operator +(const float rhs) const noexcept -> vec2;

		operator ImVec2() const;
		operator POINT() const;
	};
}