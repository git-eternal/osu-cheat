#pragma once

#include <cstdint>

namespace sed
{
	template <typename str1, typename str2>
	constexpr auto str_starts_with(const str1 * str, const str2 * with) -> const str1 *
	{
		if (!str || !*str || !with || !*with)
			return nullptr;

		while (*str == *with)
		{
			if (++str; !*++with)
				return str;
		}

		return nullptr;
	}

	template <typename T>
	constexpr auto str_len(const T * str) -> std::size_t
	{
		std::size_t len = 0;
		while (str[len++]);
		return len - 1;
	}
}