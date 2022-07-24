cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(osu-cheese-framework)

add_library(${PROJECT_NAME} STATIC
	"sed/windows/smart_handle.hpp"
	"sed/windows/smart_handle.cpp"
	"sed/windows/suspend_guard.hpp"
	"sed/windows/suspend_guard.cpp"
	"sed/windows/protect_guard.hpp"
	"sed/windows/protect_guard.cpp"

	"sed/console.hpp"
	"sed/console.cpp"
	"sed/memory.hpp"
	"sed/memory.cpp"
	"sed/macro.hpp"
	"sed/strings.hpp"
)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
set(osu-cheese-framework-includes "${CMAKE_CURRENT_SOURCE_DIR}" PARENT_SCOPE)