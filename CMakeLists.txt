cmake_minimum_required(VERSION 3.18)

project(osu-cheese CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Settings
option(DEFINE_DEBUG_MACRO "Enables the OSU_CHEESE_DEBUG_BUILD macro. Used for logging, testing, and dumping of information. (DOES NOT SET BUILD MODE TO DEBUG!)" OFF)
if (DEFINE_DEBUG_MACRO)
    add_compile_definitions(OSU_CHEESE_DEBUG_BUILD)
endif ()

# Libraries
add_subdirectory("dependencies/dncomlib")
add_subdirectory("dependencies/gl3w_out")
add_subdirectory("framework")
add_subdirectory("dependencies/imgui")

# Projects
option(BUILD_CLIENT "Build's the client library (DLL)." ON)
option(BUILD_LOADER "Build's the loader executable, this depends on the CLIENT project." ON)

if (BUILD_LOADER)
    set(BUILD_CLIENT ON) # Force client if loader build is enabled
    add_subdirectory("osu-cheese-client")
endif ()

if (BUILD_LOADER)
    add_subdirectory("osu-cheese-loader")
endif ()