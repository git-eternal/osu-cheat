cmake_minimum_required(VERSION 3.18)

project(dncomlib)

add_library(${PROJECT_NAME} STATIC
	"includes/dncomlib/unique_releasable.hpp"

	"includes/dncomlib/meta_host.hpp"
	"src/meta_host.cpp"

	"includes/dncomlib/unknown_enumerator.hpp"
	"src/unknown_enumerator.cpp"
	
	"includes/dncomlib/dncomlib.hpp"
	"src/dncomlib.cpp"
	
	"includes/dncomlib/runtime_info.hpp"
	"src/runtime_info.cpp"

	"includes/dncomlib/runtime_host.hpp"
	"src/runtime_host.cpp"

	"includes/dncomlib/app_domain.hpp"
	"src/app_domain.cpp"
)

target_include_directories(${PROJECT_NAME} PUBLIC "includes/")