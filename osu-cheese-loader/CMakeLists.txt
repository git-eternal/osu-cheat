cmake_minimum_required(VERSION 3.18)

project(osu-cheese-loader CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(${PROJECT_NAME}
    "loader_main.cpp"
    "loader_main.hpp"
)

if (EXISTS "client_path.hpp")
    add_executable(${PROJECT_NAME} "client_path.hpp")
endif ()

target_link_libraries(${PROJECT_NAME} PRIVATE
    osu-cheese-framework
    osu-cheese-client
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${osu-cheese-framework-includes}
)