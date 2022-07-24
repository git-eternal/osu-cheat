cmake_minimum_required(VERSION 3.18)

add_library(imgui
	"imgui.cpp"
	"imgui_impl_opengl3.cpp"
	"imgui_impl_win32.cpp"
	"imgui_tables.cpp"
	"imgui_widgets.cpp"
	"imgui_draw.cpp"
)

set(imgui-includes "${CMAKE_CURRENT_SOURCE_DIR}" PARENT_SCOPE)
set_target_properties(imgui PROPERTIES LINKER_LANGUAGE CXX)