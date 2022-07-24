#pragma once

#define OC_GLUE(x, y) x ## y
#define OC_MGLUE(x, y) OC_GLUE(x, y)

#define OC_PAD(n) char OC_MGLUE(_pad_, __LINE__) [n]

#define OC_UNS_PAD(n, type, name) \
	struct \
	{ \
		OC_PAD(n); \
		type name; \
	};

#if defined(OSU_CHEESE_DEBUG_BUILD)
	#include <cstdio>
	#include <iostream>
	#define DEBUG_PRINTF(fmt, ...) printf(fmt, __VA_ARGS__)
	#define DEBUG_WPRINTF(fmt, ...) wprintf(fmt, __VA_ARGS__)
    #define DEBUG_OPT_NULL(x) x
#else
	#define DEBUG_PRINTF(fmt, ...)
	#define DEBUG_WPRINTF(fmt, ...)
    #define DEBUG_OPT_NULL(x) nullptr
#endif

#define OC_IMGUI_HOVER_TXT(fmt, ...) \
	if (ImGui::IsItemHovered()) \
		ImGui::SetTooltip(fmt, __VA_ARGS__)